include(cmake/DependenciesLinux.cmake)
include(cmake/DependenciesPSP.cmake)

macro(spelunky_psp_detect_platform)

    if (DEFINED PLATFORM_PSP)
        set(SPELUNKY_PSP_PLATFORM "PSP")
        set(SPELUNKY_PSP_PLATFORM_PSP TRUE)
    endif()
    
    if (DEFINED SPELUNKY_PSP_PLATFORM)
        # Nothing to do - SPELUNKY_PSP_PLATFORM was already defined in the toolchain file.
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(SPELUNKY_PSP_PLATFORM "Linux")
        set(SPELUNKY_PSP_PLATFORM_LINUX TRUE)
    else ()
        message(FATAL_ERROR "Your platform (${CMAKE_SYSTEM_NAME}) is not supported")
    endif ()

    message(STATUS "Target platform is: ${SPELUNKY_PSP_PLATFORM}")
endmacro()

macro(spelunky_psp_add_platform_dependencies)
    if (${SPELUNKY_PSP_PLATFORM} STREQUAL PSP)
        add_psp_dependencies()
    elseif (${SPELUNKY_PSP_PLATFORM} STREQUAL Linux)
        add_linux_dependencies()
    endif ()
endmacro()

macro(spelunky_psp_post_build)
    add_custom_command(
        TARGET Spelunky_PSP
        POST_BUILD COMMAND
        "psp-strip" "$<TARGET_FILE:Spelunky_PSP>"
        COMMENT "Stripping binary"
    )

    if (${SPELUNKY_PSP_PLATFORM} STREQUAL PSP)
        create_pbp_file(TARGET Spelunky_PSP
            ICON_PATH ${ASSETS_PATH}/metadata/icon.png
            BACKGROUND_PATH ${ASSETS_PATH}/metadata/background.png
        )
    elseif (${SPELUNKY_PSP_PLATFORM} STREQUAL Linux)
        spelunky_psp_post_build_linux()
    endif ()
endmacro()
