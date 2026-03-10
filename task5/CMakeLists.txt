cmake_minimum_required(VERSION 3.10)
project(MyFTXUI)

set(CMAKE_CXX_STANDARD 20)

find_package(ftxui REQUIRED)

add_executable(myftxui main.cpp)

target_link_libraries(myftxui PRIVATE ftxui::screen ftxui::dom ftxui::component)