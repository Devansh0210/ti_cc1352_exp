/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/rf/RF.h>
#include <driverlib/rf_prop_mailbox.h>


int main(void)
{
    while(1) {
        k_msleep(1000);
        printf("Hello Devansh! %s\n", CONFIG_BOARD);
    }
    return 0;
}
