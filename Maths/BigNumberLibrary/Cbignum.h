/* --------------------------------------------------------------
    Signed integers with unlimited range (version 2.1c).

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
#ifndef _CBIGNUM_H
#define _CBIGNUM_H

#ifndef __cplusplus
#error  Must use C++ for the type cBigNumber.
#endif//__cplusplus

#ifndef _CBNL_H
#include "Cbnl.h"
#endif//_CBNL_H

#ifndef _CTTY_H
#include "Ctty.h"
#endif//_CTTY_H

#ifndef EXARRAY_H
#include "Exarray.h"
#endif//EXARRAY_H

#ifndef EXDEBUG_H
#include "Exdebug.h"
#endif//EXDEBUG_H

//================================================
//      Support for multithreading and DLL.
//      _CBIGNUM_MT         exclude non-reenterable methods.
//      _CBIGNUM_TOATMP     enable method toatmp().
//      _CBIGNUM_CONSTCAST  enable const cast.
//      EXTHREAD_LOCAL      prefix for local thread storage.
//================================================

#if 1
#define _CBIGNUM_MT
#endif

#if 0
#define _CBIGNUM_TOATMP
#endif

#if 0
#define _CBIGNUM_CONSTCAST
#endif

#ifndef _CBIGNUM_MT
#define _CBIGNUM_TOATMP
#define _CBIGNUM_CONSTCAST
#endif//_CBIGNUM_MT

#ifndef EXTHREAD_LOCAL
#ifndef _CBIGNUM_MT
#define EXTHREAD_LOCAL
#else //_CBIGNUM_MT
#include "Exthread.h"
#endif//_CBIGNUM_MT
#endif//EXTHREAD_LOCAL

//================================================
//      Allocation and index check options.
//      _CBIGNUM_DEF_ALLOC  allocate memory in default constructor
//                          as in versions 1.x of class.
//      _CBIGNUM_NCHECKPTR  do not check indexes for input arrays.
//================================================

#if 0
#define _CBIGNUM_DEF_ALLOC
#endif

#if 0
#define _CBIGNUM_NCHECKPTR
#endif

#ifdef  _CBIGNUM_NCHECKPTR

#define CBPTR(T)        T*
#define CBPTRTYPE(m)    ((m).base())
#define CBPTRBASE(p)    (p)
#define CBPTRINDEX(p,i) (p)

#else //_CBIGNUM_NCHECKPTR

#define CBPTR(T)        EXPTR(T)
#define CBPTRTYPE(m)    EXPTRTYPE(m)
#define CBPTRBASE(p)    EXPTRBASE(p)
#define CBPTRINDEX(p,i) EXPTRINDEX(p,i)

#endif//_CBIGNUM_NCHECKPTR

//================================================
//      Internal representation of cBigNumber.
//================================================
//      Unlimited number is stored in array of type CBNL.
//      Word 0 contains number of CBNL words, required for
//      storing of complementary code of number, the next
//      words contain complementary code in order from low
//      words to high words. Sign resides in the last word.
//      Method length() tells number of words in the code.
//
//      Minimal number of CBNL words is used for storing,
//      but no less than one word:
//      - if length()>1, high word differs from sign extension
//        of word preceding the high;
//      - zero is represented by single 0-word.
//      We'll call this representation to be normalized.
//
//      Normalization is not mandatory. Methods allow for any
//      number of words in the complementary code, starting from 0.
//      Obtaining of normalized result is guaranteed if all
//      operands are normalized.
//
//      From managing of CBNL array we use class exblock<CBNL>,
//      which object contains reference to array with code of number
//      and also field with overall number of CBNL words, available
//      for recording if number. Number of available words may be
//      be greater than number of occupied words as this is necessary
//      for arithmetical operation, which may require for increasing
//      of size of number.
//
//      Resizing of array is carried out by means of the following
//      methods:
//      checkexpand(n) - increase number of available words and
//                       reserve some memory for further expansion.
//      checkindex(n)  - decrease number of available words but
//                       do not free reserved memory.
//      Here number of words does not include word with index 0.
//
//      Methods checkexpand() and checkindex() may allocate larger
//      amount of memory than it is necessary for indicated number
//      of words. Extra memory is used as a reserve for sucessive
//      growing of size of number.
//
//      If macro NCHECKPTR is defined, method checkexpand() increases
//      number of available words up to number of actually reserved
//      words, whereas method checkindex() does nothing.
//
//      If macro NCHECKPTR is not defined, then methods checkexpand()
//      and checkindex() sets number of available words to be exactly
//      same as indicated.
//
//      Hierarchy of classes:
//
//      exblock<CBNL>           // Array with variable length.
//      |
//      +- cBigNumber           // Unlimited number.
//         |
//         +- cBigTemp          // Unlimited number in the stack.
//            |
//            +- cBigInc        // Postfix increment.
//            +- cBigDec        // Postfix decrement.
//            +- cBigPos        // Unary +.
//            +- cBigNeg        // Unary -.
//            +- cBigAbs        // Absolute value.
//            +- cBigUnsign     // Unsigned value.
//            +- cBigCompl      // Bit-wise inversion.
//            +- cBigXor        // Bit-wise sum by module 2.
//            +- cBigAnd        // Bit-wise conjunction.
//            +- cBigOr         // Bit-wise disjunction.
//            +- cBigAdd        // Addition.
//            +- cBigSub        // Subtraction.
//            +- cBigMul        // Multiplication.
//            +- cBigDiv        // Division.
//            +- cBigDivMod     // Division with module.
//            +- cBigMod        // Module.
//            +- cBigShl        // Left shift.
//            +- cBigShr        // Right shift.
//            +- cBigPow        // Power.
//            +- cBigPowMod     // Power by module.
//            +- cBigSqrt       // Square root.
//            +- cBigSqrtRm     // Square root with remainder.
//            +- cBigBits       // Number of meaning bits.
//            +- cBigExBits     // Number of meaning low 0-bits.
//            +- cBigRandom     // Random value.

//================================================
//      Typedef for string conversion.
//================================================

typedef exarray<char> cBigString;

//================================================
//      Main class cBigNumber.
//================================================

class cBigNumber: public exblock<CBNL>
{
protected:
#ifndef _CBIGNUM_DEF_ALLOC
  static CBNL _stub [(EXALLOC_EXTRA_ITEMS(CBNL) > 2?
                      EXALLOC_EXTRA_ITEMS(CBNL) : 2)];
#endif//_CBIGNUM_DEF_ALLOC
/*
  void  create ()               // Test method for non-normalized zero.
  {
    e = (CBNL *) exmalloc (EXCALCBLOCKSIZE_1);
    len = max_size (EXCALCBLOCKSIZE_1);
  }
*/
  void  create (CBNL b = 0)     // Optimized method for creating of number
  {                             // with minimal size and given initial value.
#ifndef _CBIGNUM_DEF_ALLOC
    if (b == 0) { e = _stub; len = 0; } else
#endif//_CBIGNUM_DEF_ALLOC
    {
      e = (CBNL *) exmalloc (EXCALCBLOCKSIZE_1);
      len = max_size (EXCALCBLOCKSIZE_1);
      e [0] = 1; e [1] = b;
      checkindex (1);
    }
  }

  cBigNumber (exblockstack_t) {}// Empty constructor for cBigTemp.

