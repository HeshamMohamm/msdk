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
 * @file    board.h
 * @brief   Board support package API.
 */

#ifndef LIBRARIES_BOARDS_MAX78000_AUD01_REVA_INCLUDE_BOARD_H_
#define LIBRARIES_BOARDS_MAX78000_AUD01_REVA_INCLUDE_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <spi_regs.h>
#include <gpio_regs.h>
#include "led.h"

#define BOARD_AUD01_REVA

#ifndef CONSOLE_UART
#define CONSOLE_UART 0 /// UART instance to use for console
#endif

#ifndef CONSOLE_BAUD
#define CONSOLE_BAUD 19200 /// Console baud rate
#endif

#ifdef LED_OFF
#undef LED_OFF
#endif
#define LED_OFF 1 /// Override inactive state of LEDs

#ifdef LED_ON
#undef LED_ON
#endif
#define LED_ON 0 /// Override active state of LEDs

/**
 *  A reference to LED1 (RED LED in the RGB LED) of the board.
 *  Can be used with the LED_On, LED_Off, and LED_Toggle functions.
 */
#define LED1 0
#define LED_RED LED1

/**
 *  A reference to LED2 (GREEN LED in the RGB LED) of the board.
 *  Can be used with the LED_On, LED_Off, and LED_Toggle functions.
 */
#define LED2 1
#define LED_GREEN LED2

/**
 *  A reference to LED2 (BLUE LED in the RGB LED) of the board.
 *  Can be used with the LED_On, LED_Off, and LED_Toggle functions.
 */
#define LED3 2
#define LED_BLUE LED3

/**
 * \brief   Initialize the BSP and board interfaces.
 * \returns #E_NO_ERROR if everything is successful
 */
int Board_Init(void);

/**
 * \brief   Initialize or reinitialize the console. This may be necessary if the
 *          system clock rate is changed.
 * \returns #E_NO_ERROR if everything is successful
 */
int Console_Init(void);

/**
 * \brief   Shutdown the console.
 * \returns #E_NO_ERROR if everything is successful
 */
int Console_Shutdown(void);

/**
 * \brief   Attempt to prepare the console for sleep.
 * \returns #E_NO_ERROR if ready to sleep, #E_BUSY if not ready for sleep.
 */
int Console_PrepForSleep(void);

#ifdef __riscv
/**
 * \brief   Set up RISCV JTAG
 * \returns #E_NO_ERROR if successful
 */
int Debug_Init(void);
#endif // __riscv

/**
 * \brief   Microphone power control.
 *
 * \param   on          1 for ON, 0 for OFF
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
#define POWER_OFF 0
#define POWER_ON 1
int Microphone_Power(int on);

/**
 * \brief   Audio codec clock control.
 *
 * \param   enable      1 for clock generator enabled, 0 for clock generator disabled
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
#define CLOCK_DISABLE 0
#define CLOCK_ENABLE 1
int Audio_Codec_Clock_Enable(int enable);

/**
 * \brief   Internal/External I2S device selection.
 *
 * \param   sel         1 for external I2S device through I2S header pins, 0 for internal (on-board I2S device)
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
#define I2S_INTERNAL 0
#define I2S_EXTERNAL 1
int Internal_External_I2S_Select(int sel);

/**
 * \brief   External CNN voltage regulator control
 *
 * \param   sel         1 external voltage regulator for CNN is enabled, 0 external voltage regulator for CNN is disabled
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
#define CNN_BOOST_DISABLE 0
#define CNN_BOOST_ENABLE 1
int CNN_Boost_Enable(int enable);

/**
 * \brief   Camera power control.
 *
 * \param   on          1 for ON, 0 for OFF
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int Camera_Power(int on);

/**
 * \brief   SD card power control.
 *
 * \param   on          1 for ON, 0 for OFF
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int SD_Power(int on);

/**
 * \brief   Informs the caller which SPI connections are used for SD card communication
 *
 * \param   spi 		The SPI instance used
 * \param   ssPort      The GPIO port used for the SD card's SSEL pin
 * \param   ssPin       The GPIO pin number used for the SD card's SSEL pin
 *
 * \return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
void SD_Get_Connections(mxc_spi_regs_t **spi, mxc_gpio_regs_t **ssPort, int *ssPin);

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_BOARDS_MAX78000_AUD01_REVA_INCLUDE_BOARD_H_
