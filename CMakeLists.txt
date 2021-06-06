cmake_minimum_required(VERSION 3.15)

project(act1 CXX)

find_package(Threads)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_library(libact1 SHARED "")
target_sources(libact1 PRIVATE "src/act1.cc")
target_link_libraries(libact1 PUBLIC Threads::Threads)

add_executable(main "")
target_sources(main PRIVATE "src/main.cc")
target_link_libraries(main PRIVATE libact1)