# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(BOARD cc1352p1_launchxl)
# set(BOARD rpi_pico)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cc1352_demo)

target_include_directories(app PRIVATE include)

add_subdirectory(src)
