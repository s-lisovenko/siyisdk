cmake_minimum_required(VERSION 3.21)

project(siyi_cli LANGUAGES CXX)

# C++ options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cmake qt resource options
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

# Status messages
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using Qt${QT_VERSION_MAJOR}")

#######################################################
#                   Target
#######################################################

# Target
add_executable(${PROJECT_NAME} SiyiCli.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Core siyisdk)
