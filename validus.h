/*
 * validus.h
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
#ifndef _VALIDUS_H_INCLUDED
#define _VALIDUS_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

///////////////////////////// preprocessor /////////////////////////////////////
#if defined(__linux__)  || defined(__GNU__)    || defined(__CYGWIN__) || \
    defined(__GLIBC__)  || defined(__HAIKU__)  || defined(__APPLE__)
# if defined(__APPLE__)
#  include <machine/endian.h>
# else
#  include <endian.h>
# endif
# if !defined(LITTLE_ENDIAN) && defined(__LITTLE_ENDIAN)
#  define LITTLE_ENDIAN __LITTLE_ENDIAN
# endif
# if !defined(BIG_ENDIAN) && defined(__BIG_ENDIAN)
#  define BIG_ENDIAN __BIG_ENDIAN
# endif
# if !defined(BYTE_ORDER) && defined(__BYTE_ORDER)
#  define BYTE_ORDER __BYTE_ORDER
# endif
#endif

#if defined(__sun)
# include <sys/byteorder.h>
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# if defined(_BIG_ENDIAN)
#  define BYTE_ORDER BIG_ENDIAN
# elif defined(_LITTLE_ENDIAN)
# else
#  error "failed to determine endian-ness of this Sun system."
# endif
#endif

#if defined(_AIX) && !defined(BYTE_ORDER)
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# if defined(__BIG_ENDIAN__)
#  define BYTE_ORDER BIG_ENDIAN
# elif defined(__LITTLE_ENDIAN__)
#  define BYTE_ORDER LITTLE_ENDIAN
# else
#  error "failed to determine endian-ness of this IBM AIX system."
# endif
#endif

#if defined(_WIN32)
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# define BYTE_ORDER LITTLE_ENDIAN
#endif

#if !defined(BYTE_ORDER) || !defined(LITTLE_ENDIAN) || !defined(BIG_ENDIAN)
# error "failed to determine endian-ness of this system."
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
# undef VALIDUS_BIG_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
# define VALIDUS_BIG_ENDIAN
#endif

/////////////////////////////// typedefs ///////////////////////////////////////

typedef uint8_t  validus_octet; /**< Validus octet/byte (8-bit). */
typedef uint32_t validus_word;  /**< Validus word (32-bit). */
typedef int32_t  validus_int;   /**< Validus integer (32-bit). */

/**
 * @struct validus_state
 * Represents the state of a Validus hash operation.
 */
typedef struct {
    validus_word bits[2]; /**< 64-bit bit counter */
    validus_word f0;      /**< word 1 */
    validus_word f1;      /**< word 2 */
    validus_word f2;      /**< word 3 */
    validus_word f3;      /**< word 4 */
    validus_word f4;      /**< word 5 */
    validus_word f5;      /**< word 6 */
} validus_state;

#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////// function exports ///////////////////////////////////

/**
 * @brief Initializes a validus_state object.
 *
 * @note Must be called before ::validus_append.
 * @note If `state` is NULL, this function will return early and have no effect.
 *
 * @param state Pointer to the validus_state object to initialize.
 */
void validus_init(validus_state* state);

/**
 * @brief Processes a block of data, accumulating the results in
 * the validus_state object.
 *
 * @note If `state` or `data` are NULL, or `len` is zero, this function will
 * return early, and have no effect.
 *
 * @param state Pointer to the validus_state object in use for this series of data.
 * @param data  Pointer to a new block of data to process.
 * @param len   Length of `data` in octets.
 */
void validus_append(validus_state* state, const void* data, validus_word len);

/**
 * @brief Finalizes a Validus hashing operation.
 *
 * @note Must be called after the final call to ::validus_append.
 * @note If `state` is NULL, this function will return early and have no effect.
 *
 * @param state Pointer to the validus_state object to finalize.
 */
void validus_finalize(validus_state* state);

/**
 * @brief Compares two validus_state objects for equality.
 *
 * @param   one  Pointer to a validus_state to compare for equality.
 * @param   two  Pointer to a validus_state to compare for equality.
 * @returns bool `true` if both states are identical, `false` otherwise.
 */
bool validus_compare(const validus_state *one, const validus_state *two);

/**
 * @brief Processes a 192-octet block of data, accumulating the results
 * in the validus_state object.
 *
 * @attention This function is only called by other Validus functions; do not
 * call it directly.
 *
 * @param state Pointer to the validus_state object in use for this series of data.
 * @param blk32 Pointer to the block of data to be processed.
 */
