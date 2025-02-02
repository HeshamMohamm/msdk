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

#include <ucl/ucl_hash.h>
#ifdef HASH_SHA256

#include "ucl/ucl_config.h"
#include "ucl/ucl_types.h"
#include "ucl/ucl_defs.h"
#include "ucl/ucl_retdefs.h"

#define ROTR(x, n)      (((x) >> (n)) | ((x) << (32 - n)))
#define SHR(x, n)       ((x) >> (n))
#define CH(x, y, z)     ((z) ^ ((x) & ((y) ^ (z))))
#define MAJ(x, y, z)    (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x)       (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIGMA1(x)       (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define GAMMA0(x)       (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define GAMMA1(x)       (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))
#define ROUND(a, b, c, e, f, g, h, i)                            \
                T1 = h + SIGMA1(e) + CH(e, f, g) + K[i] + W[i];  \
                T2 = SIGMA0(a) + MAJ(a, b, c);

static const u32 K[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
               0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
               0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
               0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
               0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
               0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
               0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
               0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

void sha256_stone(u32 hash[8], u32 stone[16])
{
    int i;
    u32 W[64];
    u32 A, B, C, D, E, F, G, H, T1, T2;

    for (i = 0; i < 16; i++) {
        W[i] = stone[i];
    }

    for (; i < 64; i++) {
        W[i] = GAMMA1(W[i-2]) + W[i-7] + GAMMA0(W[i-15]) + W[i-16];
    }

    A = hash[0];
    B = hash[1];
    C = hash[2];
    D = hash[3];
    E = hash[4];
    F = hash[5];
    G = hash[6];
    H = hash[7];

    for (i = 0; i < 64; i++) {
        ROUND(A, B, C, E, F, G, H, i);
        H = G;
        G = F;
        F = E;
        E = D + T1;
        D = C;
        C = B;
        B = A;
        A = T1 + T2;
    }

    hash[0] += A;
    hash[1] += B;
    hash[2] += C;
    hash[3] += D;
    hash[4] += E;
    hash[5] += F;
    hash[6] += G;
    hash[7] += H;
}

#endif//HASH_SHA256
