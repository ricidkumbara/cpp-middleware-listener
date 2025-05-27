cmake_minimum_required(VERSION 3.10)
project(MiddlewareListener)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add header path
include_directories(include)

# Create the library from source
add_library(file_watcher STATIC src/file_watcher.cpp)
add_library(data_parser STATIC src/data_parser.cpp)
add_library(udp_sender STATIC src/udp_sender.cpp)

# Create the main executable and link it with the library
add_executable(app main.cpp)
target_link_libraries(app file_watcher)
target_link_libraries(app data_parser)
target_link_libraries(app udp_sender)
if(WIN32)
    target_link_libraries(udp_sender PRIVATE Ws2_32)
endif()