void _validus_process(validus_state* state, const validus_word* blk32);

#if defined(__cplusplus)
}
#endif

////////////////////////////////// macros //////////////////////////////////////

/** The size of a Validus fingerprint, in bits. */
#define VALIDUS_FP_SIZE_B 192

/** The size of a Validus fingerprint, in octets. */
#define VALIDUS_FP_SIZE_O 48

/**
 * Boolean function to determine if address `a` is properly aligned on 32-bit
 * boundaries.
 */
#define WORDALIGNED(a) (((a - (const validus_word*)0) & 0x3) == 0)

/**
 * Swaps octet order of a 32-bit value `b` (represented as octets) and stores
 * the result in `a`.
 */
#define OCTETSWAP(a, b) (a = ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0])))

/** Cyclically rotates word `a` by `b` bits to the left. */
#define ROL(a, b) (((a) << (b)) | ((a) >> (32 - b)))

/** Cyclically rotates word `a` by `b` bits to the right. */
#define ROR(a, b) (((a) >> (b)) | ((a) << (32 - b)))

/** Mixer functions [M0 .. M3] */
#define M0(a, b, c, d, e) ((((a) & (b)) ^ ((c) & (d)  ^ (e))))
#define M1(a, b, c, d, e) ((((a) & (b)) ^ ((b) ^ (c)  & (d)  ^ (e))))
#define M2(a, b, c, d, e) (((a)  & ((b) ^ (c)) ^ ~(d) & (e)  ^ (c)))
#define M3(a, b, c, d, e) ((((a) & (b)) ^ (c)  & ((d) ^ (e)) ^ (e)))

/** Compression functions [VC_0 .. VC_3]*/
#define VC_0(a, b, c, d, e, f, r1, r2, blk, hcv) {  \
    register validus_word t;                        \
    t = a + M0(b, c, d, e, f) + ROL(blk + hcv, r1); \
    a = ROR(t + blk, r2);                           \
}

#define VC_1(a, b, c, d, e, f, r1, r2, blk, hcv) {  \
    register validus_word t;                        \
    t = a + M1(b, c, d, e, f) + ROL(blk + hcv, r1); \
    a = ROR(t + blk, r2);                           \
}

#define VC_2(a, b, c, d, e, f, r1, r2, blk, hcv) {  \
    register validus_word t;                        \
    t = a + M2(b, c, d, e, f) + ROL(blk + hcv, r1); \
    a = ROR(t + blk, r2);                           \
}

#define VC_3(a, b, c, d, e, f, r1, r2, blk, hcv) {  \
    register validus_word t;                        \
    t = a + M3(b, c, d, e, f) + ROL(blk + hcv, r1); \
    a = ROR(t + blk, r2);                           \
}

/** Initial state values. */
#define VALIDUS_INIT_0  0x81010881  /* 10000001000000010000100010000001 */
#define VALIDUS_INIT_1  0xA529298B  /* 10100101001010010010100110001011 */
#define VALIDUS_INIT_2  0x66AC654A  /* 01100110101011000110010101001010 */
#define VALIDUS_INIT_3  0x52865650  /* 01010010100001100101011001010000 */
#define VALIDUS_INIT_4  0x18529234  /* 00011000010100101001001000110100 */
#define VALIDUS_INIT_5  0x08508024  /* 00001000010100001000000000100100 */

