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
 * @brief   Bootloader Host Example
 *
 * @details This example provide a menu on terminal, depend on the selection it run tests.
 */

/*******************************      INCLUDES    ****************************/
#include <stdio.h>
#include <stdint.h>

#include "terminal.h"
#include "bootloader.h"
#include "platform.h"

#include "test_images.h"

/*******************************      DEFINES     ****************************/

/******************************* Type Definitions ****************************/

/*******************************    Variables   ****************************/
extern int bootloader_menu(const char *parentName);

/******************************* Static Functions ****************************/
static int select_interface(const char *parentName)
{
    int ret;

    list_t list[] = {
        { "I2C", NULL },
        { "SPI", NULL },
    };

    while (1) {
        ret = terminal_select_from_list(parentName, list, sizeof(list) / sizeof(list[0]), 1);
        if (ret == KEY_CANCEL) {
            break;
        }

        if (ret == 1) {
            bl_update_interface(plt_i2c_read, plt_i2c_write);
            terminal_printf("Host-<->Target interface configured as I2C\r\n");
            ret = 0; // means success
            break;
        } else if (ret == 2) {
            bl_update_interface(plt_spi_read, plt_spi_write);
            terminal_printf("Host-<->Target interface configured as SPI\r\n");
            ret = 0; // means success
            break;
        }
    }

    return ret;
}

static int update_fw_nonsecure_MAX32662_blinkled_slow_P0_13(const char *parentName)
{
    return bl_flash_image(nonsecure_MAX32662_blinkled_slow_P0_13);
}

static int update_fw_nonsecure_MAX32662_blinkled_fast_P0_13(const char *parentName)
{
    return bl_flash_image(nonsecure_MAX32662_blinkled_fast_P0_13);
}

static int update_fw_devkey_MAX32662_blinkled_fast_P0_13(const char *parentName)
{
    return bl_flash_image(devkey_MAX32662_blinkled_fast_P0_13);
}

static int update_fw_devkey_MAX32662_blinkled_slow_P0_13(const char *parentName)
{
    return bl_flash_image(devkey_MAX32662_blinkled_slow_P0_13);
}

/******************************* Public Functions ****************************/
static list_t list[] = {
    { "Select Interface", select_interface },
    { "Bootloader Test Menu", bootloader_menu },
    { "Load MSBL: Non-secure MAX32662 blinkled fast P0.13",
      update_fw_nonsecure_MAX32662_blinkled_fast_P0_13 },
    { "Load MSBL: Non-secure MAX32662 blinkled slow P0.13",
      update_fw_nonsecure_MAX32662_blinkled_slow_P0_13 },
    { "Load MSBL: MAX32662 blinkled fast P0.13 (development key)",
      update_fw_devkey_MAX32662_blinkled_fast_P0_13 },
    { "Load MSBL: MAX32662 blinkled slow P0.13 (development key)",
      update_fw_devkey_MAX32662_blinkled_slow_P0_13 },
};

int main(void)
{
    plt_i2c_init();
    plt_spi_init();
    plt_gpio_init();

    // Initialize terminal uart after SPI initialization because some pins (P0.20, P0.21) are overlaps
    terminal_init();

    terminal_printf("\r\n***********Bootloader Host Example***********\r\n");
    terminal_printf("The purpose of this example is:\r\n");
    terminal_printf("   1- Demonstrate how bootloader device can be program\r\n");
    terminal_printf("   2- Provide platform independent bootloader protocol (files under "
                    "bootloader folder)\r\n");
    terminal_printf("\r\nThis example can be ported on any platform\r\n");
    terminal_printf("If you would like to port it on other platform\r\n");
    terminal_printf("you need to update terminal.c and platform_max32662.c files\r\n");
    terminal_printf("\r\n");
    terminal_printf("\tHW Pins\r\n");
    terminal_printf("\tI2C:    SCL(P0.2),   SDA(P0.3) (Note: I2C requires pullup resistor) \r\n");
    terminal_printf("\tSPI:    MISO(P0.4),  MOSI(P0.5),  SCK(P0.6),  SS(P0.7)\r\n");
    terminal_printf("\tTarget: RESET(P0.8), MFIO(P0.9)\r\n");
    terminal_printf("\r\n");
    terminal_printf("UART1 is used as terminal comport\r\n");

    bl_conf_struct_t plt;

    plt.read = plt_spi_read;
    plt.write = plt_spi_write;
    plt.gpio_set = plt_gpio_set;
    plt.delay_ms = plt_delay_ms;
    plt.printf = terminal_printf;

    bl_init(&plt);

    while (1) {
        terminal_select_from_list("Main Menu", list, sizeof(list) / sizeof(list[0]), 1);
    }
}
