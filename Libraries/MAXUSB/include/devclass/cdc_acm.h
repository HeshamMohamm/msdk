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

#ifndef LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_CDC_ACM_H_
#define LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_CDC_ACM_H_

#include "usb_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  cdc_acm.h
 * @brief Communications Device Class ACM (Serial Port) over USB.
 */

#ifdef MAXUSB_HS_CAPABLE
#define ACM_MAX_PACKET    MXC_USBHS_MAX_PACKET
#else
#define ACM_MAX_PACKET    64
#endif

#define ACM_PARITY_NONE   0
#define ACM_PARITY_ODD    1
#define ACM_PARITY_EVEN   2
#define ACM_PARITY_MARK   3
#define ACM_PARITY_SPACE  4

#define ACM_STOP_1        0
#define ACM_STOP_15       1
#define ACM_STOP_2        2

/// Configuration structure
typedef struct {
  uint8_t out_ep;            // endpoint to be used for OUT packets
  uint16_t out_maxpacket;    // max packet size for OUT endpoint
  uint8_t in_ep;             // endpoint to be used for IN packets
  uint16_t in_maxpacket;     // max packet size for IN endpoint
  uint8_t notify_ep;         // endpoint to be used for notifications
  uint16_t notify_maxpacket; // max packet size for notifications
} acm_cfg_t;

/// Line coding
#if defined(__GNUC__)
typedef struct __attribute__((packed)) {
#else
typedef __packed struct {
#endif
  uint32_t speed;   /// baud rate in bps
  uint8_t stopbits;
  uint8_t parity;
  uint8_t databits;
} acm_line_t;

/// CDC-ACM callback events
typedef enum {
  ACM_CB_CONNECTED,       /// upon host connection
  ACM_CB_DISCONNECTED,    /// upon host disconnection
  ACM_CB_SET_LINE_CODING, /// when new line coding parameters are received
  ACM_CB_READ_READY,      /// when new data is available from the host
  ACM_NUM_CALLBACKS       /// number of callback events for internal use
} acm_callback_t;

/**
 *  \brief    Initialize the class driver
 *  \details  Initialize the class driver.
 *  \param    if_desc  Pointer to the interface descriptor for the Comm Class
 *  \return   Zero (0) for success, non-zero for failure
 */
int acm_init(const MXC_USB_interface_descriptor_t *if_desc);

/**
 *  \brief    Set the specified configuration
 *  \details  Configures the class and endpoints and starts operation. This function should be
 *            called upon configuration from the host.
 *  \param    cfg   configuration to be set
 *  \return   Zero (0) for success, non-zero for failure
 */
int acm_configure(const acm_cfg_t *cfg);

/**
 *  \brief    Clear the current configuration and resets endpoints
 *  \details  Clear the current configuration and resets endpoints.
 *  \return   Zero (0) for success, non-zero for failure
 */
int acm_deconfigure(void);

/**
 *  \brief    Returns the current DTE status.
 *  \return   '1' if DTE is present, '0' otherwise
 */
int acm_present(void);

/**
 *  \brief    This function is used to get the current line coding
 *  \details  Upon an #ACM_CB_SET_LINE_CODING event, this function can be used to get the new
 *            line coding parameters.
 *  \return   pointer to the current line coding
 */
const acm_line_t *acm_line_coding(void);

/**
 *  \brief    Register a callback to be called upon the specified event.
 *  \details  Register a callback to be called upon the specified event. To disable the
 *            callback, call this function with a NULL parameter.
 *  \return   Zero (0) for success, non-zero for failure
 *  \note     Callbacks are executed in interrupt context
 */
int acm_register_callback(acm_callback_t cbnum, int (*func)(void));

/**
 *  \brief    Get the number of characters available to be read.
 *  \return   The number of characters available to be read.
 */
int acm_canread(void);

/**
 *  \brief    Read the specified number of characters.
 *  \details  Read the specified number of characters. This function blocks until the specified
 *            number of characters have been received.
 *  \param    buf   buffer to store the characters in
 *  \param    len   number of characters to read
 *  \return   Number of characters read, 0 if connection closes, -1 on error, or -2 if BREAK
 *            signal received.
 */
int acm_read(uint8_t *buf, unsigned int len);

/**
 *  \brief    Write the specified number of characters.
 *  \details  Write the specified number of characters. This function blocks until all characters
 *            have been transferred to and internal FIFO.
 *  \param    buf   buffer containing the characters to be sent
 *  \param    len   number of characters to write
 *  \return   The number of characters successfully written.
 *  \note     On some processors, the actually USB transaction is performed asynchronously, after
 *            this function returns. Successful return from this function does not guarantee
 *            successful reception of characters by the host.
 */
int acm_write(uint8_t *buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_CDC_ACM_H_
