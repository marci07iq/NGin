/* --------------------------------------------------------------
    Signed integers with unlimited range (version 2.1c).
    Macro parameters for base functions.

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2017 by Raul N.Shakirov, IMach of RAS(UB).
    All Rights Reserved.

    Permission has been granted to copy, distribute and modify
    software in any context without fee, including a commercial
    application, provided that the aforesaid copyright statement
    is present here as well as exhaustive description of changes.

    THE SOFTWARE IS DISTRIBUTED "AS IS". NO WARRANTY OF ANY KIND
    IS EXPRESSED OR IMPLIED. YOU USE AT YOUR OWN RISK. THE AUTHOR
    WILL NOT BE LIABLE FOR DATA LOSS, DAMAGES, LOSS OF PROFITS OR
    ANY OTHER KIND OF LOSS WHILE USING OR MISUSING THIS SOFTWARE.
-------------------------------------------------------------- */
#ifndef _CBIGNUMF_H
#define _CBIGNUMF_H
#include "Cbnl.h"

//================================================
//      Determine if assembler package is in use.
//================================================
//      _CBIGNUM_ASM            use code from Cbignumf.cpp

#ifndef _CBIGNUMF_INL
#if 1
#define _CBIGNUM_ASM
#endif
#endif//_CBIGNUMF_INL

//================================================
//      Enabling of hardware optimization.
//================================================
//      _CBIGNUM_HARDWARE_CBR   unrolled add/subtract/shift with
//                              carry/borrow in assembler package
//      _CBIGNUM_HARDWARE_MUL   unrolled multiplication with
//                              accumulation in assembler package
//                              and multiplication to double word
//      _CBIGNUM_HARDWARE_DIV   dividing.

#ifdef  _CBIGNUM_ASM

#ifdef  _CBNL_CBR
#if 1
#define _CBIGNUM_HARDWARE_CBR
#endif
#endif//_CBNL_CBR

#endif//_CBIGNUM_ASM

#ifdef  _CBNL_MUL
#if 1
#define _CBIGNUM_HARDWARE_MUL
#endif
#endif//_CBNL_MUL

#if 0
#define _CBIGNUM_HARDWARE_DIV
#endif

//================================================
//      Enabling of automatic optimization.
//================================================
//      _CBIGNUM_KARATSUBA_MUL  use Karatsuba multiplication if
//                              both numbers have _CBNL_KARATSUBA_MIN
//                              or more words.
//
//      _CBIGNUM_BLOCK_MUL      use block multiplication if
//                              both numbers have _CBNL_TAB_HIGH
//                              or more words (block method is
//                              optimized for cache L1, now is
//                              superceded by more effective
//                              Karatsuba method, except for case
//                              when one number is shorter then
//                              _CBNL_KARATSUBA_MIN words).
//
//      _CBIGNUM_TERNARY_MUL    use 30% faster ternary method for
//                              multiplication with table of shifts
//                              instead of simpler binary method
//                              (now both methods are superceded by
//                              hardware multiplication, if in use).
//
//      _CBIGNUM_SHIFTTAB_MUL   build table of shifts to accelerate
//                              multiplication, if both numbers
//                              have _CBNL_TAB_MIN or more words
//                              (now is superceded by more effective
//                              hardware multiplication, if in use).
//
//      _CBIGNUM_SHIFTTAB_DIV   build table of shifts to accelerate
//                              division and module, if divider
//                              have at least _CBNL_TAB_MIN less
//                              words then dividend but no more
//                              then _CBNL_TAB_MAX words.
//
//      _CBIGNUM_SMALL_DIV      use special algorithms for small
//                              divider and module, currently
//                              single-word (1) and double-word (2)
//                              value.
//
//      _CBIGNUM_SMALL_POWMOD   use special algorithm for power by
//                              small, currently single-word value.
//
//      _CBIGNUM_REVERSE_MOD    calculate single-word module thereby
//                              reverse multiplication after hardware
//                              division. Turned off by default.
//
//      _CBIGNUM_REDUCE_JUMPS   use code with extra operations to
//                              reduce number of conditional jumps.
//                              Reserved fo possible future use.

#if 1
#define _CBIGNUM_KARATSUBA_MUL
#endif

#if 1
#define _CBIGNUM_BLOCK_MUL
#endif

#if 1
#define _CBIGNUM_TERNARY_MUL
#endif

#if 1
#define _CBIGNUM_SHIFTTAB_MUL
#endif

#if 1
#define _CBIGNUM_SHIFTTAB_DIV
#endif

#if 1
#define _CBIGNUM_SMALL_DIV 2
#endif

#if 1
#define _CBIGNUM_SMALL_POWMOD
#endif

#if 0
#define _CBIGNUM_REVERSE_MOD
#endif

#if 0
#define _CBIGNUM_REDUCE_JUMPS
#endif

//================================================
//      Tuning of automatic optimization.
//================================================
//      _CBNL_TAB_MIN           minimal size of multiplier when it is
//                              worth to build table of shifts for
//                              multiplicand or
//                              minimal difference between sizes of
//                              dividend and divider when it is worth
//                              to build table of shifts for divider.
//
//      _CBNL_TAB_OPT           optimal size of piece of multiplicand
//                              for block multiplication with shift,
//                              when Karatsuba method is not applicable;
//                              it is better to take 8n - 3.
//
//      _CBNL_TAB_HIGH          maximal size of piece of multiplicand
//                              for block multiplication with shift,
//                              when Karatsuba method is not applicable.
//
//      _CBNL_TAB_MAX           maximal size of divider, for which
//                              its table of shifts may be build.
//
//      _CBNL_KARATSUBA_MIN     minimal size of operands to implement
//                              Karatsuba multiplication.
//
//      _CBNL_MUL_OPT           optimal size of piece of multiplicand
//                              for block hardware multiplication,
//                              when Karatsuba method is not applicable;
//                              it is better to take 8n - 3.
//
//      _CBNL_MUL_HIGH          maximal size of piece of multiplicand
//                              for block hardware multiplication,
//                              when Karatsuba method is not applicable.
//
//      Here size is number of CBNL words in the code of number.
//
//      _CBNL_HARDDIV_BITS      minimal difference in number of bits in
//                              dividend and divider for which hardware
//                              division is more effective then binary.
//                              Affects division of multiple-word by
//                              single-word divider and power by
//                              single-word module if double-to-single
//                              word division is not accelerated
//                              by assembler package.

#define _CBNL_TAB_MIN   (3)
#define _CBNL_TAB_OPT   (((sizeof(size_t)<=2)? 120: 7680)/\
                         (sizeof (CBNL) * sizeof (CBNL)) - 3)
#define _CBNL_TAB_HIGH  (_CBNL_TAB_OPT + _CBNL_TAB_OPT / 2)
#define _CBNL_TAB_MAX   ((EXSIZE_T_MAX & (size_t)0x7FFFFFFFUL)/\
                         (1024 * sizeof (CBNL)))

#ifndef _CBIGNUM_HARDWARE_MUL
#define _CBNL_KARATSUBA_MIN     (100)
#else
#define _CBNL_KARATSUBA_MIN     (50)
#define _CBNL_MUL_OPT   (7680 / sizeof (CBNL) - 3)
#define _CBNL_MUL_HIGH  (_CBNL_MUL_OPT + _CBNL_MUL_OPT / 2)
#endif//_CBIGNUM_HARDWARE_MUL

#define _CBNL_HARDDIV_BITS 10

#endif//_CBIGNUMF_H
