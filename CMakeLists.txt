# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
# set(BOARD cc1352p1_launchxl)
# set(BOARD rpi_pico)



find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cc1352_demo)

# zephyr_include_directories(
#     ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/driverlib
# )

zephyr_library_sources(
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2x7_cc26x2x7/driverlib/rfc.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2x7_cc26x2x7/rf_patches/rf_patch_rfe_genook.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2x7_cc26x2x7/rf_patches/rf_patch_mce_genook.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2x7_cc26x2x7/rf_patches/rf_patch_cpe_prop.c
    # ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2x7_cc26x2x7/
    # ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/drivers/power/PowerCC26X2.c
    # ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/drivers/power/PowerCC26X2_calibrateRCOSC.c
  )


target_include_directories(app PRIVATE include)

add_subdirectory(src)
