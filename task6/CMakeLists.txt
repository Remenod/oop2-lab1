cmake_minimum_required(VERSION 3.10)
project(judgment)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ftxui REQUIRED)

add_executable(judgment
    main.cpp
    src/ReferenceManager.cpp
)

target_include_directories(judgment PRIVATE
    include
)

target_link_libraries(judgment PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_compile_definitions(judgment PRIVATE 
    SNIPPETS_DIR="${CMAKE_SOURCE_DIR}/snippets"
)
