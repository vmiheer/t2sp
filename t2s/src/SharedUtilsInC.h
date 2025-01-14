/*******************************************************************************
* Copyright 2021 Intel Corporation
*
* Licensed under the BSD-2-Clause Plus Patent License (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://opensource.org/licenses/BSDplusPatent
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions
* and limitations under the License.
*
*
* SPDX-License-Identifier: BSD-2-Clause-Patent
*******************************************************************************/
#ifndef SHARED_UTILS_IN_C
#define SHARED_UTILS_IN_C

/* This file contains common utilities shared by AOT runtime and roofline drawing. */

// Return the bitstream file name with full path.
// Caller: free the space after usage.
extern "C" char *bistream_file_name_with_absolute_path();

// Return the directory of the bitstream file.
// Caller: free the space after usage.
extern "C" char *bitstream_directory();

// Return the directory where Quartus outputs FPGA synthesis results like acl_quartus_report.txt.
// Caller: free the space after usage.
extern "C" char *quartus_output_directory();

// Allocate space and concatenate the directory and the file name there.
// Caller: free the space after usage.
extern "C" char *concat_directory_and_file(const char *dir, const char *file);

#endif
