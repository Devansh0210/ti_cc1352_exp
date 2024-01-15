# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(BOARD cc1352p1_launchxl)
# set(BOARD rpi_pico)



find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cc1352_demo)

# zephyr_include_directories(
#     ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/driverlib
# )

zephyr_library_sources(
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/rf_patches/rf_patch_rfe_genook.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/rf_patches/rf_patch_mce_genook.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/rf_patches/rf_patch_cpe_prop.c
    ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/devices/cc13x2_cc26x2/driverlib/rfc.c
    # ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/drivers/power/PowerCC26X2.c
    # ${ZEPHYR_TI_MODULE_DIR}/simplelink/source/ti/drivers/power/PowerCC26X2_calibrateRCOSC.c
  )
# zephyr_library_source(
#     rf_patches/rf_patch_rfe_genook.c
#     rf_patches/rf_patch_mce_genook.c
#     rf_patches/rf_patch_cpe_prop.c
#     # Required for SystemTrimDevice which is not in ROM
#     driverlib/setup.c
#     driverlib/chipinfo.c
#     driverlib/aux_sysif.c
#     # Required for CPUdelay which is not in ROM
#     driverlib/cpu.c
#     # Required functions to support the TI Power module
#     driverlib/sys_ctrl.c
#     driverlib/osc.c
#     driverlib/driverlib_release.c
#     # Required for newer HAL TI >= 4.40 (for PRCMPowerDomainsAllOn)
#     driverlib/prcm.c
#


target_include_directories(app PRIVATE include)

add_subdirectory(src)
