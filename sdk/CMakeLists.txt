# Minimum CMake required
cmake_minimum_required(VERSION 2.8)

# Project
project(sdk)

message("------------ Project -------------")
message("  Project name: " ${PROJECT_NAME})

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# src list
file(GLOB THIRD_PARTY_SRC ${PROJECT_SOURCE_DIR}/../third_party/crypto/md5/md5.cpp)
aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/main SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/network SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/logic SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/protos SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/config SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/pbconfig SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_SOURCE_DIR}/../third_party/x64-linux/include)
include_directories(/usr/local/include/mongocxx/v_noabi)
include_directories(/usr/local/include/bsoncxx/v_noabi)

message("  SRC_LIST: " ${SRC_LIST} ${THIRD_PARTY_SRC})

if (UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -Wall -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG")
    add_definitions(-DEVPP_HTTP_CLIENT_SUPPORTS_SSL)
endif()

message("------------ Cxx flags -------------")
message("  CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
message("------------------------------------")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/lib)

message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message(STATUS "CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})
message(STATUS "EXECUTABLE_OUTPUT_PATH " ${EXECUTABLE_OUTPUT_PATH})
message(STATUS "LIBRARY_OUTPUT_PATH " ${LIBRARY_OUTPUT_PATH})

link_directories(${LIBRARY_OUTPUT_PATH})
link_directories(${PROJECT_SOURCE_DIR}/../third_party/x64-linux/lib)

add_executable(sdk ${SRC_LIST} ${THIRD_PARTY_SRC})
target_link_libraries(sdk libgo dl xdb xshare xbase protobuf mongocxx bsoncxx evpp_https_static ssl crypto event_openssl event glog gflags)
