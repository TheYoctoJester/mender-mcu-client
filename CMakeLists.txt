# @file      CMakeLists.txt
# @brief     mender-mcu-client static library CMakeLists file
#
# MIT License
#
# Copyright (c) 2022-2023 joelguittet and mender-mcu-client contributors
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

cmake_minimum_required(VERSION 3.16.3)

# List of sources
file(GLOB SOURCES_TEMP
    "${CMAKE_CURRENT_LIST_DIR}/core/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/board/${CONFIG_MENDER_MCU_CLIENT_BOARD_TYPE}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/http/${CONFIG_MENDER_MCU_CLIENT_HTTP_TYPE}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/rtos/${CONFIG_MENDER_MCU_CLIENT_RTOS_TYPE}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/tls/${CONFIG_MENDER_MCU_CLIENT_TLS_TYPE}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/add-ons/src/*.c"
)

# Add include directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# Creation of the library
add_library(mender-mcu-client STATIC ${SOURCES_TEMP})