public:

//      Constructors.
#ifndef _CBIGNUM_DEF_ALLOC
  cBigNumber ()                 { e = _stub; len = 0; } // Default (0).
#else //_CBIGNUM_DEF_ALLOC
  cBigNumber ()                 { create();   }         // Default (0).
#endif//_CBIGNUM_DEF_ALLOC
  cBigNumber (CBNL b)           { create (b);   }       // Number.
  cBigNumber (const cBigNumber&);                       // Copying.
  cBigNumber (const char* psz, unsigned radix);         // String.

//      Destructor.
#ifndef _CBIGNUM_DEF_ALLOC
  ~cBigNumber()                 { if (len) exfree (e);  }
#else //_CBIGNUM_DEF_ALLOC
  ~cBigNumber()                 { exfree (e);           }
#endif//_CBIGNUM_DEF_ALLOC

//      Internal representation of number.
  size_t      length  () const  { return (size_t)(*e);} // Number of words.
  const CBNL* code    () const  { return e + 1;       } // Array of words.
  const CBNL& loword  () const  { return e [*e != 0]; } // Low word.
  const CBNL& hiword  () const  { return e [length()];} // High word.
  size_t      isfit   () const;                         // Normalized?
  size_t      words   () const;                         // Count meaning words.
  size_t      exwords () const;                         // Count low 0-words.

//      Information on number.
  CBNL        bits    () const;                         // Count meaning bits.
  cBigNumber& setbits   (const cBigNumber&);
  cBigNumber& setbits   (CBNL);
  CBNL        exbits  () const;                         // Count low 0-bits.
  cBigNumber& setexbits (const cBigNumber&);
  cBigNumber& setexbits (CBNL);

//      Service conversions.
//      Number does not change but its internal representation may change.
  cBigNumber& fit   ();                                 // Normalize.
  cBigNumber& tab   ();                                 // Table of shifts.
  cBigNumber& smp   ();                                 // Data for mulsmp().
  cBigNumber& gc    ();                                 // Optimize memory.
  cBigNumber& pack  ();                                 // Truncate memory.
#ifdef  _CBIGNUM_CONSTCAST
const cBigNumber& fit   () const { return ((cBigNumber*)this)->fit();   }
const cBigNumber& tab   () const { return ((cBigNumber*)this)->tab();   }
const cBigNumber& smp   () const { return ((cBigNumber*)this)->smp();   }
const cBigNumber& gc    () const { return ((cBigNumber*)this)->gc();    }
const cBigNumber& pack  () const { return ((cBigNumber*)this)->pack();  }
#endif//_CBIGNUM_CONSTCAST

//      Clearing with optimizing of memory.
  void        clear ();                                 // Set 0, optimize.

//      Fast inline test for normalized or zero length 0:
//          0 if number is 0 with code of length() <= 1
//      not 0 if number is 0 with code of length() >= 2 or is not 0
  CBNL       _testnot0() const  { return (hiword() | (e [0] >> 1)); }

//      Comparison operations. Returns -1 = less, 0 = equal, 1 = greater.
  int         comp  (const cBigNumber&) const;          // Compare to
  int         comp  (CBNL)              const;          // given number.
  int         comp0 ()                  const;          // Compare to 0.

//      Unary operations with accumulation.
  cBigNumber& neg   ()  { return setneg  (*this); }     // Inversion of sign.
  cBigNumber& abs   ();                                 // Absolute value.
  cBigNumber& unsign();                                 // Unsigned value.
//cBigNumber& compl ()  { return setcompl(*this); }     // Bit-wise inversion.
  cBigNumber& operator ++();                            // Prefix increment.
  cBigNumber& operator --();                            // Prefix decrement.
  cBigNumber& mul2  ();                                 // Multiplication to 2.
  cBigNumber& div2  ();                                 // Division to 2.
  cBigNumber& pow2  ();                                 // Square.
  cBigNumber& sqrt  ();                                 // Square root.

//      Assign either copy of number or result of operation.
//      Operands are passed as parameters.
  cBigNumber& set         (const cBigNumber&);          // Copying.
  cBigNumber& set         (CBNL);
  cBigNumber& setneg      (const cBigNumber&);          // Inversion of sign.
  cBigNumber& setneg      (CBNL);
  cBigNumber& setabs      (const cBigNumber&);          // Absolute value.
  cBigNumber& setabs      (CBNL);
  cBigNumber& setunsign   (const cBigNumber&);          // Unsigned value.
  cBigNumber& setunsign   (CBNL);
  cBigNumber& setcompl    (const cBigNumber&);          // Bit-wise inversion.
  cBigNumber& setcompl    (CBNL);
  cBigNumber& setxor (const cBigNumber&, const cBigNumber&);    // Bit-wise
  cBigNumber& setxor (const cBigNumber&, CBNL);                 // sum by
  cBigNumber& setxor (CBNL, const cBigNumber&);                 // module 2.
  cBigNumber& setxor (CBNL, CBNL);
  cBigNumber& setand (const cBigNumber&, const cBigNumber&);    // Bit-wise
  cBigNumber& setand (const cBigNumber&, CBNL);                 // conjunction.
  cBigNumber& setand (CBNL, const cBigNumber&);
  cBigNumber& setand (CBNL, CBNL);
  cBigNumber& setor  (const cBigNumber&, const cBigNumber&);    // Bit-wise
  cBigNumber& setor  (const cBigNumber&, CBNL);                 // disjunction.
  cBigNumber& setor  (CBNL, const cBigNumber&);
  cBigNumber& setor  (CBNL, CBNL);
  cBigNumber& setadd (const cBigNumber&, const cBigNumber&);    // Addition.
  cBigNumber& setadd (const cBigNumber&, CBNL);
  cBigNumber& setadd (CBNL, const cBigNumber&);
  cBigNumber& setadd (CBNL, CBNL);
  cBigNumber& setsub (const cBigNumber&, const cBigNumber&);    // Subtraction.
  cBigNumber& setsub (const cBigNumber&, CBNL);
  cBigNumber& setsub (CBNL, const cBigNumber&);
  cBigNumber& setsub (CBNL, CBNL);
  cBigNumber& setmul (const cBigNumber&, const cBigNumber&);    // Multipli-
  cBigNumber& setmul (const cBigNumber&, CBNL);                 // cation.
  cBigNumber& setmul (CBNL, const cBigNumber&);
  cBigNumber& setmul (CBNL, CBNL);
  cBigNumber& setdiv (const cBigNumber&, const cBigNumber&);    // Division.
  cBigNumber& setdiv (const cBigNumber&, CBNL);
  cBigNumber& setdiv (CBNL, const cBigNumber&);
  cBigNumber& setdiv (CBNL, CBNL);
  cBigNumber& setmod (const cBigNumber&, const cBigNumber&);    // Module.
  cBigNumber& setmod (const cBigNumber&, CBNL);
  cBigNumber& setmod (CBNL, const cBigNumber&);
  cBigNumber& setmod (CBNL, CBNL);
  cBigNumber& setshl (const cBigNumber&, const cBigNumber&);    // Left
  cBigNumber& setshl (const cBigNumber&, CBNL);                 // shift.
  cBigNumber& setshl (CBNL, const cBigNumber&);
  cBigNumber& setshl (CBNL, CBNL);
  cBigNumber& setshr (const cBigNumber&, const cBigNumber&);    // Right
  cBigNumber& setshr (const cBigNumber&, CBNL);                 // shift.
  cBigNumber& setshr (CBNL, const cBigNumber&);
  cBigNumber& setshr (CBNL, CBNL);
  cBigNumber& setpow (const cBigNumber&, const cBigNumber&);    // Power.
  cBigNumber& setpow (const cBigNumber&, CBNL);
  cBigNumber& setpow (CBNL, const cBigNumber&);
  cBigNumber& setpow (CBNL, CBNL);
  cBigNumber& setpowmod (const cBigNumber&, const cBigNumber&,  // Power by
                         const cBigNumber&);                    // module.
  cBigNumber& setpowmod (const cBigNumber&, const cBigNumber&, CBNL);
  cBigNumber& setpowmod (const cBigNumber&, CBNL, const cBigNumber&);
  cBigNumber& setpowmod (const cBigNumber&, CBNL, CBNL);
  cBigNumber& setpowmod (CBNL, const cBigNumber&, const cBigNumber&);
  cBigNumber& setpowmod (CBNL, const cBigNumber&, CBNL);
  cBigNumber& setpowmod (CBNL, CBNL, const cBigNumber&);
  cBigNumber& setpowmod (CBNL, CBNL, CBNL);
  cBigNumber& setsqrt   (const cBigNumber&);                    // Square root.
  cBigNumber& setsqrt   (CBNL);
  cBigNumber& setrandom (unsigned long (*p)(), unsigned CBNL);  // Random.

