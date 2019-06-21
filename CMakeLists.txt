project(${CAMERA}_mjpeg_streamer)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
#aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/lib/)
add_library(camera_ueye SHARED camera_ueye.cpp)
target_include_directories(camera_ueye PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/include/)
target_link_libraries(camera_ueye ueye_api)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/build/)
add_library(camera_realsense SHARED camera_realsense.cpp)
target_include_directories(camera_realsense PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/librealsense/include/)
target_link_libraries(camera_realsense realsense)

add_executable(${PROJECT_NAME} serve.cpp compress.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} camera_${CAMERA} turbojpeg boost_thread boost_system pthread dl)
