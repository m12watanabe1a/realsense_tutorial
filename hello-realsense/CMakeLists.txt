# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(RealsenseExamplesHelloRealSense)

add_executable(rs-hello-realsense rs-hello-realsense.cpp)
target_link_libraries(rs-hello-realsense realsense2)