//      Assignment.
  cBigNumber& operator =  (const cBigNumber& b) { return set (b);       }
  cBigNumber& operator =  (CBNL b)              { return set (b);       }

//      Binary operations with accumulation.
//      Second operand are passed as parameter.
  cBigNumber& operator ^= (const cBigNumber&);  // Bit-wise sum by mod 2
  cBigNumber& operator ^= (CBNL);
  cBigNumber& operator &= (const cBigNumber&);  // Bit-wise conjunction.
  cBigNumber& operator &= (CBNL);
  cBigNumber& operator |= (const cBigNumber&);  // Bit-wise disjunction.
  cBigNumber& operator |= (CBNL);
  cBigNumber& operator += (const cBigNumber&);  // Addition.
  cBigNumber& operator += (CBNL);
  cBigNumber& operator -= (const cBigNumber&);  // Subtraction.
  cBigNumber& operator -= (CBNL);
  cBigNumber& operator *= (const cBigNumber&);  // Multiplication.
  cBigNumber& operator *= (CBNL);
  cBigNumber& operator /= (const cBigNumber&);  // Division.
  cBigNumber& operator /= (CBNL);
  cBigNumber& operator %= (const cBigNumber&);  // Module.
  cBigNumber& operator %= (CBNL);
  cBigNumber& operator<<= (const cBigNumber&);  // Left shift.
  cBigNumber& operator<<= (CBNL);
  cBigNumber& operator>>= (const cBigNumber&);  // Right shift.
  cBigNumber& operator>>= (CBNL);
  cBigNumber& pow         (const cBigNumber&);  // Power.
  cBigNumber& pow         (CBNL);

//      Ternary operations with accumulation.
//      Second and third operands are passed as parameters.
  cBigNumber& powmod      (const cBigNumber&, const cBigNumber&);
  cBigNumber& powmod      (const cBigNumber&, CBNL);
  cBigNumber& powmod      (CBNL, const cBigNumber&);
  cBigNumber& powmod      (CBNL, CBNL);

//      Copying combined with CBNL-words left or right shift.
  cBigNumber& set         (const cBigNumber&, size_t k);
  cBigNumber& set         (CBNL, size_t k);
  cBigNumber& setr        (const cBigNumber&, size_t k);

//      Accumulation combined with CBNL-words left shift.
  cBigNumber& add         (const cBigNumber&, size_t k = 0);
  cBigNumber& add         (CBNL, size_t k = 0);
  cBigNumber& sub         (const cBigNumber&, size_t k = 0);
  cBigNumber& sub         (CBNL, size_t k = 0);

//      Combined operation of multiplication with accumulation.
//      Operands are passed as parameters, result of multiplication
//      is accumulated.
  cBigNumber& addmul      (const cBigNumber&, const cBigNumber&);
  cBigNumber& addmul      (const cBigNumber&, CBNL);
  cBigNumber& addmul      (CBNL, const cBigNumber&);
  cBigNumber& addmul      (CBNL, CBNL);
  cBigNumber& submul      (const cBigNumber&, const cBigNumber&);
  cBigNumber& submul      (const cBigNumber&, CBNL);
  cBigNumber& submul      (CBNL, const cBigNumber&);
  cBigNumber& submul      (CBNL, CBNL);

//      Combined operation of division with module.
//      Module is assigned to dividend, which must not overlap with result.
  cBigNumber& setdivmod   (cBigNumber&, const cBigNumber&);
  cBigNumber& setdivmod   (cBigNumber&, CBNL);

//      Combined operation of square root with remainder.
//      Remainder is assigned to operand, which must not overlap with result.
  cBigNumber& setsqrtrm   (cBigNumber&);

//      Multiplication with accumulation of preliminary prepared operands.
//      Multiplicand must contain table of shifts, prepared by tab().
//      Multiplier must be non-negative.
//      Operands must not overlap with buffer of result.
  cBigNumber& addmultab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& addmultab   (const cBigNumber&, CBNL, size_t k = 0);
  cBigNumber& submultab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& submultab   (const cBigNumber&, CBNL, size_t k = 0);
//      Multiplicand must be prepared by either smp() or tab().
//      Multiplier must be non-negative.
//      Operands must not overlap with buffer of result.
  cBigNumber& addmulsmp   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& addmulsmp   (const cBigNumber&, CBNL, size_t k = 0);
  cBigNumber& submulsmp   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& submulsmp   (const cBigNumber&, CBNL, size_t k = 0);

//      Division and module of preliminary prepared operands.
//      Operands must have identical signs. Divider must contain table of
//      shifts, prepared by tab() and must not overlap with buffer of result.
  cBigNumber& divtab      (const cBigNumber&, size_t k = 0);
  cBigNumber& divtab      (CBNL, size_t k = 0);
  cBigNumber& modtab      (const cBigNumber&, size_t k = 0);
  cBigNumber& modtab      (CBNL, size_t k = 0);
  cBigNumber& setdivtab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& setdivtab   (CBNL, const cBigNumber&, size_t k = 0);
  cBigNumber& setmodtab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& setmodtab   (CBNL, const cBigNumber&, size_t k = 0);

//      Division with module of preliminary prepared operands.
//      Operands must have identical signs, must not overlap each other and
//      buffer of result. Divider must contain table of shifts.
  cBigNumber& setdivmodtab(cBigNumber&, const cBigNumber&, size_t k = 0);

//      Messages.
static  const char* pszConverting;
static  const char* pszFormatting;

