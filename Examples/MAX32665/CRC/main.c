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
 * @file    	main.c
 * @brief   	Example showing how to use the CRC module. Covers 16 and 32-bit CRC.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_errors.h"
#include "board.h"
#include "tpu.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/
int crc_sw(uint8_t *data, uint8_t len, uint32_t polynomial)
{
    int i, j;
    uint32_t temp;
    uint32_t crcval = 0;

    for (i = 0; i < len; ++i) {
        temp = (data[i] & 0xFF);
        for (j = 0; j < 8; ++j) {
            if ((temp ^ crcval) & 1) {
                crcval >>= 1;
                crcval ^= polynomial;
            } else {
                crcval >>= 1;
            }
            temp >>= 1;
        }
    }

    return crcval;
}

// *****************************************************************************
int main(void)
{
    printf("\n***** CRC Example *****\n");

    uint8_t data[] = { 0x14, 0x78, 0x9C, 0xDE };
    uint8_t len = sizeof(data) / sizeof(uint8_t);
    int fail = 0;

    if (MXC_TPU_CRC_Config() != E_SUCCESS) {
        printf("Failed MXC_TPU_Crc_Config()\n");
        return E_FAIL;
    }

    printf("CRC16:\n");

    //Calculate correct result
    uint32_t sw_crc = crc_sw(data, len, MXC_TPU_CRC16);
    uint32_t hw_crc;

    //Generate hardware result
    if (MXC_TPU_CRC(data, len, MXC_TPU_CRC16, &hw_crc) != E_SUCCESS) {
        printf("Failed MXC_TPU_CRC()\n");
        return E_FAIL;
    }

    fail += memcmp(&hw_crc, &sw_crc, sizeof(hw_crc));
    printf("Calculated CRC = 0x%08x\n", hw_crc);
    printf("Expected CRC   = 0x%08x\n", sw_crc);
    printf("\n");

    //Call TPU_Crc_Config() again to reset
    if (MXC_TPU_CRC_Config() != E_SUCCESS) {
        printf("Failed MXC_TPU_Crc_Config()\n");
        return E_FAIL;
    }

    printf("CRC32:\n");

    //Calculate correct result
    sw_crc = crc_sw(data, len, MXC_TPU_CRC32_ETHERNET);

    //Generate hardware result
    if (MXC_TPU_CRC(data, len, MXC_TPU_CRC32_ETHERNET, &hw_crc) != E_SUCCESS) {
        printf("Failed MXC_TPU_CRC()\n");
        return E_FAIL;
    }

    fail += memcmp(&hw_crc, &sw_crc, sizeof(hw_crc));
    printf("Calculated CRC = 0x%08x\n", hw_crc);
    printf("Expected CRC   = 0x%08x\n", sw_crc);

    if (fail != 0) {
        printf("\nExample Failed\n");
        return E_FAIL;
    }

    printf("\nExample Succeeded\n");
    return E_NO_ERROR;
}
