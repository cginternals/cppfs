
#include <cppfs/windows/LocalFileWatcher.h>

#include <cppfs/windows/LocalFileSystem.h>

#include <Windows.h>

#include <algorithm>

namespace
{
    struct LocalWatcher
    {
        OVERLAPPED ovl;
        std::shared_ptr<VOID> event;
        DWORD buffer[16384];
    };
}

namespace cppfs
{


LocalFileWatcher::LocalFileWatcher(FileWatcher * fileWatcher, std::shared_ptr<LocalFileSystem> fs)
: AbstractFileWatcherBackend(fileWatcher)
, m_fs(fs)
{
}

LocalFileWatcher::~LocalFileWatcher()
{
    m_watchers.clear();
}

AbstractFileSystem * LocalFileWatcher::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileWatcher::add(FileHandle & fh, unsigned int events, RecursiveMode recursive)
{
    HANDLE hDir = ::CreateFileA(fh.path().c_str(),       // pointer to the directory name
                                      FILE_LIST_DIRECTORY,  // access (read/write) mode
                                      FILE_SHARE_READ       // share mode
                                      | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                      NULL,                       // security descriptor
                                      OPEN_EXISTING,              // how to create
                                      FILE_FLAG_BACKUP_SEMANTICS  // file attributes
                                      | FILE_FLAG_OVERLAPPED,
                                      NULL);  // file with attributes to copy
    if (hDir == INVALID_HANDLE_VALUE)
        throw std::runtime_error("failed to create directory listener");

    std::shared_ptr<void> hDirectory(hDir, ::CloseHandle);

    std::shared_ptr<VOID> hEvent(::CreateEvent(NULL, TRUE, FALSE, NULL), ::CloseHandle);
    if (!hEvent)
        throw std::runtime_error("failed creating wait event");

    auto lw = std::make_shared<LocalWatcher>();
    lw->event = std::move(hEvent);
    lw->ovl.hEvent = lw->event.get();

    Watcher w;
    w.handle = std::move(hDirectory);
    w.fileHandle = fh;
    w.events = events;
    w.recursive = recursive;
    w.platform = std::move(lw);
    m_watchers.push_back(std::move(w));
}

void LocalFileWatcher::watch(int timeoutMilliSeconds)
{
    std::vector<HANDLE> waitHandles;
    for (auto it : m_watchers) {
        auto lw = reinterpret_cast<LocalWatcher*>(it.platform.get());
        waitHandles.push_back(lw->event.get());
        DWORD flags = 0;
        if (it.events & FileCreated)  flags |= FILE_NOTIFY_CHANGE_FILE_NAME;
        if (it.events & FileRemoved)  flags |= FILE_NOTIFY_CHANGE_FILE_NAME;
        if (it.events & FileModified) flags |= FILE_NOTIFY_CHANGE_LAST_WRITE;

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

    for (;;) {
        auto waitResult = ::WaitForMultipleObjects(waitHandles.size(), waitHandles.data(), FALSE, timeoutMilliSeconds);
        if (waitResult == WAIT_TIMEOUT) {
            return;
        }

        auto index = waitResult - WAIT_OBJECT_0;
        if (index < 0 || index >= int(m_watchers.size()))
            continue;

        Watcher& w = m_watchers[index];
        auto lw = reinterpret_cast<LocalWatcher*>(w.platform.get());
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
                        eventType = FileModified;
                        break;
                    default:
                        break;
                    }
                    if (w.events & eventType) {
                        // Get file handle
                        FileHandle fh = w.fileHandle.open(fname);
                        // Invoke callback function
                        onFileEvent(fh, eventType);
                    }
                }
                if (fni->NextEntryOffset == 0)
                    break;
                p += fni->NextEntryOffset;
            }
        }
        break;
    }
}


} // namespace cppfs
