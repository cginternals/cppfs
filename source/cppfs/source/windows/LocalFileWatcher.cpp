
#include <cppfs/windows/LocalFileWatcher.h>

#include <algorithm>

#include <Windows.h>

#include <cppfs/windows/LocalFileSystem.h>


namespace
{
    struct LocalWatcher
    {
        OVERLAPPED ovl;
        std::shared_ptr<VOID> event;
        DWORD buffer[16384];
    };

    // NB! Using a std::mutex resulted in spurious deadlocks using VS2017, whilst
    // using CRITICAL_SECTION works fine.
    struct ScopedCriticalSection
    {
        LPCRITICAL_SECTION m_pcs;
        ScopedCriticalSection(LPCRITICAL_SECTION pcs) : m_pcs(pcs) {
            ::EnterCriticalSection(m_pcs);
        }
        ~ScopedCriticalSection() {
            ::LeaveCriticalSection(m_pcs);
        }
    };
}


namespace cppfs
{


LocalFileWatcher::LocalFileWatcher(FileWatcher * fileWatcher, std::shared_ptr<LocalFileSystem> fs)
: AbstractFileWatcherBackend(fileWatcher)
, m_fs(fs)
, m_waitStopEvent(::CreateEvent(NULL, TRUE, FALSE, NULL), ::CloseHandle)
, m_watchersCS(new CRITICAL_SECTION())
{
    ::InitializeCriticalSection((LPCRITICAL_SECTION)m_watchersCS.get());
}

LocalFileWatcher::~LocalFileWatcher()
{
    ::DeleteCriticalSection((LPCRITICAL_SECTION)m_watchersCS.get());
    m_watchers.clear();
}

AbstractFileSystem * LocalFileWatcher::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileWatcher::add(FileHandle & dir, unsigned int events, RecursiveMode recursive)
{
    // Open directory
    HANDLE hDir = ::CreateFileA(
        dir.path().c_str(),                                     // Pointer to the directory name
        FILE_LIST_DIRECTORY,                                    // Access (read/write) mode
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // File share mode
        NULL,                                                   // Security descriptor
        OPEN_EXISTING,                                          // How to create
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,      // File attributes
        NULL                                                    // File with attributes to copy
    );

    // Check if directory could be opened
    if (hDir == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("failed to create directory listener");
    }

    std::shared_ptr<void> hDirectory(hDir, ::CloseHandle);
    std::shared_ptr<void> hEvent(::CreateEvent(NULL, TRUE, FALSE, NULL), ::CloseHandle);
    if (!hEvent)
        throw std::runtime_error("failed creating wait event");

    auto lw = std::make_shared<LocalWatcher>();
    lw->event = std::move(hEvent);
    lw->ovl.hEvent = lw->event.get();

    Watcher w;
    w.handle    = std::move(hDirectory);
    w.dir       = dir;
    w.events    = events;
    w.recursive = recursive;
    w.platform  = std::move(lw);

    ::SetEvent(m_waitStopEvent.get());
    ScopedCriticalSection lock((LPCRITICAL_SECTION)m_watchersCS.get());
    m_watchers.push_back(std::move(w));
}

void LocalFileWatcher::watch(int timeout)
{
    ScopedCriticalSection lock((LPCRITICAL_SECTION)m_watchersCS.get());
    std::vector<HANDLE> waitHandles;
    waitHandles.push_back(m_waitStopEvent.get());
    for (auto it : m_watchers) {
        auto lw = reinterpret_cast<LocalWatcher*>(it.platform.get());
        waitHandles.push_back(lw->event.get());
        DWORD flags = 0;
        if (it.events & FileCreated)     flags |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;
        if (it.events & FileRemoved)     flags |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;
        if (it.events & FileModified)    flags |= FILE_NOTIFY_CHANGE_LAST_WRITE;
        if (it.events & FileAttrChanged) flags |= FILE_NOTIFY_CHANGE_ATTRIBUTES;

        DWORD dwBytes = 0;
        if (!::GetOverlappedResult(it.handle.get(), &lw->ovl, &dwBytes, FALSE)) {
            auto error = GetLastError();
            if (error == ERROR_IO_INCOMPLETE)
                continue;
        }

        if (!::ReadDirectoryChangesW(
            it.handle.get(),
            lw->buffer,
            sizeof(lw->buffer),
            (BOOL)it.recursive,
            flags,
            NULL,
            &lw->ovl,
            NULL)) {
            auto error = GetLastError();
            if (error != ERROR_IO_PENDING) {
                throw std::runtime_error("Error calling ReadDirectoryChangesW: " + std::to_string(error));
            }
        }
    }

    auto waitResult = ::WaitForMultipleObjects(
        waitHandles.size(),
        waitHandles.data(),
        FALSE,
        timeout >= 0 ? timeout : INFINITE);
    if (waitResult == WAIT_TIMEOUT) {
        return;
    }
    if (waitResult == WAIT_OBJECT_0) {
        ::ResetEvent(m_waitStopEvent.get());
        return;
    }

    auto index = waitResult - (WAIT_OBJECT_0 + 1);
    if (index < 0 || index >= int(m_watchers.size())) {
        throw std::runtime_error("Wait returned result: " + std::to_string(waitResult));
    }

    Watcher& w = m_watchers[index];
    auto lw = reinterpret_cast<LocalWatcher*>(w.platform.get());
    ::ResetEvent(lw->event.get());
    DWORD dwBytes = 0;
    if (::GetOverlappedResult(w.handle.get(), &lw->ovl, &dwBytes, FALSE) && dwBytes > 0) {
        char fileName[32768];
        char* p = (char*)lw->buffer;
        for (;;) {
            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(p);
            int ret = ::WideCharToMultiByte(CP_ACP,
                0,
                fni->FileName,
                fni->FileNameLength / sizeof(WCHAR),
                fileName,
                sizeof(fileName),
                NULL,
                NULL);
            if (ret != 0) {
                std::string fname(fileName, fileName + ret);
                std::replace(fname.begin(), fname.end(), '\\', '/');
                FileEvent eventType = (FileEvent)0;
                switch (fni->Action) {
                case FILE_ACTION_ADDED:
                    eventType = FileCreated;
                    break;
                case FILE_ACTION_REMOVED:
                    eventType = FileRemoved;
                    break;
                case FILE_ACTION_MODIFIED:
                case FILE_ACTION_RENAMED_NEW_NAME:
                    eventType = FileModified;
                    break;
                default:
                    break;
                }
                if (w.events & eventType) {
                    // Get file handle
                    FileHandle fh = w.dir.open(fname);

                    // Invoke callback function
                    onFileEvent(fh, eventType);
                }
            }
            if (fni->NextEntryOffset == 0)
                break;
            p += fni->NextEntryOffset;
        }
    }
}

} // namespace cppfs
