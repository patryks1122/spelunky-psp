if (SPELUNKY_PSP_PLATFORM_LINUX)
    add_subdirectory(glad_opengl_1.3)
elseif(SPELUNKY_PSP_PLATFORM_PSP)
    add_subdirectory(glad_opengl_es_1.0)
else()
    message(FATAL_ERROR "Could not match platform")
endif ()
