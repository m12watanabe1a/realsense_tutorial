# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(RealsenseExamplesAlignment)

find_package(PkgConfig)
pkg_check_modules(RealSense2 REQUIRED realsense2)
pkg_check_modules(OpenCV REQUIRED opencv)
include_directories(${RealSense2_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
link_directories(${RealSense2_LIBRARY_DIRS} ${OpenCV_LIBRARY_DIRS})

add_executable(rs-align rs-align.cpp)
target_link_libraries(rs-align ${RealSense2_LIBRARIES} ${OpenCV_LIBRARIES})
