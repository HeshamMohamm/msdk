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
 * @brief   UART!
 * @details This example demonstrates the UART Loopback Test.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "dma.h"
#include "nvic_table.h"

/***** Definitions *****/
//#define DMA //Un-comment to perform a DMA read

#define READING_UART MXC_UART1
#define READING_UART_IDX MXC_UART_GET_IDX(READING_UART)
#define WRITING_UART MXC_UART2

#define UART_BAUD 115200
#define BUFF_SIZE 1024

/***** Globals *****/
volatile int READ_FLAG;

/***** Functions *****/
#ifndef DMA
void Reading_UART_Handler(void)
{
    MXC_UART_AsyncHandler(READING_UART);
}
#endif // DMA

void readCallback(mxc_uart_req_t *req, int error)
{
    READ_FLAG = error;
}
/******************************************************************************/
int main(void)
{
    int error, i, fail = 0;
    uint8_t TxData[BUFF_SIZE];
    uint8_t RxData[BUFF_SIZE];

    printf("\n\n********************* UART Example ***********************\n");
    printf("This example sends data from UART2 to UART1 to another.\n");
    printf("\nIf the transaction succeeds, the green LED will illuminate.\n");
    printf("If the transaction fails, the red LED will illuminate.\n");
    printf("\nConnect UART1 RX (P0.28) to UART2 TX (P0.15, AIN7).\n");

    printf("\n-->UART Baud \t: %d Hz\n", UART_BAUD);
    printf("-->Test Length \t: %d bytes\n\n", BUFF_SIZE);

    // Initialize the data buffers
    for (i = 0; i < BUFF_SIZE; i++) {
        TxData[i] = i;
    }
    memset(RxData, 0x0, BUFF_SIZE);

#ifndef DMA
    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(READING_UART_IDX));
    NVIC_DisableIRQ(MXC_UART_GET_IRQ(READING_UART_IDX));
    MXC_NVIC_SetVector(MXC_UART_GET_IRQ(READING_UART_IDX), Reading_UART_Handler);
    NVIC_EnableIRQ(MXC_UART_GET_IRQ(READING_UART_IDX));
#endif //

    // Initialize the UART
    error = MXC_UART_Init(READING_UART, UART_BAUD, MXC_UART_APB_CLK);
    if (error < E_NO_ERROR) {
        printf("-->Error initializing UART: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }
    printf("-->Reading UART Initialized\n");

    // Initialize writing UART
    error = MXC_UART_Init(WRITING_UART, UART_BAUD, MXC_UART_APB_CLK);
    if (error < E_NO_ERROR) {
        printf("-->Error initializing UART: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }
    printf("-->Writing UART Initialized\n\n");

#ifdef DMA
    // Automatically set up DMA handlers/ISRs
    MXC_UART_SetAutoDMAHandlers(READING_UART, true);
    MXC_UART_SetAutoDMAHandlers(WRITING_UART, true);
#endif

    // Set Parameters for UART transaction requests
    mxc_uart_req_t read_req;
    read_req.uart = READING_UART;
    read_req.rxData = RxData;
    read_req.rxLen = BUFF_SIZE;
    read_req.txLen = 0;
    read_req.callback = readCallback;

    mxc_uart_req_t write_req;
    write_req.uart = WRITING_UART;
    write_req.txData = TxData;
    write_req.txLen = BUFF_SIZE;
    write_req.rxLen = 0;
    write_req.callback = NULL;

    printf("-->Starting transaction\n");

    // Initiate Reading UART transaction
    READ_FLAG = 1;
    MXC_UART_ClearRXFIFO(READING_UART); // Clear any previously pending data
#ifdef DMA
    error = MXC_UART_TransactionDMA(&read_req);
#else // DMA
    error = MXC_UART_TransactionAsync(&read_req);
#endif // DMA

    if (error != E_NO_ERROR) {
        printf("-->Error starting read: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    // Do Writing UART transaction
    error = MXC_UART_Transaction(&write_req);
    if (error != E_NO_ERROR) {
        printf("-->Error starting sync write: %d\n", error);
        printf("-->Example Failed\n");
        return error;
    }

    // Wait for read transaction to complete
    while (READ_FLAG) {}

    printf("-->Transaction complete\n\n");

    if (READ_FLAG != E_NO_ERROR) {
        printf("-->Error from UART read callback; %d\n", READ_FLAG);
        fail++;
    }

    // Verify data received matches data transmitted
    if ((error = memcmp(RxData, TxData, BUFF_SIZE)) != 0) {
        printf("-->Error data mismatch: %d\n", error);
        fail++;
    } else {
        printf("-->Data verified\n");
    }

    // Indicate success/fail
    if (fail != 0) {
        LED_On(0);
        printf("\n-->Example Failed\n");
        return E_FAIL;
    } else {
        LED_On(1);
        printf("\n-->Example Succeeded\n");
    }

    return E_NO_ERROR;
}
