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

#ifndef LIBRARIES_FCL_INCLUDE_HASH_SHA256_H_
#define LIBRARIES_FCL_INCLUDE_HASH_SHA256_H_

#include <ucl/ucl_sha256.h>

/** The main loop of sha256.
 *
 * @param[in, out] hash The intermediate hash, u32[8]
 * @param[in] stone A "Stone" of the padded message, u32[16]
 *
 * @ingroup UCL_SHA256
 * @internal
 */
void sha256_stone(u32 hash[8], u32 stone[16]);


#endif // LIBRARIES_FCL_INCLUDE_HASH_SHA256_H_
