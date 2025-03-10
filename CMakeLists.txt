cmake_minimum_required(VERSION 3.21)

#######################################################
#                   Project Version
#######################################################

##
## git functions
##
include(${CMAKE_SOURCE_DIR}/cmake/git.cmake)

# get git version
git_get_version(VERSION_MAJOR VERSION_MINOR PROJECT_VERSION_PATCH PROJECT_VERSION_SUFFIX)

#######################################################
#                   Project Info
#######################################################
project(siyisdk VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${PROJECT_VERSION_PATCH} LANGUAGES CXX)
set(PROJECT_VERSION_FULL "${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}")

message(STATUS ${PROJECT_NAME} " v" ${PROJECT_VERSION_FULL})

add_definitions(-DVERSION="${PROJECT_VERSION}")

#######################################################
#                   Project options
#######################################################

#######################################################
#                   QT, CMake and C++ options
#######################################################

# C++ options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
add_library(${PROJECT_NAME} STATIC
    include/Siyi.h
    include/CameraApi.h
    include/Message.h
    include/MessageBuilder.h
    src/Crc.h
    src/Crc.cpp
    src/CameraApi.cpp
    src/MessageParser.h
    src/MessageParser.cpp
    src/CommunicationWorker.h
    src/CommunicationWorker.cpp
    src/MessageBuilder.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Core)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Examples
add_subdirectory(example)
