#include <stdio.h>
#include <zephyr/kernel.h>
// #include <ti/drivers/GPIO.h>
#include <ti/drivers/rf/RF.h>
#include <driverlib/rf_prop_mailbox.h>

/***** Includes *****/
/* Standard C Libraries */
#include <stdlib.h>
#include <unistd.h>

/* TI Drivers */
#include <inc/hw_ccfg.h>
#include <inc/hw_fcfg1.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>

#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_prop_mailbox.h>
#include <driverlib/rfc.h>
#include <rf_patches/rf_patch_cpe_multi_protocol.h>
#include <ti/drivers/rf/RF.h>

#include <rf_patches/rf_patch_cpe_prop.h>


/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

#include <zephyr/kernel.h>

/***** Defines *****/

/* Do power measurement */

/* Packet TX Configuration */
#define PAYLOAD_LENGTH      3
#define PACKET_INTERVAL     3000  /* Set packet interval to 500000us or 500ms */
// #endif

/***** Prototypes *****/

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

static uint8_t packet[PAYLOAD_LENGTH];
RF_Mode RF_prop=
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_prop,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0 
};

uint32_t pOverrides[] =
{
    // override_tc597.json
    // PHY: Use MCE RAM patch (mode 0), RFE RAM patch (mode 0)
    MCE_RFE_OVERRIDE(1,0,0,1,0,0),
    // Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    // Rx: Set AGC reference level to 0x19 (default: 0x2E)
    HW_REG_OVERRIDE(0x609C,0x0019),
    // Rx: Set RSSI offset to adjust reported RSSI by 3 dB (default: -2), trimmed for external bias and differential configuration
    (uint32_t)0x00FB88A3,
    // Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set IFAMPCTL3[7:4]=0xD)
    ADI_HALFREG_OVERRIDE(0,61,0xF,0xF),
    // TX: Set FSCA divider bias to 1
    HW32_ARRAY_OVERRIDE(0x405C,1),
    // TX: Set FSCA divider bias to 1
    (uint32_t)0x08141131,
    // OOK: Set duty cycle to compensate for PA ramping
    HW_REG_OVERRIDE(0x51E4,0x80AF),
    // Set code length k=7 in viterbi
    HW_REG_OVERRIDE(0x5270,0x0002),
    // override_prop_common_sub1g.json
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x4001405D,
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x08141131,
    // override_prop_common.json
    // DC/DC regulator: In Tx with 14 dBm PA setting, use DCDCCTL5[3:0]=0xF (DITHER_EN=1 and IPEAK=7). In Rx, use default settings.
    (uint32_t)0x00F788D3,
    // override_phy_tx_pa_ramp_genfsk_std.json
    // Tx: Configure PA ramping, set wait time before turning off (0x1A ticks of 16/24 us = 17.3 us).
    HW_REG_OVERRIDE(0x6028,0x001A),
    // Set TXRX pin to 0 in RX and high impedance in idle/TX. 
    HW_REG_OVERRIDE(0x60A8,0x0401),
    (uint32_t)0xFFFFFFFF
};

// Overrides for CMD_PROP_RADIO_DIV_SETUP_PA
uint32_t pOverridesTxStd[] =
{
    // override_txstd_placeholder.json
    // TX Standard power override
    TX_STD_POWER_OVERRIDE(0x013F),
    // The ANADIV radio parameter based on LO divider and front end settings
    (uint32_t)0x11310703,
    // override_phy_tx_pa_ramp_genfsk_std.json
    // Tx: Configure PA ramping, set wait time before turning off (0x1A ticks of 16/24 us = 17.3 us).
    HW_REG_OVERRIDE(0x6028,0x001A),
    // Set TXRX pin to 0 in RX and high impedance in idle/TX. 
    HW_REG_OVERRIDE(0x60A8,0x0401),
    (uint32_t)0xFFFFFFFF
};

// Overrides for CMD_PROP_RADIO_DIV_SETUP_PA
uint32_t pOverridesTx20[] =
{
    // override_tx20_placeholder.json
    // TX HighPA power override
    TX20_POWER_OVERRIDE(0x001B8ED2),
    // The ANADIV radio parameter based on LO divider and front end settings
    (uint32_t)0x11C10703,
    // override_phy_tx_pa_ramp_genfsk_hpa.json
    // Tx: Configure PA ramping, set wait time before turning off (0x1F ticks of 16/24 us = 20.3 us).
    HW_REG_OVERRIDE(0x6028,0x001F),
    // Set TXRX pin to 0 in RX/TX and high impedance in idle. 
    HW_REG_OVERRIDE(0x60A8,0x0001),
    (uint32_t)0xFFFFFFFF
};