//      String conversions.
#define MAX_RADIX 16
static  const char  numtochar [2] [MAX_RADIX + 1];      // Undocumented
static        char  chartonum [UCHAR_MAX + 1];          // internal arrays.
static  size_t  SqrTab  (unsigned radix, size_t len = 0, size_t max_np = 0);
  cBigNumber& setunsign (const EXPTR(char) ps,          // Conversion of array
                         unsigned radix,                // to unsigned number.
                         size_t   width);
  cBigNumber& set       (const char* psz,               // Conversion of string
                         unsigned radix = 10);          // to number.
#define cBigNumber_uppercase    0x00040000L
#define cBigNumber_showbase     0x00080000L
#define cBigNumber_showpos      0x00200000L
#define cBigNumber_unsign       0x80000000L
  char*       toa       (cBigString& buf_,
                         unsigned radix = 10,           // Preparing string in
                         size_t   width = 0,            // extendable buffer.
                         long     fill  = 0)    const;
#ifdef  _CBIGNUM_TOATMP
  char*       toatmp    (unsigned radix = 10,           // Preparing string in
                         size_t   width = 0,            // the static buffer
                         long     fill  = 0)    const;  // (deprecated).
#endif//_CBIGNUM_TOATMP
  void        dump      ()  const;                      // Dump array of words.
  void        info      ()  const;                      // Short info of words.

//      Conversion to integer with check of range.
  void        erange    ()  const;                      // Range error.
  CBNL        toCBNL    ()  const;
  long        tolong    ()  const;
  int         toint     ()  const;
  short       toshort   ()  const;

//      Conversions for logical C operations.
  operator const void * ()  const       { return (comp0()? e: 0);       }
  int        operator ! ()  const       { return (comp0() == 0);        }

//      Control of division.
static  void  maskdiv0  (int);          // Mask for division by 0.
static  int   testdiv0  ();             // Test for division by 0.

//      Access to static buffers (deprecated).
#ifndef _CBIGNUM_MT
static  cBigNumber& lastdivmod ();      // Module of last division.
static  cBigNumber& lastrootrm ();      // Remainder of last square root.
#endif//_CBIGNUM_MT
};

//================================================
//      Class service variables (do not use).
//================================================

extern  size_t      cBigNumberMaskDiv0;     // Mask for division by 0.
#ifndef _CBIGNUM_MT
extern  cBigNumber  cBigNumberLastDivMod;   // Module of last division.
extern  cBigNumber  cBigNumberLastRootRm;   // Module of last square root.
#endif//_CBIGNUM_MT

//================================================
//      Basic test for primality.
//================================================

//      Strong Probable Primality test for base b.
int     b_SPRP      (const cBigNumber& n, const cBigNumber& b);

//================================================
//      Integral tests for primality (prime.cpp).
//================================================

//      Strong Probable Primality test.
int     SPRP        (const cBigNumber&);
int     SPRP        (const cBigNumber&, cBigNumber&);
int FastSPRP        (const cBigNumber&);
int FastSPRP        (const cBigNumber&, cBigNumber&);
int LastSPRP        (const cBigNumber&);
int LastSPRP        (const cBigNumber&, cBigNumber&);

//      Factor Primality test.
int TestFactor      (const cBigNumber&);
int TestFactor      (const cBigNumber&, cBigNumber&);
int TestSmallFactor (const cBigNumber&);
int TestSmallFactor (const cBigNumber&, cBigNumber&);
int TestLargeFactor (const cBigNumber&);
int TestLargeFactor (const cBigNumber&, cBigNumber&);

//      Combined (Factor + Strong) Primality test.
int IsPrime         (const cBigNumber&);
int IsProvedPrime   (const cBigNumber&);
int IsMillerPrime   (const cBigNumber&);
int IsStrongPrime   (const cBigNumber&);

//      Old function, compatible with version 1.1a
#ifdef  _CBIGNUM_HASFACTOR
inline int HasFactor(const cBigNumber& n) { return TestFactor(n);   }
#endif//_CBIGNUM_HASFACTOR

//================================================
//      Compiler independent stream output.
//================================================

cTTY& operator << (cTTY& os, const cBigNumber& b);
void operator <<= (cTTY& os, const cBigNumber& b);

extern cTTY cBigNumberMessages;         // Handler for messages.
extern cTTY cBigNumberProgress;         // Handler for progress indicator.

//================================================
//      Class to convert number to hex string.
//================================================

class cHexDump
{
  char buffer [CHAR_BIT * sizeof (CBNL) * 3 / 8];
public:
  cHexDump (unsigned CBNL n);
  operator const char*() { return (buffer); }
        // const avoids troubles with Borland C++
};

//================================================
//      Auxiliary class for passing of CBNL
//      number to basic functions.
//================================================

class _cBigLong
{
  CBNL e [2];           // Work array.

public:                 // Assignment to work array.

  _cBigLong         (CBNL n = 0) { e [0] = 1; e [1] = n;        }

  _cBigLong& operator = (CBNL n) { e [1] = n; return *this;     }

//      Conventional pointer for addressing without check of bounds.

  CBNL*       base()            { return (e);                   }

#ifndef NCHECKPTR

//      Restricted pointer for addressing with check of bounds:
//      on passing of index outside of array bounds function
//      assigned to ::exalloc_status.range_handler is called.

  operator    exptr<CBNL>()     { return exptr<CBNL> (e, 2);    }

#endif//NCHECKPTR
};

//================================================
//      Base functions.
//================================================

#ifdef  NCHECKPTR
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
#endif//NCHECKPTR

