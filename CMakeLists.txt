project(ueye_mjpeg_streamer)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
aux_source_directory(. SRC_LIST)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/lib/)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/ueye/usr/include/)
target_link_libraries(${PROJECT_NAME} ueye_api turbojpeg boost_thread boost_system pthread)
