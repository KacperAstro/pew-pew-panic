cmake_minimum_required(VERSION 3.10)

project(infa)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(infa
    main.cpp
)

target_link_libraries(infa sfml-graphics sfml-window sfml-system)

