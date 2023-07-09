/*
 * validus.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2004-2023
 * Version:   1.0.1
 * License:   The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "validus.h"
#include <string.h>

void validus_init(validus_state* state)
{
    if (!state)
        return;

    state->bits[0] = state->bits[1] = 0;
    state->f0 = VALIDUS_INIT_0;
    state->f1 = VALIDUS_INIT_1;
    state->f2 = VALIDUS_INIT_2;
    state->f3 = VALIDUS_INIT_3;
    state->f4 = VALIDUS_INIT_4;
    state->f5 = VALIDUS_INIT_5;
}

void validus_append(validus_state* state, const void* data, validus_word len)
{
    if (!state || !data || len == 0)
        return;

    const validus_word* ptr = (const validus_word*)data;
    validus_word left       = 0;
    validus_word done       = 0;

    state->bits[1] += (len >> 29);

    if (((state->bits[0] += (len << 3)) < (len << 3)))
        state->bits[1]++;

    while ((left = (len - done))) {
        if (left >= VALIDUS_FP_SIZE_B) {
            _validus_process(state, ptr);
            done += VALIDUS_FP_SIZE_B;
            ptr  += VALIDUS_FP_SIZE_O;
        } else {
            validus_word stk[VALIDUS_FP_SIZE_O];
            memcpy(stk, ptr, left);
            memset(((validus_octet*)stk) + left, 0, VALIDUS_FP_SIZE_B - left);
            _validus_process(state, stk);
            done += left;
        }
    }
}

void validus_finalize(validus_state* state)
{
    if (!state)
        return;

    validus_octet finish[VALIDUS_FP_SIZE_B] = {
        0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    OCTETSWAP(state->bits[1], ((validus_octet*)&state->bits[1]));
    OCTETSWAP(state->bits[0], ((validus_octet*)&state->bits[0]));

    memcpy(&finish[184], &state->bits[1], 4);
    memcpy(&finish[188], &state->bits[0], 4);

    _validus_process(state, (validus_word*)finish);
}

bool validus_compare(const validus_state* one, const validus_state* two)
{
    if (!one || !two)
        return false;

    return (one->f0 == two->f0 && one->f1 == two->f1 &&
            one->f2 == two->f2 && one->f3 == two->f3 &&
            one->f4 == two->f4 && one->f5 == two->f5);
}

void _validus_process(validus_state* state, const validus_word* blk32)
{
    if (!state || !blk32)
        return;

    validus_word a = state->f0;
    validus_word b = state->f1;
    validus_word c = state->f2;
    validus_word d = state->f3;
    validus_word e = state->f4;
    validus_word f = state->f5;

    validus_word stk[VALIDUS_FP_SIZE_O];

#ifdef VALIDUS_BIG_ENDIAN
    for(validus_int n = VALIDUS_FP_SIZE_O - 1; n >= 0; n--)
        OCTETSWAP(stk[n], ((validus_octet*)&blk32[n]));
    blk32 = stk;
#else
    if (!WORDALIGNED(blk32)) {
        memcpy(stk, blk32, VALIDUS_FP_SIZE_B);
        blk32 = stk;
    }
#endif

    VC_0(d, c, b, a, f, e,  2,  3, blk32[47], VALIDUS_0);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[46], VALIDUS_1);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[45], VALIDUS_2);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[44], VALIDUS_3);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[43], VALIDUS_4);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[42], VALIDUS_5);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[41], VALIDUS_6);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[40], VALIDUS_7);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[39], VALIDUS_8);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[38], VALIDUS_9);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[37], VALIDUS_10);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[36], VALIDUS_11);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[35], VALIDUS_12);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[34], VALIDUS_13);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[33], VALIDUS_14);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[32], VALIDUS_15);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[31], VALIDUS_16);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[30], VALIDUS_17);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[29], VALIDUS_18);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[28], VALIDUS_19);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[27], VALIDUS_20);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[26], VALIDUS_21);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[25], VALIDUS_22);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[24], VALIDUS_23);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[ 0], VALIDUS_24);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[ 1], VALIDUS_25);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[ 2], VALIDUS_26);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[ 3], VALIDUS_27);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[ 4], VALIDUS_28);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[ 5], VALIDUS_29);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[ 6], VALIDUS_30);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[ 7], VALIDUS_31);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[ 8], VALIDUS_32);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[ 9], VALIDUS_33);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[10], VALIDUS_34);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[11], VALIDUS_35);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[12], VALIDUS_36);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[13], VALIDUS_37);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[14], VALIDUS_38);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[15], VALIDUS_39);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[16], VALIDUS_40);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[17], VALIDUS_41);
    VC_0(d, c, b, a, f, e,  2,  3, blk32[18], VALIDUS_42);
    VC_0(c, b, a, f, e, d,  7,  6, blk32[19], VALIDUS_43);
    VC_0(b, a, f, e, d, c, 10,  9, blk32[20], VALIDUS_44);
    VC_0(a, f, e, d, c, b, 15, 12, blk32[21], VALIDUS_45);
    VC_0(f, e, d, c, b, a, 20, 21, blk32[22], VALIDUS_46);
    VC_0(e, d, c, b, a, f, 25, 24, blk32[23], VALIDUS_47);

    VC_1(d, c, b, a, f, e,  5,  4, blk32[22], VALIDUS_48);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[20], VALIDUS_49);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[18], VALIDUS_50);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[23], VALIDUS_51);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[21], VALIDUS_52);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[19], VALIDUS_53);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[16], VALIDUS_54);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[14], VALIDUS_55);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[12], VALIDUS_56);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[17], VALIDUS_57);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[15], VALIDUS_58);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[13], VALIDUS_59);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[10], VALIDUS_60);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[ 8], VALIDUS_61);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[ 6], VALIDUS_62);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[11], VALIDUS_63);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[ 9], VALIDUS_64);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[ 7], VALIDUS_65);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[ 4], VALIDUS_66);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[ 2], VALIDUS_67);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[ 0], VALIDUS_68);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[ 5], VALIDUS_69);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[ 3], VALIDUS_70);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[ 1], VALIDUS_71);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[25], VALIDUS_72);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[27], VALIDUS_73);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[29], VALIDUS_74);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[24], VALIDUS_75);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[26], VALIDUS_76);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[28], VALIDUS_77);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[31], VALIDUS_78);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[33], VALIDUS_79);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[35], VALIDUS_80);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[30], VALIDUS_81);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[32], VALIDUS_82);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[34], VALIDUS_83);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[37], VALIDUS_84);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[39], VALIDUS_85);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[41], VALIDUS_86);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[36], VALIDUS_87);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[38], VALIDUS_88);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[40], VALIDUS_89);
    VC_1(d, c, b, a, f, e,  5,  4, blk32[43], VALIDUS_90);
    VC_1(c, b, a, f, e, d, 13, 14, blk32[45], VALIDUS_91);
    VC_1(b, a, f, e, d, c, 17, 16, blk32[47], VALIDUS_92);
    VC_1(a, f, e, d, c, b, 22, 19, blk32[42], VALIDUS_93);
    VC_1(f, e, d, c, b, a, 26, 23, blk32[44], VALIDUS_94);
    VC_1(e, d, c, b, a, f, 28, 29, blk32[46], VALIDUS_95);

    VC_2(d, c, b, a, f, e,  3,  2, blk32[ 1], VALIDUS_96);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[ 0], VALIDUS_97);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[ 3], VALIDUS_98);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[ 2], VALIDUS_99);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[ 5], VALIDUS_100);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[ 4], VALIDUS_101);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[ 7], VALIDUS_102);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[ 6], VALIDUS_103);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[ 9], VALIDUS_104);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[ 8], VALIDUS_105);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[11], VALIDUS_106);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[10], VALIDUS_107);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[13], VALIDUS_108);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[12], VALIDUS_109);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[15], VALIDUS_110);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[14], VALIDUS_111);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[17], VALIDUS_112);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[16], VALIDUS_113);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[19], VALIDUS_114);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[18], VALIDUS_115);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[21], VALIDUS_116);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[20], VALIDUS_117);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[23], VALIDUS_118);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[22], VALIDUS_119);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[46], VALIDUS_120);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[47], VALIDUS_121);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[44], VALIDUS_122);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[45], VALIDUS_123);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[42], VALIDUS_124);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[43], VALIDUS_125);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[40], VALIDUS_126);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[41], VALIDUS_127);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[38], VALIDUS_128);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[39], VALIDUS_129);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[36], VALIDUS_130);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[37], VALIDUS_131);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[34], VALIDUS_132);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[35], VALIDUS_133);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[32], VALIDUS_134);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[33], VALIDUS_135);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[30], VALIDUS_136);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[31], VALIDUS_137);
    VC_2(d, c, b, a, f, e,  3,  2, blk32[28], VALIDUS_138);
    VC_2(c, b, a, f, e, d,  6,  7, blk32[29], VALIDUS_139);
    VC_2(b, a, f, e, d, c,  9, 10, blk32[26], VALIDUS_140);
    VC_2(a, f, e, d, c, b, 12, 15, blk32[27], VALIDUS_141);
    VC_2(f, e, d, c, b, a, 21, 20, blk32[24], VALIDUS_142);
    VC_2(e, d, c, b, a, f, 24, 25, blk32[25], VALIDUS_143);

    VC_3(d, c, b, a, f, e,  4,  5, blk32[24], VALIDUS_144);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[26], VALIDUS_145);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[28], VALIDUS_146);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[25], VALIDUS_147);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[27], VALIDUS_148);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[29], VALIDUS_149);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[30], VALIDUS_150);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[32], VALIDUS_151);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[34], VALIDUS_152);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[31], VALIDUS_153);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[33], VALIDUS_154);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[35], VALIDUS_155);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[36], VALIDUS_156);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[38], VALIDUS_157);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[40], VALIDUS_158);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[37], VALIDUS_159);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[39], VALIDUS_160);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[41], VALIDUS_161);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[42], VALIDUS_162);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[44], VALIDUS_163);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[46], VALIDUS_164);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[43], VALIDUS_165);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[45], VALIDUS_166);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[47], VALIDUS_167);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[23], VALIDUS_168);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[21], VALIDUS_169);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[19], VALIDUS_170);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[22], VALIDUS_171);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[20], VALIDUS_172);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[18], VALIDUS_173);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[17], VALIDUS_174);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[15], VALIDUS_175);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[13], VALIDUS_176);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[16], VALIDUS_177);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[14], VALIDUS_178);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[12], VALIDUS_179);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[11], VALIDUS_180);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[ 9], VALIDUS_181);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[ 7], VALIDUS_182);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[10], VALIDUS_183);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[ 8], VALIDUS_184);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[ 6], VALIDUS_185);
    VC_3(d, c, b, a, f, e,  4,  5, blk32[ 5], VALIDUS_186);
    VC_3(c, b, a, f, e, d, 14, 13, blk32[ 3], VALIDUS_187);
    VC_3(b, a, f, e, d, c, 16, 17, blk32[ 1], VALIDUS_188);
    VC_3(a, f, e, d, c, b, 19, 22, blk32[ 4], VALIDUS_189);
    VC_3(f, e, d, c, b, a, 23, 26, blk32[ 2], VALIDUS_190);
    VC_3(e, d, c, b, a, f, 29, 28, blk32[ 0], VALIDUS_191);

    state->f0 += a;
    state->f1 += b;
    state->f2 += c;
    state->f3 += d;
    state->f4 += e;
    state->f5 += f;
}
