/* --------------------------------------------------------------
    Signed integers with unlimited range (version 2.0).
    Stream input-output.

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2010 by Raul N.Shakirov, IMach of RAS(UB).
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
#include <string.h>
#define DISALLOW_EXARRAY_SHORTTYPE
#include "Cbignum.h"
#include "Cbignums.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

#ifndef _CBIGNUM_MT
#define _CBIGS  static
#else //_CBIGNUM_MT
#define _CBIGS
#endif//_CBIGNUM_MT

//================================================
//      Operator of stream input.
//================================================

//      Formatting is controlled by the following flags:
//      ios::skipws     skip initial spaces
//      ios::dec        number of radix 10
//      ios::oct        number of radix 8
//      ios::hex        number of radix 16

_CIOSTD istream& operator >> (_CIOSTD istream& is, cBigNumber& b)
{
  assert (&is);
  assert (&b);
  long flags = is.flags();              // Flags.
  unsigned radix = (                    // Radix.
         (flags & _CIOSTD ios::dec)? 10:
         (flags & _CIOSTD ios::oct)? 8:
         (flags & _CIOSTD ios::hex)? 16: 0);

//      Skip spaces if ios::skipws is set and read 1st symbol.

  if (flags & _CIOSTD ios::skipws) is >> _CIOSTD ws;
  int c = is.get();                     // Current symbol.

//      Recognition of sign.

  int sign = 0;                         // Sign of number.
  switch (c)
  {
    case '-': sign = 1;
    case '+': c = is.get();
  }
  if (radix == 0)                       // Recognition of radix.
  {
    radix = 10;
    if (c == '0')
    {
      c = is.get(); radix = 8;
      if (c == 'X' || c == 'x') { c = is.get(); radix = 16; }
    }
  }

//      Input of symbols.

  cBigNumber::SqrTab (radix);           // Filling of cBigNumber::chartonum.
  _CBIGS exarray<char> str_;            // Char array with digits.
  size_t width = 0;                     // Number of chars in array.
  for (;;)
  {
    if ((size_t)cBigNumber::chartonum [(unsigned char)c] >= radix)
      break;
    str_ [width++] = (char)c;
    c = is.get();                        // Next symbol.
  }
  if (c != EOF) is.putback ((char)c);    // Return non-recognized symbol.

  str_ [width] = 0;                      // End of string.
  EXDEBUG (str_.checkindex (width));

//      Decoding of number.

  b.setunsign (EXPTRTYPE(str_), radix, width);

//      Assigning of sign.

  if (sign) b.neg();

  return is;
}

//================================================
//      Operator of stream output.
//================================================

//      Formatting of output is controlled by the following flags:
//      ios::width()    width of field.
//      ios::fill()     filling symbol.
//      ios::left       align left.
//      ios::right      align right.
//      ios::internal   sign and radix at left, number at right.
//      ios::dec        signed number of radix 10 (default).
//      ios::oct        signed number of radix 8.
//      ios::hex        signed number of radix 16.
//      ios::showbase   add indicator of radix (0 or 0x).
//      ios::showpos    show sign for positive numbers.
//      ios::uppercase  upper-case letters for number with radix > 10.

_CIOSTD ostream& operator << (_CIOSTD ostream& os, const cBigNumber& b)
{
  assert (&os);
  assert (&b);
  long fill  = 0;
  long flags = (&os? os.flags(): 0);
  unsigned radix = ((flags & _CIOSTD ios::hex)? 16:
                    (flags & _CIOSTD ios::oct)? 8: 10);
  unsigned width = 0;

  if (flags & _CIOSTD ios::internal)
  {
    width = (unsigned) os.width();
    fill  = (long)     os.fill();
  }
#ifdef  _CBIGNUM_UNSIGN_OCT_HEX
  if (radix != 10) fill |= cBigNumber_unsign;
#endif//_CBIGNUM_UNSIGN_OCT_HEX
  if (flags & _CIOSTD ios::uppercase) fill |= cBigNumber_uppercase;
  if (flags & _CIOSTD ios::showbase)  fill |= cBigNumber_showbase;
  if (flags & _CIOSTD ios::showpos)   fill |= cBigNumber_showpos;

  _CBIGS cBigString str;
  os << b.toa (str, radix, width, fill);
  return os;
}
