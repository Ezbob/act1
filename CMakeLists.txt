cmake_minimum_required(VERSION 3.15)

project(act1 CXX)

add_subdirectory(readerwriterqueue EXCLUDE_FROM_ALL)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(main "")
target_sources(main PRIVATE "src/main.cc")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_LIST_DIR}/readerwriterqueue")