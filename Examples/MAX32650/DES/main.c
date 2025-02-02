/**
 * @file        main.c
 * @brief       Data Encryption Standard (DES) Example.
 * @details     Encryption and decryption of DES on different modes (ECB) with
 *              different bit sizes (128, 192, and 256)
 */

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

/* **** Includes **** */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_errors.h"
#include "tpu.h"

/* **** Definitions **** */
#define MXC_DES_DATA_LEN \
    (64 /                \
     8) /**< Number of bytes in an DES plaintext or cyphertext block, which are always 64-bits long. */
#define MXC_DES_KEY_LEN (64 / 8) /**< Number of bytes in a TDES key. */
#define MXC_TDES_KEY_LEN (192 / 8) /**< Number of bytes in a TDES key. */

/* **** Globals **** */
char result[512];

/* **** Functions **** */
/* Convert ascii to byte */
void ascii_to_byte(const char *src, char *dst, int len)
{
    int i;
    int val;
    char temp[3];

    temp[2] = 0x00;
    for (i = 0; i < len; ++i) {
        temp[0] = *src;
        src++;
        temp[1] = *src;
        src++;
        sscanf(temp, "%0x", &val);
        dst[i] = val;
    }

    return;
}

/* Verify by comparing calculated to expected */
int DES_check(char *calculated, char *expected, int len)
{
    int i, fail = 0;

    for (i = 0; i < len; ++i) {
        if (calculated[i] != expected[i]) {
            ++fail;
        }
    }

    if (fail > 0) {
        printf("Fail.\n");
        return -1;
    }

    printf("Pass.\n");
    return 0;
}

int DES_ECB_enc(void)
{
    char *xkey = "2f5d4b8c12a4a9c1";
    char key[MXC_DES_KEY_LEN];
    char *iv_src = "";
    char iv_dst[MXC_DES_DATA_LEN];
    char *xmsg = "0000000000000000";
    char msg[MXC_DES_DATA_LEN];
    char *xexpected = "20597b6decaf7166";
    char expected[MXC_DES_DATA_LEN];

    ascii_to_byte(xkey, key, MXC_DES_KEY_LEN);
    ascii_to_byte(iv_src, iv_dst, MXC_DES_DATA_LEN);
    ascii_to_byte(xmsg, msg, MXC_DES_DATA_LEN);

    MXC_TPU_Cipher_Config(MXC_TPU_MODE_ECB, MXC_TPU_CIPHER_DES);
    MXC_TPU_Cipher_DES_Encrypt(msg, iv_dst, key, MXC_TPU_MODE_ECB, MXC_DES_DATA_LEN, result);

    ascii_to_byte(xexpected, expected, MXC_DES_DATA_LEN);

    return DES_check(result, expected, MXC_DES_DATA_LEN);
}

int DES_ECB_dec(void)
{
    char *xkey = "00c3de5446614d35";
    char key[MXC_DES_KEY_LEN];
    char *iv_src = "";
    char iv_dst[MXC_DES_DATA_LEN];
    char *xct = "d940635dcb8148ae";
    char ct[MXC_DES_DATA_LEN];
    char *xexpected = "0000000000000000";
    char expected[MXC_DES_DATA_LEN];

    ascii_to_byte(xkey, key, MXC_DES_KEY_LEN);

    ascii_to_byte(iv_src, iv_dst, MXC_DES_DATA_LEN);

    ascii_to_byte(xct, ct, MXC_DES_DATA_LEN);

    MXC_TPU_Cipher_Config(MXC_TPU_MODE_ECB, MXC_TPU_CIPHER_DES);
    MXC_TPU_Cipher_DES_Decrypt(ct, iv_dst, key, MXC_TPU_MODE_ECB, MXC_DES_DATA_LEN, result);

    ascii_to_byte(xexpected, expected, MXC_DES_DATA_LEN);

    return DES_check(result, expected, MXC_DES_DATA_LEN);
}

int TDES_ECB_enc(void)
{
    char *xkey = "0fb5b906471296bc1ab269585e1c99dcf10dd7b047cdee29";
    char key[MXC_TDES_KEY_LEN];
    char *iv_src = "";
    char iv_dst[MXC_DES_DATA_LEN];
    char *xpt = "0000000000000000";
    char pt[MXC_DES_DATA_LEN];
    char *xexpected = "d05ef547adf0db98";
    char expected[MXC_DES_DATA_LEN];

    ascii_to_byte(xkey, key, MXC_TDES_KEY_LEN);

    ascii_to_byte(iv_src, iv_dst, MXC_DES_DATA_LEN);

    ascii_to_byte(xpt, pt, MXC_DES_DATA_LEN);

    MXC_TPU_Cipher_Config(MXC_TPU_MODE_ECB, MXC_TPU_CIPHER_TDEA);
    MXC_TPU_Cipher_TDES_Encrypt(pt, iv_dst, key, MXC_TPU_MODE_ECB, MXC_DES_DATA_LEN, result);

    ascii_to_byte(xexpected, expected, MXC_DES_DATA_LEN);

    return DES_check(result, expected, MXC_DES_DATA_LEN);
}

int TDES_ECB_dec(void)
{
    char *xkey = "2e0a67fe76bc3d3c1081c45a48784f49c876033acc85f69c";
    char key[MXC_TDES_KEY_LEN];
    char *iv_src = "";
    char iv_dst[MXC_DES_DATA_LEN];
    char *xct = "2a78627595b42376";
    char ct[MXC_DES_DATA_LEN];
    char *xexpected = "0000000000000000";
    char expected[MXC_DES_DATA_LEN];

    ascii_to_byte(xkey, key, MXC_TDES_KEY_LEN);
    ascii_to_byte(iv_src, iv_dst, MXC_DES_DATA_LEN);
    ascii_to_byte(xct, ct, MXC_DES_DATA_LEN);

    MXC_TPU_Cipher_Config(MXC_TPU_MODE_ECB, MXC_TPU_CIPHER_TDEA);
    MXC_TPU_Cipher_TDES_Decrypt(ct, iv_dst, key, MXC_TPU_MODE_ECB, MXC_DES_DATA_LEN, result);

    ascii_to_byte(xexpected, expected, MXC_DES_DATA_LEN);

    return DES_check(result, expected, MXC_DES_DATA_LEN);
}

// *****************************************************************************
int main(void)
{
    printf("\n***** DES Example *****\n");

    int fail = 0;

    printf("DES ECB Encryption ... ");
    fail += DES_ECB_enc();
    printf("DES ECB Decryption ... ");
    fail += DES_ECB_dec();
    printf("Triple DES ECB Encryption ... ");
    fail += TDES_ECB_enc();
    printf("Triple DES ECB Decryption ... ");
    fail += TDES_ECB_dec();

    if (fail != 0) {
        printf("\nExample Failed\n");
        return E_FAIL;
    }

    printf("\nExample Succeeded\n");
    return E_NO_ERROR;
}
