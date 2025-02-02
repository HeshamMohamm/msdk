/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   ADC demo application
 * @details Continuously monitors the ADC channels
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <mxc.h>
#include "example_config.h"

/***** Globals *****/
#ifdef USE_INTERRUPTS
volatile unsigned int adc_done = 0;
#endif

static uint16_t adc_val;

/***** Functions *****/

#ifdef USE_INTERRUPTS
void adc_complete_cb(void *req, int error)
{
    adc_done = 1;
    return;
}
void ADC_IRQHandler(void)
{
    MXC_ADC_Handler();
}
#endif

int main(void)
{
    printf("\n******************** ADC Example ********************\n");
    printf("\nADC readings are taken on ADC channel %d every 250ms\n", ADC_CHANNEL);
    printf("and are subsequently printed to the terminal.\n\n");

    /* Initialize ADC */
    if (MXC_ADC_Init() != E_NO_ERROR) {
        printf("Error Bad Parameter\n");
        while (1) {}
    }

    /* Set up LIMIT0 to monitor high and low trip points */
    while (MXC_ADC->status & (MXC_F_ADC_STATUS_ACTIVE | MXC_F_ADC_STATUS_AFE_PWR_UP_ACTIVE)) {}
    MXC_ADC_SetMonitorChannel(MXC_ADC_MONITOR_3, ADC_CHANNEL);
    MXC_ADC_SetMonitorHighThreshold(MXC_ADC_MONITOR_3, 0x300);
    MXC_ADC_SetMonitorLowThreshold(MXC_ADC_MONITOR_3, 0x25);
    MXC_ADC_EnableMonitor(MXC_ADC_MONITOR_3);

#ifdef USE_INTERRUPTS
    NVIC_EnableIRQ(ADC_IRQn);
#endif

    while (1) {
        /* Flash LED when starting ADC cycle */
        LED_On(LED_RED);
        MXC_TMR_Delay(MXC_TMR0, MSEC(10));
        LED_Off(LED_RED);

        /* Convert channel 0 */
#ifdef USE_INTERRUPTS
        adc_done = 0;
        MXC_ADC_StartConversionAsync(ADC_CHANNEL, adc_complete_cb);

        while (!adc_done) {}
#else
        MXC_ADC_StartConversion(ADC_CHANNEL);
#endif
        static uint8_t overflow;

        overflow = (MXC_ADC_GetData(&adc_val) == E_OVERFLOW ? 1 : 0);

        /* Display results on OLED display, display asterisk if overflow */
        printf("%d: 0x%04x%s\n\n", ADC_CHANNEL, adc_val, overflow ? "*" : " ");

        /* Determine if programmable limits on AIN0 were exceeded */
        if (MXC_ADC_GetFlags() & (MXC_F_ADC_INTR_LO_LIMIT_IF | MXC_F_ADC_INTR_HI_LIMIT_IF)) {
            printf(" %s Limit on AIN0 ",
                   (MXC_ADC_GetFlags() & MXC_F_ADC_INTR_LO_LIMIT_IF) ? "Low" : "High");
            MXC_ADC_ClearFlags(MXC_F_ADC_INTR_LO_LIMIT_IF | MXC_F_ADC_INTR_HI_LIMIT_IF);
        } else {
            printf("                   ");
        }

        printf("\n");

        /* Delay for 1/4 second before next reading */
        MXC_TMR_Delay(MXC_TMR0, MSEC(250));
    }
}
