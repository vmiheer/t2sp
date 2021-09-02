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
#include "util.h"

int main(void) {
    // Define the compute.
    ImageParam a(Int(32), 1, "a");
    Func A(PLACE0);
    Var i;
    A(i) = a(i) * 2; 

    Func A_feeder(PLACE1);
    A_feeder(i) = i * 2;
    try {
        // This will incur an error since isolation assumes the producer, A_feeder, has no definition.
        A.isolate_producer_chain(a, A_feeder);
    } catch (Halide::CompileError & e) {
        cout << e.what() << "Success!\n";
        exit(0);
    }
    cout << "Failed!\n";
    exit(1);
}

