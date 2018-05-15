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
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#define DISALLOW_EXARRAY_SHORTTYPE
#include "Cbignum.h"
#include "Cbignumf.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))
static  int     log2_BITS = cLongExactLog2 (BITS);

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

#ifndef _CBIGNUM_MT
#define _CBIGS  static
#else //_CBIGNUM_MT
#define _CBIGS
#endif//_CBIGNUM_MT

//================================================
//      Stub for of zero initialized number.
//================================================

#ifndef _CBIGNUM_DEF_ALLOC
CBNL cBigNumber::_stub [(EXALLOC_EXTRA_ITEMS(CBNL) > 2?
                         EXALLOC_EXTRA_ITEMS(CBNL) : 2)];
#endif//_CBIGNUM_DEF_ALLOC

//================================================
//      Service objects in thread local storage.
//================================================

//      Service objects are global to exclude expenses for creation
//      of temporary objects on execution of time-critical methods.
//      These objects are local to each program thread as it will be
//      requested by thread_local keyword of forthcoming C++ 0x standard.
//      Memory, occupied for service objects is not freed.
//
//      We use special aggregate types with no constructors to declare
//      service objects because on compilers limitations and also must
//      add fictive (not used) objects to instantiate templates.
//
//      Note that variant with static member variables instead of the
//      global ones caused crash in Borland C++ 4.5 and Builder 1.0.

#ifdef  EXTHREAD_LOCAL
exblockstack_t EXTHREAD_LOCAL cBigTemp_stack;   // Stack of unlimited numbers.
exblockstack<long>            cBigTemp_sfict;   // Fictive array.
#else
exblockstack_t                cBigTemp_stack;   // Stack (not used).
#endif//EXTHREAD_LOCAL

//================================================
//      Class service variables.
//================================================

size_t  cBigNumberMaskDiv0;                     // Mask for division by 0.

//      Deprecated, not for reenterable code and multithreaded applications.
#ifndef _CBIGNUM_MT
cBigNumber cBigNumberLastDivMod;                // Remainder of last division.
cBigNumber cBigNumberLastRootRm;                // Remainder of last root.
#endif//_CBIGNUM_MT

//================================================
//      Service functions.
//================================================

inline size_t size_max (size_t n1, size_t n2)
{
  return ((n1 > n2)? n1 : n2);
}
inline size_t size_min (size_t n1, size_t n2)
{
  return ((n1 < n2)? n1 : n2);
}

//================================================
//      Implementation of information methods.
//================================================

cBigNumber& cBigNumber::setbits     (const cBigNumber& b)
{
  size_t n = cBigNumberWords (CBPTRTYPE(b));
  set (n > 0? cULongBits ((unsigned CBNL)
                (b.code()[--n] ^ (b.hiword()>>(BITS-1)))): 0);
  return addmul ((CBNL)n, BITS);
}

cBigNumber& cBigNumber::setbits     (CBNL b)
{
  return set (cLongBits (b));
}

cBigNumber& cBigNumber::setexbits   (const cBigNumber& b)
{
  size_t n = cBigNumberExWords (CBPTRTYPE(b));
  set (cLongExBits (b.code()[n - (b.length() == 0)]));
  return addmul ((CBNL)n, BITS);
}

cBigNumber& cBigNumber::setexbits   (CBNL b)
{
  return set (cLongExBits (b));
}

//================================================
//      Implementation of service conversions.
//================================================

