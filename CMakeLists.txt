cmake_minimum_required(VERSION 3.13)

set(CAMERA "UNDEFINED" CACHE STRING "Camera Backend")
set_property(CACHE CAMERA PROPERTY STRINGS ueye realsense pylon)

project(${CAMERA}_mjpeg_streamer)
set(CMAKE_CXX_STANDARD 14)

add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)

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
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/pylon/include/)
target_link_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/pylon/lib/)
target_link_libraries(camera_${CAMERA} pylonbase GenApi_gcc_v3_1_Basler_pylon GCBase_gcc_v3_1_Basler_pylon)
endif()

add_executable(${PROJECT_NAME} serve.cpp compress.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} camera_${CAMERA} turbojpeg boost_thread boost_system pthread dl)
