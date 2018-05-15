/* --------------------------------------------------------------
    Macro for stream input-output (version 2.0).

    http://www.imach.uran.ru/cbignum

    Copyright 2005-2010 by Raul N.Shakirov, IMach of RAS(UB).
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
#ifndef _CIOS_H
#define _CIOS_H

#ifndef _CIOS_OLDSTREAM
#ifndef _CIOS_STDSTREAM

#ifdef  _MSC_VER
#if     _MSC_VER >= 1300
#define _CIOS_STDSTREAM
#endif//_MSC_VER
#endif//_MSC_VER

#ifdef  __GNUC__
#if     __GNUC__ >= 3
#define _CIOS_STDSTREAM
#endif/*__GNUC__*/
#endif/*__GNUC__*/

#ifndef _CIOS_STDSTREAM
#define _CIOS_OLDSTREAM
#endif//_CIOS_STDSTREAM

#endif//_CIOS_STDSTREAM
#endif//_CIOS_OLDSTREAM

#ifdef  _CIOS_STDSTREAM
#include <iostream>
#include <iomanip>
#define _CIOSTD std ::
#endif//_CIOS_STDSTREAM

#ifdef  _CIOS_OLDSTREAM
#include <iostream.h>
#include <iomanip.h>
#define _CIOSTD
#endif//_CIOS_OLDSTREAM

#endif//_CIOS_H
