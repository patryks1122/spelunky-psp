add_library(Logger STATIC
        src/log.c
        interface/logger/log.h
)

target_include_directories(Logger PRIVATE interface/logger INTERFACE interface)

if(SPELUNKY_PSP_PLATFORM_LINUX)
    # Don't use ANSI color codes on PSP, as when printed in PPSSPP's output, they break log message.
    target_compile_definitions(Logger PRIVATE LOG_USE_COLOR SPELUNKY_PSP_PLATFORM_LINUX)
endif()
