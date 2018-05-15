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
#ifndef _CBIGNUMS_H
#define _CBIGNUMS_H

#include "Cbignum.h"
#include "Cios.h"

_CIOSTD istream& operator >> (_CIOSTD istream& is, cBigNumber& b);
_CIOSTD ostream& operator << (_CIOSTD ostream& is, const cBigNumber& b);

#endif//_CBIGNUMS_H
