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

// aisegment_unet
// Created using ai8xize.py --test-dir sdk/Examples/MAX78000/CNN --prefix aisegment_unet --checkpoint-file trained/ai85-aisegment-unet-large-fakept-q.pth.tar --config-file networks/aisegment-unet-large-fakept.yaml --device MAX78000 --timer 0 --display-checkpoint --verbose --overlap-data --mlator --no-unload --max-checklines 8192 --new-kernel-loader

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "cnn.h"
#include "sampledata.h"
#include "sampleoutput.h"

volatile uint32_t cnn_time; // Stopwatch

void fail(void)
{
    printf("\n*** FAIL ***\n\n");
    while (1) {}
}

// 48-channel 48x48 data input (110592 bytes total / 2304 bytes per channel):
// HWC 48x48, channels 0 to 3
static const uint32_t input_0[] = SAMPLE_INPUT_0;

// HWC 48x48, channels 4 to 7
static const uint32_t input_4[] = SAMPLE_INPUT_4;

// HWC 48x48, channels 8 to 11
static const uint32_t input_8[] = SAMPLE_INPUT_8;

// HWC 48x48, channels 12 to 15
static const uint32_t input_12[] = SAMPLE_INPUT_12;

// HWC 48x48, channels 16 to 19
static const uint32_t input_16[] = SAMPLE_INPUT_16;

// HWC 48x48, channels 20 to 23
static const uint32_t input_20[] = SAMPLE_INPUT_20;

// HWC 48x48, channels 24 to 27
static const uint32_t input_24[] = SAMPLE_INPUT_24;

// HWC 48x48, channels 28 to 31
static const uint32_t input_28[] = SAMPLE_INPUT_28;

// HWC 48x48, channels 32 to 35
static const uint32_t input_32[] = SAMPLE_INPUT_32;

// HWC 48x48, channels 36 to 39
static const uint32_t input_36[] = SAMPLE_INPUT_36;

// HWC 48x48, channels 40 to 43
static const uint32_t input_40[] = SAMPLE_INPUT_40;

// HWC 48x48, channels 44 to 47
static const uint32_t input_44[] = SAMPLE_INPUT_44;

void load_input(void)
{
    // This function loads the sample data input -- replace with actual data

    memcpy32((uint32_t *)0x50400700, input_0, 2304);
    memcpy32((uint32_t *)0x50408700, input_4, 2304);
    memcpy32((uint32_t *)0x50410700, input_8, 2304);
    memcpy32((uint32_t *)0x50418700, input_12, 2304);
    memcpy32((uint32_t *)0x50800700, input_16, 2304);
    memcpy32((uint32_t *)0x50808700, input_20, 2304);
    memcpy32((uint32_t *)0x50810700, input_24, 2304);
    memcpy32((uint32_t *)0x50818700, input_28, 2304);
    memcpy32((uint32_t *)0x50c00700, input_32, 2304);
    memcpy32((uint32_t *)0x50c08700, input_36, 2304);
    memcpy32((uint32_t *)0x50c10700, input_40, 2304);
    memcpy32((uint32_t *)0x50c18700, input_44, 2304);
}

// Truncated further checks...
// Expected output of layer 18 for aisegment_unet given the sample input (known-answer test)
// Delete this function for production code
static const uint32_t sample_output[] = SAMPLE_OUTPUT;
int check_output(void)
{
    int i;
    uint32_t mask, len;
    volatile uint32_t *addr;
    const uint32_t *ptr = sample_output;

    while ((addr = (volatile uint32_t *)*ptr++) != 0) {
        mask = *ptr++;
        len = *ptr++;
        for (i = 0; i < len; i++)
            if ((*addr++ & mask) != *ptr++) {
                printf("Data mismatch (%d/%d) at address 0x%08x: Expected 0x%08x, read 0x%08x.\n",
                       i + 1, len, addr - 1, *(ptr - 1), *(addr - 1) & mask);
                return CNN_FAIL;
            }
    }

    return CNN_OK;
}

int main(void)
{
    MXC_ICC_Enable(MXC_ICC0); // Enable cache

    // Switch to 100 MHz clock
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    printf("Waiting...\n");

    // DO NOT DELETE THIS LINE:
    MXC_Delay(SEC(2)); // Let debugger interrupt if needed

    // Enable peripheral, enable CNN interrupt, turn on CNN clock
    // CNN clock: APB (50 MHz) div 1
    cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

    printf("\n*** CNN Inference Test ***\n");

    cnn_init(); // Bring state machine into consistent state
    cnn_load_weights(); // Load kernels
    cnn_load_bias();
    cnn_configure(); // Configure state machine
    load_input(); // Load data input
    cnn_start(); // Start CNN processing

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; // SLEEPDEEP=0
    while (cnn_time == 0) __WFI(); // Wait for CNN

    if (check_output() != CNN_OK)
        fail();

    printf("\n*** PASS ***\n\n");

#ifdef CNN_INFERENCE_TIMER
    printf("Approximate inference time: %u us\n\n", cnn_time);
#endif

    cnn_disable(); // Shut down CNN clock, disable peripheral

    return 0;
}

/*
  SUMMARY OF OPS
  Hardware: 183,069,504 ops (182,034,432 macc; 1,035,072 comp; 0 add; 0 mul; 0 bitwise)
    Layer 0: 7,225,344 ops (7,077,888 macc; 147,456 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 9,584,640 ops (9,437,184 macc; 147,456 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 4,792,320 ops (4,718,592 macc; 73,728 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 5,326,848 ops (5,308,416 macc; 18,432 comp; 0 add; 0 mul; 0 bitwise)
    Layer 4: 1,195,776 ops (1,161,216 macc; 34,560 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 2,056,320 ops (2,032,128 macc; 24,192 comp; 0 add; 0 mul; 0 bitwise)
    Layer 6: 2,044,224 ops (2,032,128 macc; 12,096 comp; 0 add; 0 mul; 0 bitwise)
    Layer 7: 451,584 ops (451,584 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 8: 8,128,512 ops (8,128,512 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 9: 8,136,576 ops (8,128,512 macc; 8,064 comp; 0 add; 0 mul; 0 bitwise)
    Layer 10: 8,128,512 ops (8,128,512 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 11: 8,144,640 ops (8,128,512 macc; 16,128 comp; 0 add; 0 mul; 0 bitwise)
    Layer 12: 4,644,864 ops (4,644,864 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 13: 16,035,840 ops (15,925,248 macc; 110,592 comp; 0 add; 0 mul; 0 bitwise)
    Layer 14: 63,848,448 ops (63,700,992 macc; 147,456 comp; 0 add; 0 mul; 0 bitwise)
    Layer 15: 9,584,640 ops (9,437,184 macc; 147,456 comp; 0 add; 0 mul; 0 bitwise)
    Layer 16: 9,584,640 ops (9,437,184 macc; 147,456 comp; 0 add; 0 mul; 0 bitwise)
    Layer 17: 9,437,184 ops (9,437,184 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 18: 4,718,592 ops (4,718,592 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 279,264 bytes out of 442,368 bytes total (63%)
  Bias memory:   876 bytes out of 2,048 bytes total (43%)
*/
