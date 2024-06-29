/*
 *  ======== ti_radio_config.h ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1352P1F3RGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.17.2
 *  SmartRF Studio data version : 2.29.0
 */
#ifndef _TI_RADIO_CONFIG_H_
#define _TI_RADIO_CONFIG_H_

#include <ti/devices/DeviceFamily.h>
#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_cmd.h>
#include <ti/drivers/rf/RF.h>


/* SmartRF Studio version that the RF data is fetched from */
#define SMARTRF_STUDIO_VERSION "2.29.0"

// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LAUNCHXL-CC1352P-2
#define LAUNCHXL_CC1352P_2

// High-Power Amplifier supported
#define SUPPORT_HIGH_PA

// RF frontend configuration
#define FRONTEND_SUB1G_DIFF_RF
#define FRONTEND_SUB1G_EXT_BIAS
#define FRONTEND_24G_DIFF_RF
#define FRONTEND_24G_EXT_BIAS

// Supported frequency bands
#define SUPPORT_FREQBAND_868
#define SUPPORT_FREQBAND_2400

// TX power table size definitions
#define TXPOWERTABLE_868_PA13_SIZE 22 // 868 MHz, 13 dBm
#define TXPOWERTABLE_2400_PA5_SIZE 16 // 2400 MHz, 5 dBm
#define TXPOWERTABLE_2400_PA5_20_SIZE 23 // 2400 MHz, 5 + 20 dBm

// TX power tables
extern RF_TxPowerTable_Entry txPowerTable_868_pa13[]; // 868 MHz, 13 dBm
extern RF_TxPowerTable_Entry txPowerTable_2400_pa5[]; // 2400 MHz, 5 dBm
extern RF_TxPowerTable_Entry txPowerTable_2400_pa5_20[]; // 2400 MHz, 5 + 20 dBm



//*********************************************************************************
//  RF Setting:   4.8 kbps, OOK, 39 kHz RX Bandwidth
//
//  PHY:          ook48kbps
//  Setting file: setting_tc597.json
//*********************************************************************************

// PA table usage
#define TX_POWER_TABLE_SIZE TXPOWERTABLE_868_PA13_SIZE

#define txPowerTable txPowerTable_868_pa13

// TI-RTOS RF Mode object
// extern RF_Mode RF_prop;

// RF Core API commands
// extern volatile rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t RF_cmdPropRadioDivSetup;
extern volatile rfc_CMD_FS_t RF_cmdFs;
extern volatile rfc_CMD_PROP_TX_t RF_cmdPropTx;
extern volatile rfc_CMD_PROP_RX_t RF_cmdPropRx;

// RF Core API overrides
// extern uint32_t pOverrides[];

#endif // _TI_RADIO_CONFIG_H_
