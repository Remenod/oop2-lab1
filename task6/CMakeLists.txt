cmake_minimum_required(VERSION 3.10)
project(judgment)

set(CMAKE_CXX_STANDARD 20)

find_package(ftxui REQUIRED)

add_executable(judgment main.cpp)

target_link_libraries(judgment PRIVATE ftxui::screen ftxui::dom ftxui::component)