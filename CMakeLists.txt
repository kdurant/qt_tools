cmake_minimum_required(VERSION 3.5)

project(debug_tool LANGUAGES CXX)

add_definitions( -DSOFT_VERSION="v0.01" )

execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_HASH=\"${GIT_HASH}\")

execute_process(
  COMMAND git log --pretty=format:%cs -1
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_DATE=\"${GIT_DATE}\")



set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5SerialPort REQUIRED)

include_directories(./src)

aux_source_directory(./src DIR_SRCS)

add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/mainwindow.h
    ./src/mainwindow.ui
    ./res/res.qrc
    ./src/common.h
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::SerialPort)