/** Constants used in compression functions. */
#define VALIDUS_0   0x4528A03E
#define VALIDUS_1   0xCABBB352
#define VALIDUS_2   0x8147ED07
#define VALIDUS_3   0xAC5567E6
#define VALIDUS_4   0x244F0AE0
#define VALIDUS_5   0x7AE6DC24
#define VALIDUS_6   0x607FDE7A
#define VALIDUS_7   0xED604141
#define VALIDUS_8   0x8B601567
#define VALIDUS_9   0x94994DDD
#define VALIDUS_10  0x00910572
#define VALIDUS_11  0x6C036810
#define VALIDUS_12  0x74273753
#define VALIDUS_13  0x11809AEC
#define VALIDUS_14  0x9EC28BE7
#define VALIDUS_15  0x856B6A07
#define VALIDUS_16  0xDCC772A0
#define VALIDUS_17  0x54854204
#define VALIDUS_18  0x7E8DDF77
#define VALIDUS_19  0x343C0872
#define VALIDUS_20  0xB9E401F3
#define VALIDUS_21  0x8001489E
#define VALIDUS_22  0xBBCC20AB
#define VALIDUS_23  0x364B7F69
#define VALIDUS_24  0x7EDFBBD5
#define VALIDUS_25  0x52CE4099
#define VALIDUS_26  0xDA9B320E
#define VALIDUS_27  0x84C954BC
#define VALIDUS_28  0xA03FA6A0
#define VALIDUS_29  0x13BE82F2
#define VALIDUS_30  0x7516470E
#define VALIDUS_31  0x6AC7D4C0
#define VALIDUS_32  0xFE4CF293
#define VALIDUS_33  0x93620C9C
#define VALIDUS_34  0x8C53D058
#define VALIDUS_35  0xEF9EE2C6
#define VALIDUS_36  0x61F9166F
#define VALIDUS_37  0x7A3FDA68
#define VALIDUS_38  0x22215B08
#define VALIDUS_39  0xAAA1C60C
#define VALIDUS_40  0x819EDDD8
#define VALIDUS_41  0xCCCD47FE
#define VALIDUS_42  0x470DF3E1
#define VALIDUS_43  0x7FFADD92
#define VALIDUS_44  0x433DC03C
#define VALIDUS_45  0xC8AE64A3
#define VALIDUS_46  0x80FB26C0
#define VALIDUS_47  0xAE0FC014
#define VALIDUS_48  0x2679D104
#define VALIDUS_49  0x7B8401BE
#define VALIDUS_50  0x5EFEE8A1
#define VALIDUS_51  0xEB231E3C
#define VALIDUS_52  0x8A75B5AE
#define VALIDUS_53  0x95D92CE8
#define VALIDUS_54  0x02D50CAF
#define VALIDUS_55  0x6D365104
#define VALIDUS_56  0x732ED60F
#define VALIDUS_57  0x0F414B6F
#define VALIDUS_58  0x9D4D3E47
#define VALIDUS_59  0x861754DC
#define VALIDUS_60  0xDEF6868E
#define VALIDUS_61  0x56357B91
#define VALIDUS_62  0x7E31DBF2
#define VALIDUS_63  0x322860B2
#define VALIDUS_64  0xB8018321
#define VALIDUS_65  0x800B8D5C
#define VALIDUS_66  0xBDB9B81F
#define VALIDUS_67  0x38569B47
#define VALIDUS_68  0x7F276A7B
#define VALIDUS_69  0x51109A87
#define VALIDUS_70  0xD871F179
#define VALIDUS_71  0x843121FC
#define VALIDUS_72  0xA1C46FE1
#define VALIDUS_73  0x15FAD577
#define VALIDUS_74  0x75FBF212
#define VALIDUS_75  0x6983B065
#define VALIDUS_76  0xFC09029A
#define VALIDUS_77  0x9233821D
#define VALIDUS_78  0x8D50D2F4
#define VALIDUS_79  0xF1DED4B0
#define VALIDUS_80  0x636A723D
#define VALIDUS_81  0x798F09EE
#define VALIDUS_82  0x1FF0EE3A
#define VALIDUS_83  0xA8F4FD78
#define VALIDUS_84  0x81FFF238
#define VALIDUS_85  0xCEE2F829
#define VALIDUS_86  0x48ED9435
#define VALIDUS_87  0x7FEB76B7
#define VALIDUS_88  0x414D7B3D
#define VALIDUS_89  0xC6A58615
#define VALIDUS_90  0x80B8912B
#define VALIDUS_91  0xAFD0AB19
#define VALIDUS_92  0x28A180F1
#define VALIDUS_93  0x7C173E98
#define VALIDUS_94  0x5D7653C5
#define VALIDUS_95  0xE8E7A7B1
#define VALIDUS_96  0x8994C3FB
#define VALIDUS_97  0x97219014
#define VALIDUS_98  0x0518D9C1
#define VALIDUS_99  0x6E6076FC
#define VALIDUS_100 0x722D372E
#define VALIDUS_101 0x0D00C2A5
#define VALIDUS_102 0x9BDFDBB4
#define VALIDUS_103 0x86CD0770
#define VALIDUS_104 0xE12840FF
#define VALIDUS_105 0x57DECA91
#define VALIDUS_106 0x7DCBB8A9
#define VALIDUS_107 0x3010B2D0
#define VALIDUS_108 0xB624CAE9
#define VALIDUS_109 0x80201605
#define VALIDUS_110 0xBFACA0B5
#define VALIDUS_111 0x3A5D3213
#define VALIDUS_112 0x7F64E5A9
#define VALIDUS_113 0x4F4C7390
#define VALIDUS_114 0xD64BDD44
#define VALIDUS_115 0x83A2DDFC
#define VALIDUS_116 0xA350C878
#define VALIDUS_117 0x18356491
#define VALIDUS_118 0x76D825F1
#define VALIDUS_119 0x683714FF
#define VALIDUS_120 0xF9C56410
#define VALIDUS_121 0x910DC6A7
#define VALIDUS_122 0x8E5708EE
#define VALIDUS_123 0xF41FE8CA
#define VALIDUS_124 0x64D3D441
#define VALIDUS_125 0x78D478E5
#define VALIDUS_126 0x1DBDF16B
#define VALIDUS_127 0xA74F3092
#define VALIDUS_128 0x826B225F
#define VALIDUS_129 0xD0FC9903
#define VALIDUS_130 0x4AC75AC1
#define VALIDUS_131 0x7FD1CCAA
#define VALIDUS_132 0x3F57F910
#define VALIDUS_133 0xC4A14172
#define VALIDUS_134 0x808031A0
#define VALIDUS_135 0xB19804F1
#define VALIDUS_136 0x2AC5EE66
#define VALIDUS_137 0x7CA086E3
#define VALIDUS_138 0x5BE63F66
#define VALIDUS_139 0xE6AE0B79
#define VALIDUS_140 0x88BD5259
#define VALIDUS_141 0x98725D07
#define VALIDUS_142 0x075C3E24
#define VALIDUS_143 0x6F81C20D
#define VALIDUS_144 0x71226F5D
#define VALIDUS_145 0x0ABF2ECD
#define VALIDUS_146 0x9A7A817E
#define VALIDUS_147 0x878C732E
#define VALIDUS_148 0xE35C74E1
#define VALIDUS_149 0x59810CE8
#define VALIDUS_150 0x7D5B7DCC
#define VALIDUS_151 0x2DF529C6
#define VALIDUS_152 0xB44DFF8A
#define VALIDUS_153 0x803EE0F3
#define VALIDUS_154 0xC1A4B268
#define VALIDUS_155 0x3C5F1A33
#define VALIDUS_156 0x7F982871
#define VALIDUS_157 0x4D81EFF9
#define VALIDUS_158 0xD429218F
#define VALIDUS_159 0x831E9426
#define VALIDUS_160 0xA4E49098
#define VALIDUS_161 0x1A6E027A
#define VALIDUS_162 0x77AAD103
#define VALIDUS_163 0x66E21D3F
#define VALIDUS_164 0xF7824573
#define VALIDUS_165 0x8FF0F1C9
#define VALIDUS_166 0x8F665D3E
#define VALIDUS_167 0xF661F0CA
#define VALIDUS_168 0x66351F7D
#define VALIDUS_169 0x78103647
#define VALIDUS_170 0x1B8891C7
#define VALIDUS_171 0xA5B08130
#define VALIDUS_172 0x82E065B1
#define VALIDUS_173 0xD319FF6A
#define VALIDUS_174 0x4C9B2181
#define VALIDUS_175 0x7FADE179
#define VALIDUS_176 0x3D5D61F3
#define VALIDUS_177 0xC2A1C024
#define VALIDUS_178 0x80520CA3
#define VALIDUS_179 0xB365A915
#define VALIDUS_180 0x2CE6ED63
#define VALIDUS_181 0x7D1FCF9C
#define VALIDUS_182 0x5A4ECB9D
#define VALIDUS_183 0xE4767747
#define VALIDUS_184 0x87EF7212
#define VALIDUS_185 0x99CB78BD
#define VALIDUS_186 0x099F0B5D
#define VALIDUS_187 0x709A1B02
#define VALIDUS_188 0x700E9401
#define VALIDUS_189 0x087CBE3D
#define VALIDUS_190 0x991D4C51
#define VALIDUS_191 0x885588BD

#endif /* !_VALIDUS_H_INCLUDED */