void    cBigNumberDump    (const CBPTR(CBNL) p1);
void    cBigNumberInfo    (const CBPTR(CBNL) p1);
void    cBigNumberERange  (const CBPTR(CBNL) p1);
size_t  cBigNumberWords   (const CBPTR(CBNL) p1);
CBNL    cBigNumberBits    (const CBPTR(CBNL) p1);
size_t  cBigNumberExWords (const CBPTR(CBNL) p1);
CBNL    cBigNumberExBits  (const CBPTR(CBNL) p1);
size_t _CBNL_C  cBigNumberIsFit
                          (const CBPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberFit   (EXPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberFitTo (EXPTR(CBNL) p1, size_t n);
int    _CBNL_C  cBigNumberComp
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2);
int    _CBNL_C  cBigNumberCompHigh
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2);
void   _CBNL_C  cBigNumberCopy
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberCompl   (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberCopyShl (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void    cBigNumberCopyShr (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void   _CBNL_C  cBigNumberCopyShrToM  (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void   _CBNL_C  cBigNumberCopyShrUToM (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void   _CBNL_C  cBigNumberClearTo (CBPTR(CBNL) p1, size_t n);
void    cBigNumberCutOut  (const CBPTR(CBNL) p1, size_t k1, size_t n,
                                 EXPTR(CBNL) p);
size_t _CBNL_C _cBigNumberNeg
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberMNegF         (EXPTR(CBNL) p1);
size_t _cBigNumberMInc          (EXPTR(CBNL) p1);
size_t _cBigNumberMDec          (EXPTR(CBNL) p1);
size_t _cBigNumberMMul2         (EXPTR(CBNL) p1);
size_t _cBigNumberMDiv2         (EXPTR(CBNL) p1);
size_t _cBigNumberMul2    (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
size_t _cBigNumberDiv2    (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberTab           (EXPTR(CBNL) p1);
size_t _cBigNumberXor     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberAnd     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberOr      (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberAdd     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberSub     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberSubS    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberMAdd          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
size_t _cBigNumberMSub          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
size_t _CBNL_C  cBigNumberSkipLow0
                          (const CBPTR(CBNL) p1);
size_t _CBNL_C  cBigNumberCopySkipLow0
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void   _CBNL_C  cBigNumberMAddM (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void   _CBNL_C  cBigNumberMSubM (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
size_t _CBNL_C _cBigNumberMSubD (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
size_t _CBNL_C _cBigNumberMMul2M(EXPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberMDiv2D(EXPTR(CBNL) p1);
void _CBNL_C cBigNumberMAddMulM (EXPTR(CBNL) p1, const EXPTR(CBNL) p2,
                                 unsigned CBNL l2, size_t k2);
void _CBNL_C cBigNumberMSubMulM (EXPTR(CBNL) p1, const EXPTR(CBNL) p2,
                                 unsigned CBNL l2, size_t k2);
void    cBigNumberMShrM         (EXPTR(CBNL) p1);
void    cBigNumberMShlD         (EXPTR(CBNL) p1);
void    cBigNumberMAddShl       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMSubShl       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMAddMulShl    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShl    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMulShlTab
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShlTab
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMulShlKar (EXPTR(CBNL) p1, EXPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShlKar (EXPTR(CBNL) p1, EXPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMul (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMSubMul (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMul     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberDiv0();
void    cBigNumberMModDivShlTab (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2, EXPTR(CBNL) p);
void    cBigNumberMModDiv       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMModShlTab    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMMod          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
void    cBigNumberPow           (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberPowMod        (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                                 const CBPTR(CBNL) mod,
                                 EXPTR(CBNL) p);
void    cBigNumberMRmSqrt       (EXPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberRandom  (unsigned long (*pfnRand)(), unsigned CBNL lBits,
                                 EXPTR(CBNL) p);

#ifdef  NCHECKPTR
#ifdef  __cplusplus
}
#endif//__cplusplus
#endif//NCHECKPTR

//================================================
//      Basic inline functions.
//================================================

inline  size_t  cBigNumberNeg     (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberNeg (p1, p));
}

inline  size_t  cBigNumberMInc          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMInc (p1));
}

inline  size_t  cBigNumberMDec          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDec (p1));
}

inline  size_t  cBigNumberMMul2         (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMMul2 (p1));
}

inline  size_t  cBigNumberMDiv2         (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDiv2 (p1));
}

inline  size_t  cBigNumberMul2   (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberMul2 (p1, p));
}

inline  size_t  cBigNumberDiv2   (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberDiv2 (p1, p));
}

inline  size_t  cBigNumberFit          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberFit (p1));
}

inline  size_t  cBigNumberFitTo        (EXPTR(CBNL) p1, size_t n)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberFitTo (p1, n));
}

inline  size_t  cBigNumberXor    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberXor (p1, p2, p));
}

inline  size_t  cBigNumberAnd    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberAnd (p1, p2, p));
}

inline  size_t  cBigNumberOr     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberOr  (p1, p2, p));
}

inline  size_t  cBigNumberAdd    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberAdd (p1, p2, p));
}

inline  size_t  cBigNumberSub    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberSub (p1, p2, p));
}

inline  size_t  cBigNumberSubS   (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberSubS (p1, p2, p));
}

inline  size_t  cBigNumberMAdd         (EXPTR(CBNL) p1, const CBPTR(CBNL) p2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMAdd (p1, p2));
}

inline  size_t  cBigNumberMSub         (EXPTR(CBNL) p1, const CBPTR(CBNL) p2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMSub (p1, p2));
}

inline  size_t  cBigNumberMSubD        (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        size_t k2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMSubD (p1, p2, k2));
}

inline  size_t  cBigNumberMMul2M        (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMMul2M (p1));
}

inline  size_t  cBigNumberMDiv2D        (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDiv2D (p1));
}

//================================================
//      Implementation of constructors.
//================================================

inline cBigNumber::cBigNumber (const cBigNumber& b)
{
  len = 0;
#ifndef _CBIGNUM_DEF_ALLOC
  if (b._testnot0() != 0)
#endif//_CBIGNUM_DEF_ALLOC
  {
    checkexpand (b.length());                           // Set size.
    cBigNumberCopy (CBPTRTYPE(b), EXPTRTYPE(*this));    // Copying.
    checkindex (length());                              // Debug check.
  }
#ifndef _CBIGNUM_DEF_ALLOC
  else e = _stub;                                       // Initialization by 0.
#endif//_CBIGNUM_DEF_ALLOC
}

inline cBigNumber::cBigNumber (const char* psz, unsigned radix)
{
#ifndef _CBIGNUM_DEF_ALLOC
  e = _stub; len = 0;                                   // Initialization by 0.
#else //_CBIGNUM_DEF_ALLOC
  create();                                             // Create number.
#endif//_CBIGNUM_DEF_ALLOC
  set (psz, radix);                                     // Parse constant.
}

//================================================
//      Implementation of information methods.
//================================================

inline void   cBigNumber::dump()    const
{
  cBigNumberDump (CBPTRTYPE(*this));
}

inline void   cBigNumber::info()    const
{
  cBigNumberInfo (CBPTRTYPE(*this));
}

inline void   cBigNumber::erange()  const
{
  cBigNumberERange (CBPTRTYPE(*this));
}

inline size_t cBigNumber::isfit()   const
{
  return cBigNumberIsFit (CBPTRTYPE(*this));
}

inline size_t cBigNumber::words()   const
{
  return cBigNumberWords (CBPTRTYPE(*this));
}

inline size_t cBigNumber::exwords() const
{
  return cBigNumberExWords (CBPTRTYPE(*this));
}

inline CBNL   cBigNumber::bits()    const
{
  return cBigNumberBits (CBPTRTYPE(*this));
}

inline CBNL   cBigNumber::exbits()  const
{
  return cBigNumberExBits (CBPTRTYPE(*this));
}

//================================================
//      Implementation of comparison operations.
//================================================

inline int  cBigNumber::comp (const cBigNumber& b)  const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(b));
}

inline int  cBigNumber::comp (CBNL b)               const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)));
}

inline int  cBigNumber::comp0 ()                    const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(_cBigLong(0)));
}

//================================================
//      Implementation of unary accumulations.
//================================================

inline cBigNumber& cBigNumber::abs()
{
  if (hiword() < 0L) neg();
  return *this;
}

inline cBigNumber& cBigNumber::unsign()
{
  if (hiword() < 0L)
  {
    size_t l = length() + 1;
    checkexpand (l); e [0] = (CBNL)l; e [l] = 0;
  }
  return *this;
}

//================================================
//      Implementation of assign operations.
//================================================

inline cBigNumber& cBigNumber::set (const cBigNumber& b)
{
#ifndef _CBIGNUM_DEF_ALLOC
  if (((e && e [0]) | b._testnot0()) != 0)
#endif//_CBIGNUM_DEF_ALLOC
  {
    checkexpand (b.length());
    cBigNumberCopy (CBPTRTYPE(b), EXPTRTYPE(*this));
    checkindex (length());
  }
  return *this;
}

