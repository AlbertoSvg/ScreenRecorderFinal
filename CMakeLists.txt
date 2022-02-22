cmake_minimum_required(VERSION 3.17)
project(ScreenRecorderProject)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavdevice
        libavfilter
        libavformat
        libavcodec
        libswresample
        libswscale
        libavutil
        )

else ()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
            libavdevice
            libavfilter
            libavformat
            libavcodec
            libswresample
            libswscale
            libavutil
            )

endif ()

add_executable(ScreenRecorderProject main.cpp Recorder.cpp Recorder.h)

if (WIN32)

    target_link_libraries(${PROJECT_NAME} PkgConfig::LIBAV)
else ()
    target_link_libraries(${PROJECT_NAME} PkgConfig::LIBAV)
endif ()