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
#include "Halide.h"
#include "util.h"
#include "sizes.h"

using namespace Halide;

int main()
{
    // Dependences
    #define P               kkk,      jjj,  iii,  jj, ii, kk,     k,  j,i
    #define P_kkk_minus_1   kkk-1,    jjj,  iii,  jj, ii, kk,     k,  j,i
    #define P_kk_minus_1    kkk+KKK-1,jjj,  iii,  jj, ii, kk-1,   k,  j,i
    #define P_k_minus_1     kkk+KKK-1,jjj,  iii,  jj, ii, kk+KK-1,k-1,j,i
    #define P_jjj_minus_1   kkk,      jjj-1,iii,  jj, ii, kk,     k,  j,i
    #define P_iii_minus_1   kkk,      jjj,  iii-1,jj, ii, kk,     k,  j,i
    #define P_Out                     jjj,  iii,  jj, ii,             j,i

    // Linearized addresses
    #define total_i         (iii + III * ii + III * II * i)
    #define total_j         (jjj + JJJ * jj + JJJ * JJ * j)
    #define total_k         (kkk + KKK * kk + KKK * KK * k)

    // Type of the data to process in C and T2S
    #define CTYPE float
    #define TTYPE Float(32)

    // Inputs
    ImageParam A("A", TTYPE, 2), B("B", TTYPE, 2);


    // UREs
    Var kkk("kkk"), jjj("jjj"), iii("iii"), jj("jj"), ii("ii"), kk("kk"), k("k"), j("j"), i("i");
    URE X("X", TTYPE, {P}), Y("Y", TTYPE, {P}), Z("Z", TTYPE, {P}), Out("Out");
    X(P) = select(jjj == 0, A(total_k, total_i), X(P_jjj_minus_1));
    Y(P) = select(iii == 0, B(total_j, total_k), Y(P_iii_minus_1));
    Z(P) = select(kkk == 0 && kk == 0 && k == 0, 0,
                select(kkk == 0, select(kk == 0, Z(P_k_minus_1), Z(P_kk_minus_1)), Z(P_kkk_minus_1)))
                + X(P) * Y(P);
    Out(P_Out) = select(kkk == KKK-1 && kk == KK-1 && k == K-1, Z(P));

    // Put all the UREs inside the same loop nest of X.
    X.merge_ures(Y, Z, Out);

    // Explicitly set the loop bounds
    X.set_bounds(jjj, 0, JJJ, iii, 0, III, kkk, 0, KKK)
     .set_bounds(jj,  0, JJ,  ii,  0, II,  kk,  0, KK)
     .set_bounds(j,   0, B.dim(0).extent() / (JJJ * JJ),
                 i,   0, A.dim(1).extent() / (III * II),
                 k,   0, A.dim(0).extent() / (KKK * KK));

    // Create a systolic array
    X.space_time_transform(kkk, jjj, iii);

    // GPU can have many threads running in parallel.
#ifdef GPU
    X.gpu_blocks(j, i).gpu_threads(jj, ii);
#endif

    // I/O network
    Stensor DA("aLoader", DRAM), SA("aFeeder", SRAM), DB("bLoader", DRAM), SB("bFeeder", SRAM);
    Stensor RC2("drainer", REG), RC1("collector", REG), DC("unloader", DRAM), C("deserializer");
    A >> DA.bankwidth(kkk) >> FIFO(128)
      >> SA.scope(k).banks(iii).bankwidth(kkk) >> FIFO(128);
    B >> DB.bankwidth(kkk) >> FIFO(128)
      >> SB.scope(k).banks(jjj).bankwidth(kkk) >> FIFO(128);
    Out >> FIFO(1024) >> RC2.scope(jj).banks(jjj, iii)
        >> FIFO(128)  >> RC1.scope(iii).banks(jjj)
        >> FIFO(128)  >> DC >> C;

    // Compile the kernel to an FPGA bitstream, and expose a C interface for the host to invoke
    C.compile_to_host("gemm-interface", { A, B }, "GEMM", IntelFPGA);
    printf("Success\n");
    return 0;
}