inline cBigNumber& cBigNumber::set (CBNL b)
{
#ifndef _CBIGNUM_DEF_ALLOC
  if ((e [0] | b) != 0)
#endif//_CBIGNUM_DEF_ALLOC
  {
    checkexpand (1); e [0] = 1; e [1] = b;
    checkindex (1);
  }
  return *this;
}

inline cBigNumber& cBigNumber::setneg (const cBigNumber& b)
{
  checkexpand (b.length() + 1);
  cBigNumberNeg (CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setneg (CBNL b)
{
  checkexpand (2); e [0] = 1 + (b == LONG_MIN); e [1] = -b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setabs (const cBigNumber& b)
{
  return (b.hiword()>=0? set (b): setneg (b));
}

inline cBigNumber& cBigNumber::setabs (CBNL b)
{
  if (b < 0) b = -b;
  checkexpand (2); e [0] = 1 + (b == LONG_MIN); e [1] = b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setunsign (const cBigNumber& b)
{
  set (b); return unsign();
}

inline cBigNumber& cBigNumber::setunsign (CBNL b)
{
  checkexpand (2); e [0] = 1 + (b < 0); e [1] = b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setcompl (const cBigNumber& b)
{
  checkexpand (b.length() + (b.length() == 0));
  cBigNumberCompl (CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setcompl (CBNL b)
{
  checkexpand (1); e [0] = 1; e [1] = ~b;
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of binary accumulations.
//================================================

inline cBigNumber& cBigNumber::operator ^= (const cBigNumber& b)
{
  return setxor (*this, b);
}

inline cBigNumber& cBigNumber::operator ^= (CBNL b)
{
  return setxor (*this, b);
}

inline cBigNumber& cBigNumber::operator &= (const cBigNumber& b)
{
  return setand (*this, b);
}

inline cBigNumber& cBigNumber::operator &= (CBNL b)
{
  return setand (*this, b);
}

inline cBigNumber& cBigNumber::operator |= (const cBigNumber& b)
{
  return setor (*this, b);
}

inline cBigNumber& cBigNumber::operator |= (CBNL b)
{
  return setor (*this, b);
}

inline cBigNumber& cBigNumber::operator += (const cBigNumber& b)
{
  return add (b);
}

inline cBigNumber& cBigNumber::operator += (CBNL b)
{
  return add (b);
}

inline cBigNumber& cBigNumber::operator -= (const cBigNumber& b)
{
  return sub (b);
}

inline cBigNumber& cBigNumber::operator -= (CBNL b)
{
  return sub (b);
}

inline cBigNumber& cBigNumber::operator *= (const cBigNumber& b)
{
  return setmul (*this, b);
}

inline cBigNumber& cBigNumber::operator *= (CBNL b)
{
  return setmul (*this, b);
}

inline cBigNumber& cBigNumber::operator /= (const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmod (cBigNumberLastDivMod, b);
}

inline cBigNumber& cBigNumber::operator /= (CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmod (cBigNumberLastDivMod, b);
}

inline cBigNumber& cBigNumber::operator %= (const cBigNumber& b)
{
  fit();
  cBigNumberMMod (EXPTRTYPE(*this), CBPTRTYPE(b));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::operator %= (CBNL b)
{
  fit();
  cBigNumberMMod (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)));
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of copying with shift.
//================================================

inline cBigNumber& cBigNumber::set (const cBigNumber& b, size_t k)
{
  checkexpand (b.length() + (b.length() == 0) + k);
  cBigNumberCopyShl (CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::set (CBNL b, size_t k)
{
  checkexpand (k + 1);
  cBigNumberCopyShl (CBPTRTYPE(_cBigLong(b)), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setr (const cBigNumber& b, size_t k)
{
  checkexpand (b.length() > k? b.length() - k: 1);
  cBigNumberCopyShr (CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of combined operations.
//================================================

inline cBigNumber& cBigNumber::setdivmod (cBigNumber& a, const cBigNumber& b)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDiv (EXPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setdivmod (cBigNumber& a, CBNL b)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDiv (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of operations on
//      preliminary prepared operands.
//================================================

inline cBigNumber& cBigNumber::divtab (const cBigNumber& b, size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmodtab (cBigNumberLastDivMod, b, k);
}

inline cBigNumber& cBigNumber::divtab (CBNL b, size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmodtab (cBigNumberLastDivMod, b, k);
}

inline cBigNumber& cBigNumber::modtab (const cBigNumber& b, size_t k)
{
  fit();
  cBigNumberMModShlTab (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::modtab (CBNL b, size_t k)
{
  fit();
  cBigNumberMModShlTab (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)), k);
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setdivmodtab (cBigNumber& a,
                                       const cBigNumber& b, size_t k)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDivShlTab (EXPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of check of range.
//================================================

inline  CBNL    cBigNumber::toCBNL()  const
{
  if (bits() >= CHAR_BIT * sizeof (CBNL)) erange();
  return (loword());
}

inline  long    cBigNumber::tolong()  const
{
  CBNL n = toCBNL();
  if ((long) n != n)  erange();
  return ((long)n);
}

inline  int     cBigNumber::toint()   const
{
  CBNL n = toCBNL();
  if ((int) n != n)   erange();
  return ((int)n);
}

inline  short   cBigNumber::toshort() const
{
  CBNL n = toCBNL();
  if ((short) n != n) erange();
  return ((short)n);
}

//================================================
//      Implementation of control methods.
//================================================

inline void cBigNumber::maskdiv0 (int mask) { cBigNumberMaskDiv0 = mask << 1; }
inline int  cBigNumber::testdiv0()          { return cBigNumberMaskDiv0 == 1; }

#ifndef _CBIGNUM_MT
inline cBigNumber& cBigNumber::lastdivmod() { return cBigNumberLastDivMod; }
inline cBigNumber& cBigNumber::lastrootrm() { return cBigNumberLastRootRm; }
#endif//_CBIGNUM_MT

//================================================
//      Implementation of compare operations.
//================================================

inline  int  operator == (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) == 0);
}

inline  int  operator == (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) == 0);
}

inline  int  operator == (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) == 0);
}

inline  int  operator != (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) != 0);
}

inline  int  operator != (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) != 0);
}

inline  int  operator != (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) != 0);
}

inline  int  operator >= (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >= (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >= (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >  (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) > 0);
}

inline  int  operator >  (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) > 0);
}

inline  int  operator >  (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) > 0);
}

//================================================
//      Stack of temporary unlimited numbers
//      in thread local storage and fictive
//      array to generate template.
//================================================

#ifdef  EXTHREAD_LOCAL
extern exblockstack_t EXTHREAD_LOCAL cBigTemp_stack;
extern exblockstack<long>            cBigTemp_sfict;
#else //EXTHREAD_LOCAL
extern exblockstack_t                cBigTemp_stack;
#endif//EXTHREAD_LOCAL

//================================================
//      Inherited classes for implementation
//      of operations which require for creation
//      of temporary objects.
//================================================

class cBigTemp: public cBigNumber               // Number in temporary object.
{
//      Class cBigTemp describes temporary objects
//      for results of arithmetical operations.
//
//      For optimization of memory allocation constructor cBigTemp
//      uses stack of dynamic arrays, which allows to avoid redundant
//      allocations of memory: when object cBigTemp is deleted, pointer
//      to its base array is put to stack; when object cBigTemp is
//      created it receives base array from stack. Memory allocation
//      is required only when the stack if empty.
//
//      Stack can give multiple performance boost on small objects,
//      but it is used only if macro EXTHREAD_LOCAL_TL is set.
//      1) For single-threaded applications the macro may be empty.
//      2) For multi-threaded applications the macro must contain prefix
//         for thread local storage, which is dependant on compiler.

public:

#ifdef  EXTHREAD_LOCAL

  cBigTemp (): cBigNumber (cBigTemp_stack)      // Constructor.
              { (*(exblockstack<CBNL>*)&cBigTemp_stack).get (*this); }
  ~cBigTemp() { (*(exblockstack<CBNL>*)&cBigTemp_stack).put (*this); }
                                                // Destructor.
#else //EXTHREAD_LOCAL

  cBigTemp (): cBigNumber (cBigTemp_stack)
               { e = NULL; len = 0; }           // Constructor.
  ~cBigTemp()  {}                               // Destructor.

#endif//EXTHREAD_LOCAL

//      Blocking of assignments to objects of class cBigNumber to prevent
//      incorrect assignments to temporary objects, such as (-x) = 10;

private:

  cBigNumber& operator ++ ();                   // Prefix increment.
  cBigNumber& operator -- ();                   // Prefix decrement.
  cBigNumber& operator =  (const cBigNumber&);  // Assignment.
  cBigNumber& operator =  (CBNL);
  cBigNumber& operator ^= (const cBigNumber&);  // Bit-wise sum by mod 2.
  cBigNumber& operator ^= (CBNL);
  cBigNumber& operator &= (const cBigNumber&);  // Bit-wise conjunction.
  cBigNumber& operator &= (CBNL);
  cBigNumber& operator |= (const cBigNumber&);  // Bit-wise disjunction.
  cBigNumber& operator |= (CBNL);
  cBigNumber& operator += (const cBigNumber&);  // Addition.
  cBigNumber& operator += (CBNL);
  cBigNumber& operator -= (const cBigNumber&);  // Subtraction.
  cBigNumber& operator -= (CBNL);
  cBigNumber& operator *= (const cBigNumber&);  // Multiplication.
  cBigNumber& operator *= (CBNL);
  cBigNumber& operator /= (const cBigNumber&);  // Division.
  cBigNumber& operator /= (CBNL);
  cBigNumber& operator %= (const cBigNumber&);  // Module.
  cBigNumber& operator %= (CBNL);
  cBigNumber& operator<<= (const cBigNumber&);  // Left shift.
  cBigNumber& operator<<= (CBNL);
  cBigNumber& operator>>= (const cBigNumber&);  // Right shift.
  cBigNumber& operator>>= (CBNL);
};

struct cBigInc: public cBigTemp                 // Postfix increment.
{
  cBigInc   (cBigNumber& a)                             { set (a); ++a; }
};

struct cBigDec: public cBigTemp                 // Postfix decrement.
{
  cBigDec   (cBigNumber& a)                             { set (a); --a; }
};

struct cBigPos: public cBigTemp                 // Unary +.
{
  cBigPos   (const cBigNumber& a)                       { set (a);      }
  cBigPos   (CBNL a)                                    { set (a);      }
};

struct cBigNeg: public cBigTemp                 // Unary -.
{
  cBigNeg   (const cBigNumber& a)                       { setneg (a);   }
  cBigNeg   (CBNL a)                                    { setneg (a);   }
};

struct cBigAbs: public cBigTemp                 // Absolute value.
{
  cBigAbs   (const cBigNumber& a)                       { setabs (a);   }
  cBigAbs   (CBNL a)                                    { setabs (a);   }
};

struct cBigUnsign: public cBigTemp              // Unsigned value.
{
  cBigUnsign(const cBigNumber& a)                       { setunsign (a);}
  cBigUnsign(CBNL a)                                    { setunsign (a);}
};

struct cBigCompl: public cBigTemp               // Bit-wise inversion.
{
  cBigCompl (const cBigNumber& a)                       { setcompl (a); }
  cBigCompl (CBNL a)                                    { setcompl (a); }
};

struct cBigXor: public cBigTemp                 // Bit-wise sum by module 2.
{
  cBigXor (const cBigNumber& a, const cBigNumber& b)    { setxor (a, b); }
  cBigXor (const cBigNumber& a, CBNL b)                 { setxor (a, b); }
  cBigXor (CBNL a, const cBigNumber& b)                 { setxor (a, b); }
  cBigXor (CBNL a, CBNL b)                              { setxor (a, b); }
};

struct cBigAnd: public cBigTemp                 // Bit-wise conjunction.
{
  cBigAnd (const cBigNumber& a, const cBigNumber& b)    { setand (a, b); }
  cBigAnd (const cBigNumber& a, CBNL b)                 { setand (a, b); }
  cBigAnd (CBNL a, const cBigNumber& b)                 { setand (a, b); }
  cBigAnd (CBNL a, CBNL b)                              { setand (a, b); }
};

struct cBigOr: public cBigTemp                  // Bit-wise disjunction.
{
  cBigOr  (const cBigNumber& a, const cBigNumber& b)    { setor  (a, b); }
  cBigOr  (const cBigNumber& a, CBNL b)                 { setor  (a, b); }
  cBigOr  (CBNL a, const cBigNumber& b)                 { setor  (a, b); }
  cBigOr  (CBNL a, CBNL b)                              { setor  (a, b); }
};

struct cBigAdd: public cBigTemp                 // Addition.
{
  cBigAdd (const cBigNumber& a, const cBigNumber& b)    { setadd (a, b); }
  cBigAdd (const cBigNumber& a, CBNL b)                 { setadd (a, b); }
  cBigAdd (CBNL a, const cBigNumber& b)                 { setadd (a, b); }
  cBigAdd (CBNL a, CBNL b)                              { setadd (a, b); }
};

struct cBigSub: public cBigTemp                 // Subtraction.
{
  cBigSub (const cBigNumber& a, const cBigNumber& b)    { setsub (a, b); }
  cBigSub (const cBigNumber& a, CBNL b)                 { setsub (a, b); }
  cBigSub (CBNL a, const cBigNumber& b)                 { setsub (a, b); }
  cBigSub (CBNL a, CBNL b)                              { setsub (a, b); }
};

struct cBigMul: public cBigTemp                 // Multiplication.
{
  cBigMul (const cBigNumber& a, const cBigNumber& b)    { setmul (a, b); }
  cBigMul (const cBigNumber& a, CBNL b)                 { setmul (a, b); }
  cBigMul (CBNL a, const cBigNumber& b)                 { setmul (a, b); }
  cBigMul (CBNL a, CBNL b)                              { setmul (a, b); }
};

struct cBigDiv: public cBigTemp                 // Division.
{
  cBigDiv (const cBigNumber& a, const cBigNumber& b)    { setdiv (a, b); }
  cBigDiv (const cBigNumber& a, CBNL b)                 { setdiv (a, b); }
  cBigDiv (CBNL a, const cBigNumber& b)                 { setdiv (a, b); }
  cBigDiv (CBNL a, CBNL b)                              { setdiv (a, b); }
};

struct cBigDivMod: public cBigTemp              // Division & module.
{
  cBigDivMod (cBigNumber& a, const cBigNumber& b)       { setdivmod (a, b); }
  cBigDivMod (cBigNumber& a, CBNL b)                    { setdivmod (a, b); }
};

struct cBigMod: public cBigTemp                 // Module.
{
  cBigMod (const cBigNumber& a, const cBigNumber& b)    { setmod (a, b); }
  cBigMod (const cBigNumber& a, CBNL b)                 { setmod (a, b); }
  cBigMod (CBNL a, const cBigNumber& b)                 { setmod (a, b); }
  cBigMod (CBNL a, CBNL b)                              { setmod (a, b); }
};

struct cBigShl: public cBigTemp                 // Left shift.
{
  cBigShl (const cBigNumber& a, const cBigNumber& b)    { setshl (a, b); }
  cBigShl (const cBigNumber& a, CBNL b)                 { setshl (a, b); }
  cBigShl (CBNL a, const cBigNumber& b)                 { setshl (a, b); }
  cBigShl (CBNL a, CBNL b)                              { setshl (a, b); }
};

struct cBigShr: public cBigTemp                 // Right shift.
{
  cBigShr (const cBigNumber& a, const cBigNumber& b)    { setshr (a, b); }
  cBigShr (const cBigNumber& a, CBNL b)                 { setshr (a, b); }
  cBigShr (CBNL a, const cBigNumber& b)                 { setshr (a, b); }
  cBigShr (CBNL a, CBNL b)                              { setshr (a, b); }
};

struct cBigPow: public cBigTemp                 // Power.
{
  cBigPow (const cBigNumber& a, const cBigNumber& b)    { setpow (a, b); }
  cBigPow (const cBigNumber& a, CBNL b)                 { setpow (a, b); }
  cBigPow (CBNL a, const cBigNumber& b)                 { setpow (a, b); }
  cBigPow (CBNL a, CBNL b)                              { setpow (a, b); }
};

struct cBigPowMod: public cBigTemp              // Power by module.
{
  cBigPowMod (const cBigNumber& a, const cBigNumber& b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, const cBigNumber& b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, CBNL b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, CBNL b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, const cBigNumber& b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, const cBigNumber& b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, CBNL b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, CBNL b, CBNL mod)
                                                { setpowmod (a, b, mod); }
};

struct cBigSqrt: public cBigTemp                // Square root.
{
  cBigSqrt (const cBigNumber& a)                        { setsqrt (a);   }
  cBigSqrt (CBNL a)                                     { setsqrt (a);   }
};

struct cBigSqrtRm: public cBigTemp              // Square root & remainder.
{
  cBigSqrtRm (cBigNumber& a)                            { setsqrtrm (a); }
};

struct cBigBits: public cBigTemp                // Number of meaning bits.
{
  cBigBits (cBigNumber& a)                              { setbits (a);   }
};

struct cBigExBits: public cBigTemp              // Number of meaning low 0-bits.
{
  cBigExBits (cBigNumber& a)                            { setexbits (a); }
};

struct cBigRandom: public cBigTemp              // Random value.
{
  cBigRandom (unsigned long (*p)(), unsigned CBNL n)    { setrandom (p, n); }
};

//================================================
//      Implementation of unary operations, which
//      require for creation of temporary object.
//================================================

inline cBigInc   operator ++ (cBigNumber &a, int)
{
  return cBigInc (a);
}

inline cBigDec   operator -- (cBigNumber &a, int)
{
  return cBigDec (a);
}

inline cBigPos   operator + (const cBigNumber &a)
{
  return cBigPos (a);
}

inline cBigNeg   operator - (const cBigNumber &a)
{
  return cBigNeg (a);
}

inline cBigCompl operator ~ (const cBigNumber &a)
{
  return cBigCompl (a);
}

//================================================
//      Implementation of binary operations, which
//      require for creation of temporary object.
//================================================

inline cBigXor operator ^ (const cBigNumber& a, const cBigNumber& b)
{
  return cBigXor (a, b);
}

inline cBigXor operator ^ (const cBigNumber& a, CBNL b)
{
  return cBigXor (a, b);
}

inline cBigXor operator ^ (CBNL b, const cBigNumber& a)
{
  return cBigXor (a, b);
}

inline cBigAnd operator & (const cBigNumber& a, const cBigNumber& b)
{
  return cBigAnd (a, b);
}

inline cBigAnd operator & (const cBigNumber& a, CBNL b)
{
  return cBigAnd (a, b);
}

inline cBigAnd operator & (CBNL b, const cBigNumber& a)
{
  return cBigAnd (a, b);
}

inline cBigOr  operator | (const cBigNumber& a, const cBigNumber& b)
{
  return cBigOr  (a, b);
}

inline cBigOr  operator | (const cBigNumber& a, CBNL b)
{
  return cBigOr  (a, b);
}

inline cBigOr  operator | (CBNL b, const cBigNumber& a)
{
  return cBigOr  (a, b);
}

inline cBigAdd operator + (const cBigNumber& a, const cBigNumber& b)
{
  return cBigAdd (a, b);
}

inline cBigAdd operator + (const cBigNumber& a, CBNL b)
{
  return cBigAdd (a, b);
}

inline cBigAdd operator + (CBNL b, const cBigNumber& a)
{
  return cBigAdd (a, b);
}

inline cBigSub operator - (const cBigNumber& a, const cBigNumber& b)
{
  return cBigSub (a, b);
}

inline cBigSub operator - (const cBigNumber& a, CBNL b)
{
  return cBigSub (a, b);
}

inline cBigSub operator - (CBNL a, const cBigNumber& b)
{
  return cBigSub (a, b);
}

inline cBigMul operator * (const cBigNumber& a, const cBigNumber& b)
{
  return cBigMul (a, b);
}

inline cBigMul operator * (const cBigNumber& a, CBNL b)
{
  return cBigMul (a, b);
}

inline cBigMul operator * (CBNL b, const cBigNumber& a)
{
  return cBigMul (a, b);
}

inline cBigDiv operator / (const cBigNumber& a, const cBigNumber& b)
{
  return cBigDiv (a, b);
}

inline cBigDiv operator / (const cBigNumber& a, CBNL b)
{
  return cBigDiv (a, b);
}

inline cBigDiv operator / (CBNL a, const cBigNumber& b)
{
  return cBigDiv (a, b);
}

inline cBigMod operator % (const cBigNumber& a, const cBigNumber& b)
{
  return cBigMod (a, b);
}

inline cBigMod operator % (const cBigNumber& a, CBNL b)
{
  return cBigMod (a, b);
}

inline cBigMod operator % (CBNL a, const cBigNumber& b)
{
  return cBigMod (a, b);
}

inline cBigShl operator << (const cBigNumber& a, const cBigNumber& b)
{
  return cBigShl (a, b);
}

inline cBigShl operator << (const cBigNumber& a, CBNL b)
{
  return cBigShl (a, b);
}

inline cBigShl operator << (CBNL a, const cBigNumber& b)
{
  return cBigShl (a, b);
}

inline cBigShr operator >> (const cBigNumber& a, const cBigNumber& b)
{
  return cBigShr (a, b);
}

inline cBigShr operator >> (const cBigNumber& a, CBNL b)
{
  return cBigShr (a, b);
}

inline cBigShr operator >> (CBNL a, const cBigNumber& b)
{
  return cBigShr (a, b);
}

#endif//_CBIGNUM_H