// CMD_PROP_RADIO_DIV_SETUP_PA
// Proprietary Mode Radio Setup Command for All Frequency Bands
rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t RF_cmdPropRadioDivSetup =
{
    .commandNo = 0x3807,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .modulation.modType = 0x1,
    .modulation.deviation = 0x64,
    .modulation.deviationStepSz = 0x0,
    .symbolRate.preScale = 0xF,
    .symbolRate.rateWord = 0x8000,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x52,
    .preamConf.nPreamBytes = 0x4,
    .preamConf.preamMode = 0x0,
    .formatConf.nSwBits = 0x20,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x1,
    .formatConf.fecMode = 0x0,
    .formatConf.whitenMode = 0x0,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x0,
    .txPower = 0xFFFF,
    .pRegOverride = pOverrides,
    .centerFreq = 0x0364,
    .intFreq = 0x8000,
    .loDivider = 0x05,
    .pRegOverrideTxStd = pOverridesTxStd,
    .pRegOverrideTx20 = pOverridesTx20
};


rfc_CMD_PROP_TX_t RF_cmdPropTx =
{
    .commandNo = 0x3801,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bVarLen = 0x1,
    .pktLen = 0x14,
    .syncWord = 0x930B51DE,
    .pPkt = 0
};


// static uint16_t seqNumber;

/***** Function definitions *****/

void rfThread(void *p1, void* p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);


    RF_Params rfParams;
    RF_Params_init(&rfParams);
    printf("Params Init\n");

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

    /* Request access to the radio */
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
    printf("PropSetupDone\n");

    // /* Set the frequency */
    // RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
    // printf("CmdFs done\n");

    while(1)
    {
        /* Create packet with incrementing sequence number and random payload */
        
        printf("Sending Packet\n");
//         packet[0] = (uint8_t)68;
//         packet[1] = (uint8_t)101;
//         packet[2] = (uint8_t)118;
//
//         /* Send packet */
//         RF_EventMask terminationReason = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx,
//                                                    RF_PriorityNormal, NULL, 0);
//
//         switch(terminationReason)
//         {
//             case RF_EventLastCmdDone:
//                 // A stand-alone radio operation command or the last radio
//                 // operation command in a chain finished.
//                 break;
//             case RF_EventCmdCancelled:
//                 // Command cancelled before it was started; it can be caused
//             // by RF_cancelCmd() or RF_flushCmd().
//                 break;
//             case RF_EventCmdAborted:
//                 // Abrupt command termination caused by RF_cancelCmd() or
//                 // RF_flushCmd().
//                 break;
//             case RF_EventCmdStopped:
//                 // Graceful command termination caused by RF_cancelCmd() or
//                 // RF_flushCmd().
//                 break;
//             default:
//                 break;
//                 // Uncaught error event
//                 // while(1);
//         }
//
//         uint32_t cmdStatus = ((volatile RF_Op*)&RF_cmdPropTx)->status;
//         switch(cmdStatus)
//         {
//             case PROP_DONE_OK:
//                 // Packet transmitted successfully
//                 break;
//             case PROP_DONE_STOPPED:
//                 // received CMD_STOP while transmitting packet and finished
//                 // transmitting packet
//                 break;
//             case PROP_DONE_ABORT:
//                 // Received CMD_ABORT while transmitting packet
//                 break;
//             case PROP_ERROR_PAR:
//                 // Observed illegal parameter
//                 break;
//             case PROP_ERROR_NO_SETUP:
//                 // Command sent without setting up the radio in a supported
//                 // mode using CMD_PROP_RADIO_SETUP or CMD_RADIO_SETUP
//                 break;
//             case PROP_ERROR_NO_FS:
//                 // Command sent without the synthesizer being programmed
//                 break;
//             case PROP_ERROR_TXUNF:
//                 // TX underflow observed during operation
//                 break;
//             default:
//                 break;
//                 // Uncaught error event - these could come from the
//                 // pool of states defined in rf_mailbox.h
//                 // while(1);
//         }
//
// // #ifndef POWER_MEASUREMENT
// //         GPIO_toggle(CONFIG_GPIO_GLED);
// // #endif
//         /* Power down the radio */
//         RF_yield(rfHandle);
//
// #ifdef POWER_MEASUREMENT
//         /* Sleep for PACKET_INTERVAL s */
//         sleep(PACKET_INTERVAL);
// #else
        /* Sleep for PACKET_INTERVAL us */
        k_msleep(PACKET_INTERVAL);
        printf("Sent Packet");
// #endif

    }
}

// K_THREAD_DEFINE(rf_thread, 2048, rfThread, NULL, NULL, NULL, 2, 0, 0);

int main(void)
{

    // k_msleep(10000);
    rfThread(NULL, NULL, NULL);
    k_sleep(K_FOREVER);
    return 0;
}
