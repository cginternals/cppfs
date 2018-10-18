
# 
# Default dependencies for the runtime-package
# 

# Install 3rd-party runtime dependencies into runtime-component
# install(FILES ... COMPONENT runtime)


# 
# Full dependencies for self-contained packages
# 

if(OPTION_SELF_CONTAINED)

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")

        # Install 3rd-party runtime dependencies into runtime-component
        # install(FILES ... COMPONENT runtime)

        find_file(DLLS_CPPASSIST cppassist.dll)

        set(DLLS
            ${DLLS_CPPASSIST}
        )
        set(PLATFORMS
            ${DLLS_WIN}
        )
        install(FILES ${DLLS} DESTINATION ${INSTALL_BIN} COMPONENT examples)
        install(FILES ${PLATFORMS} DESTINATION ${INSTALL_BIN}/platforms COMPONENT examples)

    endif()

endif()
