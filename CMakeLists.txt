cmake_minimum_required(VERSION 3.13)

set(CAMERA "ffmpeg" CACHE STRING "Camera Backend")
set_property(CACHE CAMERA PROPERTY STRINGS none ueye realsense pylon ffmpeg)

set(COMPRESSOR "jpeg" CACHE STRING "Compressor")
set_property(CACHE COMPRESSOR PROPERTY STRINGS jpeg webp)

project(${CAMERA}_${COMPRESSOR}_streamer)
set(CMAKE_CXX_STANDARD 23)

add_library(camera_${CAMERA} STATIC camera_${CAMERA}.cpp)

if("${CAMERA}" STREQUAL "ueye")
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/include/)
target_link_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/lib/)
target_link_libraries(camera_${CAMERA} ueye_api)
endif()

if("${CAMERA}" STREQUAL "realsense")
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/include/)
target_link_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/build/)
target_link_libraries(camera_${CAMERA} realsense)
endif()

if("${CAMERA}" STREQUAL "pylon")
find_package(pylon REQUIRED)
target_link_libraries(camera_${CAMERA} pylon::pylon)
endif()

if("${CAMERA}" STREQUAL "ffmpeg")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil)
    target_include_directories(camera_${CAMERA} PUBLIC ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(camera_${CAMERA} PUBLIC ${FFMPEG_LIBRARIES})
    target_compile_options(camera_${CAMERA} PUBLIC ${FFMPEG_CFLAGS_OTHER})
endif()

add_executable(${PROJECT_NAME} serve.cpp main.cpp util.cpp compress_${COMPRESSOR}.cpp placeholder_${COMPRESSOR}.cpp)
target_compile_definitions(${PROJECT_NAME} PUBLIC COMPRESSOR="${COMPRESSOR}")
target_link_libraries(${PROJECT_NAME} camera_${CAMERA} turbojpeg boost_thread pthread)
if("${COMPRESSOR}" STREQUAL "webp")
    # linking against turbojpeg is not optional since the webp code uses the JPEG enums for discriminating the pixel formats
    target_link_libraries(${PROJECT_NAME} webp)
endif()
