cmake_minimum_required(VERSION 3.7)

project(flairtool)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(redditcpp REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(RapidJSON REQUIRED MODULE)
find_package(SpdLog REQUIRED MODULE)

set(SOURCES
    main.cpp
    flairhandler.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC redditcpp RapidJSON::RapidJSON SpdLog::SpdLog)
