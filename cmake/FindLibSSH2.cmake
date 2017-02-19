
# LIBSSH2_FOUND
# LIBSSH2_INCLUDE_DIR
# LIBSSH2_LIBRARY

include(FindPackageHandleStandardArgs)

find_path(LIBSSH2_INCLUDE_DIR libssh2.h)
find_library(LIBSSH2_LIBRARY NAMES ssh2 libssh2 PATH_SUFFIXES /lib)

find_package_handle_standard_args(LibSSH2 DEFAULT_MSG LIBSSH2_INCLUDE_DIR LIBSSH2_LIBRARY)
mark_as_advanced(LIBSSH2_INCLUDE_DIR LIBSSH2_LIBRARY)
