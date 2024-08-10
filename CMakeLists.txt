#
# MIT License
#
# Copyright (c) 2021 Olivier Le Doeuff
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# ── CMAKE PROPERTIES ──

cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(NETTCPJSON_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(NETTCPJSON_MAIN_PROJECT ON)
endif()

# ── CMAKE PROPERTIES ──

set(NETTCPJSON_PROJECT "NetTcpJson" CACHE STRING "Project Name")
set(NETTCPJSON_BUILD_SHARED OFF CACHE BOOL "Build NetTcpJson as shared library")
set(NETTCPJSON_ENABLE_TESTS OFF CACHE BOOL "Enable NetTcpJson unit tests")

set(NETTCPJSON_VERSION_MAJOR 1)
set(NETTCPJSON_VERSION_MINOR 0)
set(NETTCPJSON_VERSION_PATCH 2)
set(NETTCPJSON_VERSION ${NETTCPJSON_VERSION_MAJOR}.${NETTCPJSON_VERSION_MINOR}.${NETTCPJSON_VERSION_PATCH} CACHE STRING "NetTcpJson Version")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE NETTCPJSON_VERSION_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(NETTCPJSON_VERSION_TAG 00000000)
endif()
set(NETTCPJSON_VERSION_TAG_HEX 0x${NETTCPJSON_VERSION_TAG})

# ── PROJECT PROPERTIES ──

project(${NETTCPJSON_PROJECT} VERSION ${NETTCPJSON_VERSION} LANGUAGES CXX)
if(NETTCPJSON_MAIN_PROJECT)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# ── STATUS ──

message(STATUS  "------ ${NETTCPJSON_TARGET} Configuration ${NETTCPJSON_VERSION} ------")

message(STATUS "NETTCPJSON_VERSION           : ${NETTCPJSON_VERSION}")
message(STATUS "NETTCPJSON_BUILD_SHARED      : ${NETTCPJSON_BUILD_SHARED}")
message(STATUS "NETTCPJSON_ENABLE_TESTS      : ${NETTCPJSON_ENABLE_TESTS}")

message(STATUS "------ ${NETTCPJSON_TARGET} End Configuration ------")

# ── DEPENDENCIES ──

find_package(Qt5 REQUIRED COMPONENTS Core)
include(cmake/Dependencies.cmake)
include(cmake/FetchNetTcp.cmake)

# ── NETTCPJSON ──

set(NETTCPJSON_TARGET NetTcpJson)

set(NETTCPJSON_SRCS
  src/NetTcpJson/Extractor.hpp
  src/NetTcpJson/Extractor.cpp
  src/NetTcpJson/SocketWorker.hpp
  src/NetTcpJson/SocketWorker.cpp
  src/NetTcpJson/Socket.hpp
  src/NetTcpJson/Socket.cpp
  src/NetTcpJson/Server.hpp
  src/NetTcpJson/Server.cpp
 )

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${NETTCPJSON_SRCS})

if(NETTCPJSON_BUILD_SHARED)
  add_library(${NETTCPJSON_TARGET} SHARED ${NETTCPJSON_SRCS})
  target_compile_definitions(${NETTCPJSON_TARGET} PRIVATE -DNETTCPJSON_SHARED)
else()
  add_library(${NETTCPJSON_TARGET} STATIC ${NETTCPJSON_SRCS})
  target_compile_definitions(${NETTCPJSON_TARGET} PUBLIC -DNETTCPJSON_STATIC)
endif()

target_compile_features(${NETTCPJSON_TARGET} PUBLIC cxx_std_17)
target_include_directories(${NETTCPJSON_TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> $<INSTALL_INTERFACE:include>)
target_link_libraries(${NETTCPJSON_TARGET} PUBLIC
  Qt::Core
  NetTcp
)
add_library(NetTcpJson::NetTcpJson ALIAS ${NETTCPJSON_TARGET})
set_target_properties(${NETTCPJSON_TARGET} PROPERTIES AUTOMOC TRUE)

# ── NETTCPJSON TESTS ──

if(NETTCPJSON_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ── LOG CONFIGURATION ──

if(NETTCPJSON_MAIN_PROJECT)
  include(cmake/PrintConfiguration.cmake)
endif()
