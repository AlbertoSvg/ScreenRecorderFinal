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

elseif (APPLE)


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
#add_executable(ScreenRecorderProject audio_test/main2.cpp audio_test/AudioRecorder.cpp audio_test/AudioRecorder.h)

if (WIN32)

    target_link_libraries(${PROJECT_NAME}  PkgConfig::LIBAV)

elseif (APPLE)

else ()
    target_link_libraries(${PROJECT_NAME} PkgConfig::LIBAV)
endif ()