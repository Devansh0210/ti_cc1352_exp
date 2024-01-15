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
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
// #include <ti/drivers/GPIO.h>
//#include <ti/drivers/pin/PINCC26XX.h>

/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

/* Board Header files */
#include "ti_driver_config.h"
#include <ti_radio_config.h>
// #include <ti/devices/cc13x2_cc26x2/rf_patches/rf_patch_mce_genook.h>
#include "zephyr/sys_clock.h"
#include <zephyr/kernel.h>

/***** Defines *****/

/* Do power measurement */
//#define POWER_MEASUREMENT

/* Packet TX Configuration */
#define PAYLOAD_LENGTH      3
// #ifdef POWER_MEASUREMENT
// #define PACKET_INTERVAL     5  /* For power measurement set packet interval to 5s */
// #else
#define PACKET_INTERVAL     3000  /* Set packet interval to 500000us or 500ms */
// #endif

/***** Prototypes *****/

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;

/***** Function definitions *****/

void rfThread(void *p1, void* p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);


    RF_Params rfParams;
    RF_Params_init(&rfParams);

    // GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    //
    // GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

    /* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif// DeviceFamily_CC26X0R2

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    while(1)
    {
        /* Create packet with incrementing sequence number and random payload */
        packet[0] = (uint8_t)68;
        packet[1] = (uint8_t)101;
        packet[2] = (uint8_t)118;

        /* Send packet */
        RF_EventMask terminationReason = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx,
                                                   RF_PriorityNormal, NULL, 0);

        switch(terminationReason)
        {
            case RF_EventLastCmdDone:
                // A stand-alone radio operation command or the last radio
                // operation command in a chain finished.
                break;
            case RF_EventCmdCancelled:
                // Command cancelled before it was started; it can be caused
            // by RF_cancelCmd() or RF_flushCmd().
                break;
            case RF_EventCmdAborted:
                // Abrupt command termination caused by RF_cancelCmd() or
                // RF_flushCmd().
                break;
            case RF_EventCmdStopped:
                // Graceful command termination caused by RF_cancelCmd() or
                // RF_flushCmd().
                break;
            default:
                break;
                // Uncaught error event
                // while(1);
        }

        uint32_t cmdStatus = ((volatile RF_Op*)&RF_cmdPropTx)->status;
        switch(cmdStatus)
        {
            case PROP_DONE_OK:
                // Packet transmitted successfully
                break;
            case PROP_DONE_STOPPED:
                // received CMD_STOP while transmitting packet and finished
                // transmitting packet
                break;
            case PROP_DONE_ABORT:
                // Received CMD_ABORT while transmitting packet
                break;
            case PROP_ERROR_PAR:
                // Observed illegal parameter
                break;
            case PROP_ERROR_NO_SETUP:
                // Command sent without setting up the radio in a supported
                // mode using CMD_PROP_RADIO_SETUP or CMD_RADIO_SETUP
                break;
            case PROP_ERROR_NO_FS:
                // Command sent without the synthesizer being programmed
                break;
            case PROP_ERROR_TXUNF:
                // TX underflow observed during operation
                break;
            default:
                break;
                // Uncaught error event - these could come from the
                // pool of states defined in rf_mailbox.h
                // while(1);
        }

// #ifndef POWER_MEASUREMENT
//         GPIO_toggle(CONFIG_GPIO_GLED);
// #endif
        /* Power down the radio */
        RF_yield(rfHandle);

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

void hello_world_entry(void* p1, void* p2, void* p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while(1) {
        k_msleep(2000);
        printf("Hello Devansh");
    };
}
//     
//
// K_THREAD_DEFINE(hello_world, 1024, hello_world_entry, NULL, NULL, NULL, 6, 0, 0);

K_THREAD_DEFINE(rf_thread, 2048, rfThread, NULL, NULL, NULL, 2, 0, 0);

int main(void)
{
    // while(1) {
    //     k_msleep(1000);
    //     printf("Hello TI! %s\n", CONFIG_BOARD);
    // }


    k_sleep(K_FOREVER);
    return 0;
}
