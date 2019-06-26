project(${CAMERA}_mjpeg_streamer)
cmake_minimum_required(VERSION 3.12) # TODO: find minimal version
set(CMAKE_CXX_STANDARD 14)

if(${CAMERA} STREQUAL "ueye") # TODO: create a "select one" option
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/lib/)
add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/include/)
target_link_libraries(camera_${CAMERA} ueye_api)
endif()

if(${CAMERA} STREQUAL "realsense")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/build/)
add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/include/)
target_link_libraries(camera_${CAMERA} realsense)
endif()

if(${CAMERA} STREQUAL "pylon")
link_directories(/opt/pylon5/lib64/)
add_library(camera_${CAMERA} SHARED camera_${CAMERA}.cpp)
target_include_directories(camera_${CAMERA} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/pylon5/include/)
target_link_libraries(camera_${CAMERA} pylonbase)
endif()

add_executable(${PROJECT_NAME} serve.cpp compress.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} camera_${CAMERA} turbojpeg boost_thread boost_system pthread dl)
