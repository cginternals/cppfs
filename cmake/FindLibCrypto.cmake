
# LIBCRYPTO_LIBRARY

include(FindPackageHandleStandardArgs)

find_library(LIBCRYPTO_LIBRARY NAMES crypto)

find_package_handle_standard_args(LibCrypto DEFAULT_MSG LIBCRYPTO_LIBRARY)
mark_as_advanced(LIBCRYPTO_LIBRARY)