cBigNumber& cBigNumber::fit()
{
  if (length() == 0) checkexpand (1);
  checkindex (cBigNumberFit (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::tab()
{
  checkexpand (exmuladd (BITS, length() + (length() == 0), BITS * 2 + 1));
  cBigNumberTab (EXPTRTYPE(*this));
  return *this;
}

cBigNumber& cBigNumber::smp()
{
#ifndef _CBIGNUM_HARDWARE_MUL
  checkexpand (exmuladd (BITS, length() + (length() == 0), BITS * 2 + 1));
  cBigNumberTab (EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  return *this;
}

cBigNumber& cBigNumber::gc()
{
  if (len != 0)
  {
#ifndef _CBIGNUM_DEF_ALLOC
    if (_testnot0() == 0) clear(); else
#endif//_CBIGNUM_DEF_ALLOC
    checkshrink (length());
  }
  return *this;
}

cBigNumber& cBigNumber::pack()
{
  if (len != 0)
  {
#ifndef _CBIGNUM_DEF_ALLOC
    if (_testnot0() == 0) clear(); else
#endif//_CBIGNUM_DEF_ALLOC
    reallocate (length() + 1);
  }
  return *this;
}

//================================================
//      Implementation of clearing.
//================================================

void cBigNumber::clear()
{
#ifndef _CBIGNUM_DEF_ALLOC
  if (len) exfree (e);
  e = _stub; len = 0;
#else //_CBIGNUM_DEF_ALLOC
  if (len) { checkshrink (1); e [0] = 1; e [1] = 0; }
#endif//_CBIGNUM_DEF_ALLOC
}

//================================================
//      Implementation of unary accumulations.
//================================================

cBigNumber& cBigNumber::operator ++()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMInc (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::operator --()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMDec (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::mul2()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMMul2 (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::div2()
{
  checkexpand (length());
  checkindex (cBigNumberMDiv2 (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::pow2()
{
  checkexpand (length() * 2 + 2);
  cBigNumberMul (CBPTRTYPE(*this), CBPTRTYPE(*this), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sqrt()
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastRootRm);
  return setsqrtrm (cBigNumberLastRootRm);
}

//================================================
//      Implementation of assign operations.
//================================================

cBigNumber& cBigNumber::setxor (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberXor (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberXor (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberXor (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberXor (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberXor (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberXor (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (CBNL a, CBNL b)
{
  return *this = a ^ b;
}

cBigNumber& cBigNumber::setand (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberAnd (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberAnd (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberAnd (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberAnd (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberAnd (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberAnd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (CBNL a, CBNL b)
{
  return *this = a & b;
}

cBigNumber& cBigNumber::setor  (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberOr (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberOr (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberOr (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberOr (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberOr (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberOr (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (CBNL a, CBNL b)
{
  return *this = a | b;
}

cBigNumber& cBigNumber::setadd (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length() + 1),
     cBigNumberAdd (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length() + 1),
     cBigNumberAdd (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length() + 1),
     cBigNumberAdd (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberAdd (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length() + 1),
     cBigNumberAdd (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberAdd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (CBNL a, CBNL b)
{
  checkexpand (2);
  checkindex (cBigNumberAdd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(_cBigLong(b)),
                             EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::setsub (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length() + 1),
     cBigNumberSub (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length() + 1),
     cBigNumberSubS(CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length() + 1),
     cBigNumberSub (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberSubS(CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length() + 1),
     cBigNumberSubS(CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberSub (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (CBNL a, CBNL b)
{
  checkexpand (2);
  checkindex (cBigNumberSub (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(_cBigLong(b)),
                             EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::setmul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (la + lb + 2);
  if (la >= lb)
        cBigNumberMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (const cBigNumber& a, CBNL b)
{
  checkexpand (a.length() + 3);
  cBigNumberMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (CBNL a, const cBigNumber& b)
{
  checkexpand (b.length() + 3);
  cBigNumberMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (CBNL a, CBNL b)
{
#ifndef _CBIGNUM_HARDWARE_MUL
  checkexpand (4);
  cBigNumberMul (EXPTRTYPE(_cBigLong(a)), EXPTRTYPE(_cBigLong(b)),
                 EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  checkexpand (2); cLongMul (a, b, e + 1);
  e [0] = 1 + ((e [1] >> (BITS-1)) != e [2]);
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdiv (const cBigNumber& a, const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod, b);
}

cBigNumber& cBigNumber::setdiv (const cBigNumber& a, CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod, b);
}

cBigNumber& cBigNumber::setdiv (CBNL a, const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod, b);
}

cBigNumber& cBigNumber::setdiv (CBNL a, CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod, b);
}

cBigNumber& cBigNumber::setmod (const cBigNumber& a, const cBigNumber& b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (const cBigNumber& a, CBNL b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (CBNL a, const cBigNumber& b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (CBNL a, CBNL b)
{
  return (*this = a) %= b;
}

//================================================
//      Implementation of shifts.
//================================================

cBigNumber& cBigNumber::operator <<= (const cBigNumber& b)
{
  return (*this <<= b.toCBNL());
}

cBigNumber& cBigNumber::operator <<= (CBNL b)
{
  if (b < 0) return (*this >>= (-b));
  int n = (int)(b % BITS); b /= BITS;
  checkexpand ((unsigned CBNL)b < UINT_MAX - 2 - length()?
                      (size_t)b + length() + 2: UINT_MAX);
  cBigNumberFit (EXPTRTYPE(*this));
  for (; n > 0; --n) cBigNumberMMul2 (EXPTRTYPE(*this));
  cBigNumberCopyShl (CBPTRTYPE(*this), (size_t)b, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setshl (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (const cBigNumber& a, CBNL b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (CBNL a, const cBigNumber& b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (CBNL a, CBNL b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::operator >>= (const cBigNumber& b)
{
  return (*this >>= b.toCBNL());
}

cBigNumber& cBigNumber::operator >>= (CBNL b)
{
  if (b < 0) { if (-b > 0) return (*this <<= (-b));
               else { cBigNumberERange (CBPTRTYPE(_cBigLong (b))); b = 0; } }
  size_t n = (size_t) b % BITS; b /= BITS;
  if (length() == 0) checkexpand (1);
  cBigNumberFit (EXPTRTYPE(*this));
  for (; n > 0; --n) cBigNumberMDiv2 (EXPTRTYPE(*this));
  cBigNumberCopyShr (CBPTRTYPE(*this),
                     (unsigned CBNL)b <= UINT_MAX? (size_t)b: UINT_MAX,
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setshr (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (const cBigNumber& a, CBNL b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (CBNL a, const cBigNumber& b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (CBNL a, CBNL b)
{
  *this = a; return *this >>= b;
}

//================================================
//      Implementation of power.
//================================================

cBigNumber& cBigNumber::pow (const cBigNumber& b)
{
  cBigNumber a; swap (a); a.fit();
  if (b.hiword() >= 0)                          // Positive degree.
  {
    setbits (a) *= b;                           // Estimate for size of result.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // Optimized division.
    else
      *this = cBigDivMod (*this, BITS);         // Generic division.
  }
  else *this = 0;                               // Negative degree.
  CBNL lw = loword();
  size_t n = ((length() <= 1 &&
              (unsigned CBNL)lw <= UINT_MAX - 4)?
                    ((size_t)lw + 4): UINT_MAX);
  checkexpand (n); a.checkexpand (n);
  cBigNumberPow (EXPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::pow (CBNL b)
{
  cBigNumber a; swap (a); a.fit();
  if (b >= 0)                                   // Positive degree.
  {
    setbits (a) *= b;                           // Estimate for size of result.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // Optimized division.
    else
      *this = cBigDivMod (*this, BITS);         // Generic division.
  }
  else *this = 0;                               // Negative degree.
  CBNL lw = loword();
  size_t n = ((length() <= 1 &&
              (unsigned CBNL)lw <= UINT_MAX - 4)?
                    ((size_t)lw + 4): UINT_MAX);
  checkexpand (n); a.checkexpand (n);
  cBigNumberPow (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setpow (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (const cBigNumber& a, CBNL b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (CBNL a, const cBigNumber& b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (CBNL a, CBNL b)
{
  *this = a; return pow (b);
}

//================================================
//      Implementation of power by module.
//================================================

cBigNumber& cBigNumber::powmod (const cBigNumber& b, const cBigNumber& mod)
{
  cBigTemp a; swap (a);
  size_t n = mod.length() * 2 + 2;
  checkexpand (n); a.checkexpand (size_max (a.length() + 1, n));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(b),
                    CBPTRTYPE(mod), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (const cBigNumber& b, CBNL mod)
{
  cBigTemp a; swap (a);
  checkexpand (4); a.checkexpand (size_max (a.length() + 1, 4));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(b),
                    CBPTRTYPE(_cBigLong(mod)), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (CBNL b, const cBigNumber& mod)
{
  cBigTemp a; swap (a);
  size_t n = mod.length() * 2 + 2;
  checkexpand (n); a.checkexpand (size_max (a.length() + 1, n));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                    CBPTRTYPE(mod), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (CBNL b, CBNL mod)
{
  cBigTemp a; swap (a);
  checkexpand (4); a.checkexpand (size_max (a.length() + 1, 4));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                    CBPTRTYPE(_cBigLong(mod)), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, const cBigNumber& b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, const cBigNumber& b,
                                   CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, CBNL b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, CBNL b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (CBNL a, const cBigNumber& b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (CBNL a, const cBigNumber& b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (CBNL a, CBNL b, const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (CBNL a, CBNL b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

//================================================
//      Implementation of square root.
//================================================

cBigNumber& cBigNumber::setsqrt (const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm (b);
#else //_CBIGNUM_MT
  cBigNumberLastRootRm = b;
#endif//_CBIGNUM_MT
  return setsqrtrm (cBigNumberLastRootRm);
}

cBigNumber& cBigNumber::setsqrt (CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm (b);
#else //_CBIGNUM_MT
  cBigNumberLastRootRm = b;
#endif//_CBIGNUM_MT
  return setsqrtrm (cBigNumberLastRootRm);
}

//================================================
//      Implementation of random generator.
//================================================

cBigNumber& cBigNumber::setrandom (unsigned long (*p)(), unsigned CBNL lBits)
{
  checkexpand ((size_t)(lBits / BITS + 1));
  cBigNumberRandom (p, lBits, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of combined operations.
//================================================

cBigNumber& cBigNumber::add (const cBigNumber& b, size_t k)
{
  checkexpand (size_max (length(), b.length() + k) + 1);
  cBigNumberMAddShl (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::add (CBNL b, size_t k)
{
  checkexpand (size_max (length(), k + 1) + 1);
  cBigNumberMAddShl (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sub (const cBigNumber& b, size_t k)
{
  checkexpand (size_max (length(), b.length() + k) + 1);
  cBigNumberMSubShl (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sub (CBNL b, size_t k)
{
  checkexpand (size_max (length(), k + 1) + 1);
  cBigNumberMSubShl (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (size_max (length(), la + lb + 1) + 1);
  if (la >= lb)
        cBigNumberMAddMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMAddMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (const cBigNumber& a, CBNL b)
{
  checkexpand (size_max (length(), a.length() + 2) + 1);
  cBigNumberMAddMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (CBNL a, const cBigNumber& b)
{
  checkexpand (size_max (length(), b.length() + 2) + 1);
  cBigNumberMAddMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (CBNL a, CBNL b)
{
  CBNL c [5]; c [0] = 1; c [1] = a;
  checkexpand (size_max (length(), 2) + 1);
  if (b >= 0)
     cBigNumberMAddMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(b)), 0, EXPTRTYPE(*this));
  else
     cBigNumberMSubMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(-b)), 0, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (size_max (length(), la + lb + 1) + 1);
  if (la >= lb)
        cBigNumberMSubMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMSubMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (const cBigNumber& a, CBNL b)
{
  checkexpand (size_max (length(), a.length() + 2) + 1);
  cBigNumberMSubMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (CBNL a, const cBigNumber& b)
{
  checkexpand (size_max (length(), b.length() + 2) + 1);
  cBigNumberMSubMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (CBNL a, CBNL b)
{
  CBNL c [5]; c [0] = 1; c [1] = a;
  checkexpand (size_max (length(), 2) + 1);
  if (b >= 0)
     cBigNumberMSubMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(b)), 0, EXPTRTYPE(*this));
  else
     cBigNumberMAddMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(-b)), 0, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setsqrtrm (cBigNumber& a)
{
  checkexpand (a.fit().length() / 2 + 1);
  if (a.hiword() >= 0)
    cBigNumberMRmSqrt (EXPTRTYPE(a), EXPTRTYPE(*this));
  else
    *this = 0;
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

//================================================
//      Implementation of operations on
//      preliminary prepared operands.
//================================================

cBigNumber& cBigNumber::addmultab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmultab (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submultab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submultab (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmulsmp (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(b),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmulsmp (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(_cBigLong(b)),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submulsmp (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(b),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submulsmp (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(_cBigLong(b)),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivtab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmodtab (cBigNumberLastDivMod, b, k);
}

cBigNumber& cBigNumber::setdivtab (CBNL a, const cBigNumber& b, size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod (a);
#else //_CBIGNUM_MT
  cBigNumberLastDivMod = a;
#endif//_CBIGNUM_MT
  return setdivmodtab (cBigNumberLastDivMod, b, k);
}

cBigNumber& cBigNumber::setmodtab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  *this = a;
  return modtab (b, k);
}

cBigNumber& cBigNumber::setmodtab (CBNL a, const cBigNumber& b, size_t k)
{
  *this = a;
  return modtab (b, k);
}

//================================================
//      Basic tests for primality of numbers.
//================================================

//      Strong Probable Primarily test for base b
//      by Olivier Langlois <olanglois@sympatico.ca>
//      Returns:
//      0 - composite
//      2 - probably prime
//      http://primes.utm.edu/glossary/page.php?sort=StrongPRP

int     b_SPRP  (
        const cBigNumber& n,            // Number to test.
        const cBigNumber& b             // Base.
)
{
  if (n <= 3) return (n < 2? 0: 2);     // Check for small numbers.
  if ((n.loword() & 1) == 0) return 0;  // Check if number is odd.

//                                            a
//      Find t and a satisfying: n - 1 = t * 2 , t odd
//

  cBigNumber t = n;
  cBigNumber nMinus1 = --t;
  cBigNumber a; a.setexbits (nMinus1);
  t >>= a;

//      We check the congruence class of      i
//                                       t * 2
//                                      b      % n
//      for each i from 0 to a - 1.
//      If any one is correct, then n passes.
//      Otherwise, n fails.

  cBigNumber test; test.setpowmod (b, t, n);

  if (test == 1 || test == nMinus1) return 2;

  if (a)
  {
    while (--a)
    {
      test.pow2(); test %= n; if (test == nMinus1) return 2;
    }
  }

  return 0;
}

//================================================
//      Messages.
//================================================

const char* cBigNumber::pszConverting   = "Converting";
const char* cBigNumber::pszFormatting   = "Formatting";

//================================================
//      Tables for conversion routines.
//================================================

//      Table for conversion of digits to symbols.

const char  cBigNumber::numtochar [2] [MAX_RADIX + 1] =
                       {{'0','1','2','3','4','5','6','7','8','9',
                         'a','b','c','d','e','f','x'},
                        {'0','1','2','3','4','5','6','7','8','9',
                         'A','B','C','D','E','F','X'}};

//      Table for conversion of symbols to digits, build by SqrTab().

      char  cBigNumber::chartonum [UCHAR_MAX + 1];

//      Table of coefficients contains maximal integer power of type long
//      and number of symbols in string representation of the aforesaid
//      power for each radix.

static const long   maxradix[][2] = {{ 0L,  1},     //  0
                                     { 1L,  1},     //  1
                            { 1073741824L, 30},     //  2
                            { 1162261467L, 19},     //  3
                            { 1073741824L, 15},     //  4
                            { 1220703125L, 13},     //  5
                            {  362797056L, 11},     //  6
                            { 1977326743L, 11},     //  7
                            { 1073741824L, 10},     //  8
                            {  387420489L,  9},     //  9
                            { 1000000000L,  9},     // 10
                            {  214358881L,  8},     // 11
                            {  429981696L,  8},     // 12
                            {  815730721L,  8},     // 13
                            { 1475789056L,  8},     // 14
                            {  170859375L,  7},     // 15
                            {  268435456L,  7}};    // 16

//      For optimization of input/output of unlimited numbers we create
//      array with consequent squares of coefficients for each radix.
//
//      Maximal number of consequent squares MAX_NP_IN/MAX_NP_OUT is
//      determined by the criteria of minimization of input/output time
//      for numbers of size about 1,000,000 bit on processor with fast
//      cache of size >= 256K.
//
//      Large MAX_NP_IN can be set if effective Karatsuba multiplication
//      is used for senior squares, otherwise it must be same as MAX_NP_OUT.

#ifdef  _CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 18)
#else //_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 9)
#endif//_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_OUT  ((sizeof (size_t))==2? 2: 9)
#define MAX_NP      (MAX_NP_IN > MAX_NP_OUT? MAX_NP_IN: MAX_NP_OUT)

//      Squares for which progress indicator is shown.
//      SHOW_NP_IN is greater SHOW_NP_OUT, because input is faster
//      than output, in particular with hardware acceleration.

#define SHOW_NP_IN  (MAX_NP_IN < 12? MAX_NP_IN: 12)
#define SHOW_NP_OUT (MAX_NP_OUT)

//      For small squares table of shifts is generated and used.

#ifdef  _CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 7? MAX_NP_IN: 7)
#else //_CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 8? MAX_NP_IN: 8)
#endif//_CBIGNUM_HARDWARE_MUL
#define MAX_TAB_OUT (MAX_NP_OUT)
#define MAX_TAB     (MAX_SMP_IN > MAX_TAB_OUT? MAX_SMP_IN: MAX_TAB_OUT)

//     We store references to dynamically allocated cBigNumber objects
//     but not objects itself because in multithreading environment
//     these static arrays can be filled in concurrent mode.

static cBigNumber*  powradix    [MAX_RADIX + 1] [MAX_NP + 1];
static cBigNumber*  powradixtab [MAX_RADIX + 1] [MAX_TAB + 1];
static size_t       powradixexw [MAX_RADIX + 1] [MAX_TAB + 1];

//      Filling of the conversion table and the array of coefficients and
//      squares. Table must contain at least low coefficient and its square.
//      The method use anti-racing code, because it can be called
//      simultaneously from multiple threads.

size_t  cBigNumber::SqrTab (
                unsigned radix,         // Radix (2..16).
                size_t   len,           // Number of words in number.
                size_t   max_np         // Maximal number of squares + 1.
)                                       // Returns required number of
{                                       // squares + 1 (0..max_np).
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);
  assert (max_np <= MAX_NP);
  static size_t npow [MAX_RADIX + 1];   // Number of squares + 1.

  if (npow [radix] == 0)
  {
//      Fill in the conversion table (anti-racing code).

    int i;
    for (i = 0; i < MAX_RADIX; i++)
    {
      chartonum [numtochar [0] [i]] = (char)i;
      chartonum [numtochar [1] [i]] = (char)i;
    }
    for (i = 0; i <= UCHAR_MAX; i++)
    {
      if (chartonum [i] == 0 && i != '0') chartonum [i] = MAX_RADIX;
    }

//      Allocate array for coefficients. This array will never
//      be deleted because we'll take references to it. If parallel
//      threads create multiple arrays, only one will be used and
//      other will become memory leak.

    EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 3);

//      The lowest coefficient.

    pow [2] = maxradix [radix] [0];
    pow [2].tab();                      // Table of shifts.
    powradix    [radix] [0] =
    powradixtab [radix] [0] = &pow [2]; // Anti-racing code.
    powradixexw [radix] [0] = 0;

//      Square of the lowest coefficient.

    pow [0].setmul (maxradix [radix] [0], maxradix [radix] [0]);
    powradix    [radix] [1] = &pow [0]; // Anti-racing code.

    pow [1].setr (pow [0], powradixexw [radix] [1] = pow [0].exwords());
    pow [1].tab();                      // Table of shifts.
    powradixtab [radix] [1] = &pow [1]; // Anti-racing code.

    npow [radix] = 2;                   // Number of squares + 1.
  }

//      Higher consequent squares, if required.

  size_t np = 0;                        // Index of coefficient.
  while (np < max_np && powradix [radix] [np]->length() * 4 < len)
  {
    if (++np >= npow [radix])
    {

//      Allocate array for coefficients. This array will never
//      be deleted because we'll take references to it. If parallel
//      threads create multiple arrays, only one will be used and
//      other will become memory leak.

      EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 2);

      pow [0] = 0;                     // Subsequent square.
      if (np <= MAX_SMP_IN)
        pow [0].addmulsmp
          (*powradixtab [radix] [np - 1], *powradixtab [radix] [np - 1],
            powradixexw [radix] [np - 1] * 2);
      else
        pow [0].addmul
          (*powradix [radix] [np - 1], *powradix [radix] [np - 1]);

      powradix [radix] [np] = &pow [0]; // Anti-racing code.

      if (np <= MAX_TAB)
      {
        pow [1].setr (pow [0], powradixexw [radix] [np] = pow [0].exwords());
        pow [1].tab();                  // Table of shifts.
        powradixtab [radix] [np] = &pow [1];
      }                                 // Anti-racing code.
      npow [radix] = np + 1;            // Number of squares + 1.
    }
  }
  assert (np <= max_np);
  return np;
}

//================================================
//      Conversion of string to unlimited number.
//================================================

//      Conversion of char array to unsigned unlimited number of radix 2..16.

cBigNumber& cBigNumber::setunsign (     // Conversion from unsigned char array.
                const EXPTR(char) ps,   // Char array with digits.
                unsigned radix,         // Radix (2..16).
                size_t   width          // Number of chars in array.
            )
{
  assert (ps);
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);

//      Input of number.
//
//      Initial (non-optimized algorithm looks as the following:
//      *this = 0;
//      for (; width > 0; --width)
//      {
//        *this *= radix;
//        *this += chartonum [*ps++];
//      }
//
//      In optimized algorithm we use methods of multiplication
//      with accumulation addmul and addmulsmp. To reduce number
//      of multiplications we use multiplication to coefficients
//      powradix, obtained by consequent squaring of radix.
//      Results of consequent multiplications with accumulation
//      are stored in num_ and stack of multipliers mul.
//      Initially stack is empty.
//
//      ALGORITHM
//      Multipliers of 0-th order are represented by long word
//      and are obtained by conversion of short subchains.
//      Multiplier of i-th order is obtained by addmul or addmulsmp
//      as accumulation of two consequent multipliers of order i-1,
//      first of which is stored in mul [i].
//      Result number is obtained by addmul or addmulsmp as consequent
//      accumulation of highest multiplier stored in num_.

  *this = 0;
  if (width != 0)
  {
    long lradix =       maxradix [radix] [0];
    int  nradix = (int) maxradix [radix] [1];
    size_t nr = (width - 1) / nradix;   // Number of full words (no more).
    size_t n = width - nr++ * nradix;   // Number of symbols in last word.

//      Filling of stack of coefficients powradix [radix].
//      Required number of coefficients is returned.

    size_t np = SqrTab (radix, nr, MAX_NP_IN);
    EXPTR(cBigNumber*) powr    = EXPTRTO(cBigNumber*,
                                         powradix    [radix], MAX_NP_IN + 1);
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_SMP_IN + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_SMP_IN + 1);

//      Zero-filling of stack of multipliers.

    _CBIGS cBigNumber num_;                     // The highest multiplier.
    _CBIGS exvector<cBigNumber> stk_ (MAX_NP_IN);
    EXPTR(cBigNumber) mul = EXPTRTYPE(stk_);    // Stack of multipliers.
    { for (size_t i = 1; i < np; i++) mul [i] = 0; }

//      Progress indication.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = nr * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;
    if (numlength != 0 && np >= SHOW_NP_IN)
      cProgress.showScaled (cBigNumber::pszConverting, 0, (long)numlength);

//      Cycle on pair groups of nradix digits.

    long lhigh = 0;                     // The high word.
    if ((nr & 1) != 0) goto odd;        // The lower word (may be incomplete).
    do
    {
//      Conversion of high group of nradix digits.

      {
        long lb = 0;                    // Word.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        lhigh = lb;                     // The high word.
      }
      --nr;                             // Have converted the group.

//      Conversion of lower group of nradix digits.

odd:  {
        long lb = 0;                    // Word.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        num_ = lb;                      // Lower word.
      }
      --nr;                             // Have converted the group.

//      Calculation of multiplier of 1st order.

      num_.addmul (lhigh, lradix);      // Accumulation of words.
      assert (num_ >= 0);

//      Calculation of multipliers of 2nd and higher order.
//      and accumulation of the number.

      size_t i = 1;
      for (;; i++)
      {
        if (i < np)                     // Calculation of multipliers.
        {
          if ((nr & (2<<i)-1) != 0)     // Odd multiplier
          {                             // is stored
            num_.swap (mul [i]);        // in the stack.
            break;
          }                             // Otherwise multiplier of
                                        // higher order is calculated.
          if (i <= MAX_SMP_IN)
            num_.addmulsmp (*powrtab [i], mul [i], powrexw [i]);
          else
            num_.addmul (*powr [i], mul [i]);
          assert (num_ >= 0);
        }
        else                            // Accumulation of the number.
        {
          if (i <= MAX_SMP_IN)
            num_.addmulsmp (*powrtab [i], *this, powrexw [i]);
          else
            num_.addmul (*powr [i], *this);
          assert (num_ >= 0);
          num_.swap (*this);
          break;
        }
      }

      if (numlength != 0 && i == SHOW_NP_IN)
        cProgress.showScaled (cBigNumber::pszConverting,
          (long)(numlength - nr * sizeof (long)), (long)numlength);
    }
    while (nr != 0);
  }

//      Conversion finished.

  return *this;
}

//      Conversion of string to signed unlimited number of radix 2..16.
//      If radix is not indicated (set to 0) then radix is set to
//      10, 8 or 16 depending of prefixes 0, 0x, 0X.

cBigNumber& cBigNumber::set (           // Conversion from string.
                const char* psz,        // String.
                unsigned radix          // Radix (2..16) or 0.
            )                           // 0 - by C conventions.
{
  assert (psz);
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  int sign = 0;                         // Sign of number.
  unsigned c = *psz++;                  // Current symbol.

//      Recognition of sign.

  switch (c)
  {
    case '-': sign = 1;
    case '+': c = *psz++;
  }

//      Recognition of radix.

  if (radix == 0)
  {
    radix = 10;
    if (c == '0')
    {
      c = *psz++; radix = 8;
      if (c == 'X' || c == 'x') { c = *psz++; radix = 16; }
    }
  }

//      Find end of number.

  SqrTab (radix);                       // Filling of chartonum.
  int width = 0;
  for (;;)
  {
    if ((size_t)chartonum [c] >= radix) break;
    c = psz [width++];                  // Next symbol.
  }
  --psz;

//      Decoding of number.

  setunsign (EXPTRTO(char, psz, width), radix, width);

//      Assigning of sign.

  if (sign) this->neg();

  return *this;
}

//================================================
//      Conversion of unlimited number to string.
//================================================

//      Conversion of unlimited number to string for radixes 2..16,
//      sign and radix at left, number at right.
//
//      Format flags:
//      cBigNumber_unsign     unsigned output.
//      cBigNumber_uppercase  upper-case letters for number with radix > 10.
//      cBigNumber_showbase   add indicator of radix (0 or 0x).
//      cBigNumber_showpos    show sign for positive numbers.

char*   cBigNumber::toa (               // Conversion to string.
                cBigString& buf_,       // Extendable buffer for conversion.
                unsigned radix,         // Radix (2..16) or 0 (=10).
                size_t   width,         // Minimal number of chars in string.
                long     fill           // Fill-in character and format flags.
        ) const                         // Returns pointer to buffer.
{
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  if (radix == 0) radix = 10;
  size_t i = 0;                                 // Index in buffer.
  _CBIGS cBigNumber num_;                       // The highest quotient.
  num_ = *this; num_.fit();                     // Normalization.
  if (fill & cBigNumber_unsign) num_.unsign();  // Make the number unsigned.

//      Select conversion table.

  const EXPTR(char) ps = EXPTRTO (char,
                    numtochar [(fill & cBigNumber_uppercase) != 0],
                    sizeof (numtochar [0]));

//      Output of sign and prefixes.

  {
    CBNL sign = num_.comp0();                   // Sign.

    if (sign < 0)
    {
      buf_ [i++] = '-'; num_.neg();             // Output sign '-'.
    }
    else if (sign > 0 && (fill & cBigNumber_showpos))
    {
      buf_ [i++] = '+';                         // Output sign '+'.
    }

    if (radix % 8 == 0 && (fill & cBigNumber_showbase))
    {
      buf_ [i++] = '0';                         // Output radix.
      if (radix == 16) buf_ [i++] = ps [16];
    }
  }

//      Output of number in reverse order.
//
//      Initial (non-optimized algorithm looks as the following:
//      do
//      {
//        buf_ [i++] = ps [num % radix];
//        num_ /= radix;
//      }
//      while (num_ != 0);
//
//      In optimized algorithm we use method setdivmodtab for
//      division with module. To reduce number of divisions
//      we use division to coefficients powradix, obtained by
//      consequent squaring of radix.
//
//      As a result of division to the highest coefficient with index
//      npmax we obtain quotient and module, which are divided to
//      coefficient with index npmax-1, e.t.c, until as result
//      of division to lowest coefficient with index 0 we obtain
//      number that fit to digital grid of type long. Long word
//      is converted to string be consequent division to radix.
//
//      The highest quotient is stored in num_. Lower quotients are stored
//      in the stack of quotients div and marked in the stack of flags kdiv.
//      Flag is 1, if the corresponding div contains low quotient.
//      For the highest quotient the corresponding kdiv is always 0.
//      Initially array of quotients is empty.
//
//      ALGORITHM
//      When the highest quotient is divided quotient is stored in num_
//      and module is stored in div [npmax - 1]. When div [npmax - 1]
//      is divided quotient is stored in div [npmax - 1] and module is
//      stored in div [npmax - 2] e.t.c. Short module and quotient from
//      division of div [0] are converted to string. While module is
//      converted quotient is stored in div [0]. Then quotient div [1]
//      is moved to div [0] and conversion is repeated, starting from
//      div [0], then div [2] is moved to div [1] and the process is
//      repeated from div [1], e.t.c. np to the lowers quotient, when
//      npmax is decreased and the algorithm is repeated once again.

  size_t j = i;                                 // Position of the 1st digit.
  {
//      Filling of stack of coefficients powradix [radix].
//      Required number of coefficients is returned.

    size_t np = SqrTab (radix, num_.length(), MAX_TAB_OUT);
#ifdef  _CBIGNUM_SMALL_DIV
    EXPTR(cBigNumber*) powr    = EXPTRTO(cBigNumber*,
                                         powradix    [radix], MAX_TAB_OUT + 1);
#endif//_CBIGNUM_SMALL_DIV
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_TAB_OUT + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_TAB_OUT + 1);

//      Work stacks.

    _CBIGS cBigNumber mod_;                     // Module of division.
    _CBIGS exvector<cBigNumber> stk_ (MAX_TAB_OUT);
    _CBIGS exvector<int>       kstk_ (MAX_TAB_OUT);
    EXPTR(cBigNumber) div = EXPTRTYPE(stk_);    // Stack of quotients.
    EXPTR(int)       kdiv = EXPTRTYPE(kstk_);   // Stack of flags.

//      Progress indication.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = num_.length() * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;

//      Cycle of quotients.
//      All parts if number are converted, except for the last module

    size_t npmax = np;                          // Index of highest quotient.
    for (;;)                                    // Cycle of quotient.
    {
      if (np == npmax)                          // The highest quotient.
      {
        if (numlength != 0 && np >= SHOW_NP_OUT)
          cProgress.showScaled (cBigNumber::pszFormatting,
            (long)(numlength - num_.length() * sizeof (long)),
            (long)numlength);

//        The highest quotient is divided to the corresponding coefficient.
//        If result is 0 and index of the highest quotient os greater 0
//        then index of the highest quotient decreases and the cycle
//        repeats once again.

        num_.swap (mod_);                       // Store quotient to num_
#ifdef  _CBIGNUM_SMALL_DIV                      // and module to mod_.
        if (np <= (size_t)(_CBIGNUM_SMALL_DIV > 1)) {
          num_.setdivmod (mod_, *powr [np]);
        } else
#endif//_CBIGNUM_SMALL_DIV
          num_.setdivmodtab (mod_, *powrtab [np], powrexw [np]);

        if (num_.comp0() == 0)                  // Highest quotient is over,
        {                                       // highest module remains.
          if (np == 0) break;                   // If it is the last module
                                                // the cycle is over,
          num_.swap (mod_);                     // else module becomes
          npmax = --np;                         // the highest quotient,
          numlength = 0;                        // indication is blocked
          assert (kdiv [np] == 0);              // and after debug check
          continue;                             // division is repeated.
        }
      }
      else                                      // Find of quotient.
      {
        assert (np > 0);                        // Debug check.
        assert (kdiv [np - 1] == 0);            // Debug check.
        if (kdiv [np] == 0)                     // If no quotient is found,
        {                                       // then go to higher
          ++np; continue;                       // quotient.
        }                                       // If quotient is found,
        mod_.swap (div [np]);                   // then load div [np]
        kdiv [np] = 0;                          // and mark is as free.
      }

//      Cycle of division of module - while index of module is greater 0,
//      divide it by the corresponding coefficient.

      while (np != 0)
      {
        assert (kdiv [np - 1] == 0);            // Debug check.
        kdiv [--np] = 1;                        // Set flag.
#ifdef  _CBIGNUM_SMALL_DIV
        if (np <= (size_t)(_CBIGNUM_SMALL_DIV > 1)) {
          div [np].setdivmod (mod_, *powr [np]);
        } else                                  // Store quotient to div
#endif//_CBIGNUM_SMALL_DIV                      // and module to mod_.
          div [np].setdivmodtab (mod_, *powrtab [np], powrexw [np]);
      }

//      Cycle of conversion of the lowest module and quotient,
//      which fits to digital grid of type long.
//      The lowest module is stored in mod_ and the lowest quotient,
//      is stored in div [0], if it exists there (it is always so,
//      except for case npmax == 0).

      {
        long lnum = (long) mod_.loword();       // Output of module.
        for (;;)                                // Repeat cycle
        {                                       // once or twice.
          int knum = (int) maxradix [radix] [1] - 1;
          do                                    // Output knum digits.
          {
            buf_ [i++] = ps [ (int) (lnum % radix) ];
            lnum /= radix;                      // Output digits.
          }
          while (--knum != 0);

          assert (lnum >= 0);                   // Debug check.
          assert (lnum < (long) radix);         // Debug check.
          buf_ [i++] = ps [(int)(lnum)];        // The last digit.

          if (kdiv [0] == 0) break;             // Is there quotient?
          lnum = (long) div [0].loword();       // Output of quotient.
          kdiv [0] = 0;                         // div [0] is free.
        }
      }

//      Repeat the cycle, starting form the most lower quotient.

      np = (npmax != 0);
    }                                           // Cycle on quotients.

//      Output the last module.

    {
      long lnum = (long) mod_.loword();
      if (lnum != 0)                            // Short module.
      do
      {
        buf_ [i++] = ps [(int) (lnum % radix)]; // Output of digit.
      }
      while ((lnum /= radix) != 0);
    }

//      Output 0 for number 0.

    if (i == j) buf_ [i++] = ps [0];            // The number is 0.
  }

//      Output of fill-in character and finishing of string.

  {
    if ((char)fill == '\0') fill = ps [0];      // Determine fill-in character.
    while (i < width) buf_ [i++] = (char)fill;  // Output of fill-in character.
    buf_ [i] = 0;                               // String is ready.
    EXDEBUG (buf_.checkindex (i));
    i--;
  }

//      Swapping of symbols.

  {
    EXPTR(char) psb = EXPTRTYPE(buf_);
    while (i > j)
    {
      char c = psb [j]; psb [j] = psb [i]; j++; psb [i] = c; i--;
    }
  }

//      Conversion is finished.

  return ((char*)(const char *)buf_);           // Return string.
}

//      Deprecated non-reenterable method returns pointer to static string,
//      which is overwritten after each consequent call of method.

#ifdef  _CBIGNUM_TOATMP
char*   cBigNumber::toatmp (            // Conversion to string.
                unsigned radix,         // Radix (2..16) or 0 (=10).
                size_t   width,         // Minimal number of chars in string.
                long     fill           // Fill-in character and format flags.
        ) const                         // Returns pointer to string
{                                       // in internal static buffer.
  static cBigString buf_;
  return toa (buf_, radix, width, fill);
}
#endif//_CBIGNUM_TOATMP

//================================================
//      Implementation of independent output.
//================================================

cTTY& operator << (cTTY& os, const cBigNumber& b)
{
  cTTY_h ph = os;
  if (ph) { cBigString buf_; (*ph) (&os, b.toa (buf_), 0, 0, 0); }
  return os;
}

void operator <<= (cTTY& os, const cBigNumber& b)
{
  cTTY_h ph = os;
  if (ph) { cBigString buf_; (*ph) (&os, b.toa (buf_), 0, 0, '\r'); }
}

//================================================
//      Objects for console output.
//================================================

cTTY    cBigNumberMessages;             // Handler for messages.
cTTY    cBigNumberProgress;             // Handler for progress indicators.

//================================================
//      Hexadecimal dump of unsigned CBNL value.
//================================================

cHexDump::cHexDump (unsigned CBNL num)
{
  buffer [BITS / 4] = 0;
  for (size_t i = BITS / 4; i > 0; i--)
  {
    buffer [i - 1] = cBigNumber::numtochar [0] [(size_t)num & 0xF];
    num >>= 4;
  }
}
