project(${CAMERA}_mjpeg_streamer)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)

set(CAMERA "UNDEFINED" CACHE STRING "Camera Backend")
set_property(CACHE CAMERA PROPERTY STRINGS ueye realsense pylon)

if("${CAMERA}" STREQUAL "ueye")
link_directories(camera_${CAMERA} ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/lib/)
add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)
target_include_directories(camera_${CAMERA} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/include/)
target_link_libraries(camera_${CAMERA} ueye_api)
endif()

if("${CAMERA}" STREQUAL "realsense")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/build/)
add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)
target_include_directories(camera_${CAMERA} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/include/)
target_link_libraries(camera_${CAMERA} realsense)
endif()

if("${CAMERA}" STREQUAL "pylon")
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/pylon/include/)
target_link_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/pylon/lib/)
target_link_libraries(camera_${CAMERA} pylonbase GenApi_gcc_v3_1_Basler_pylon GCBase_gcc_v3_1_Basler_pylon)
endif()

add_executable(${PROJECT_NAME} serve.cpp compress.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} camera_${CAMERA} turbojpeg boost_thread boost_system pthread dl)
