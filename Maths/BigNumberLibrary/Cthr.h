/* --------------------------------------------------------------
    Macro for multithreading support (version 2.0).

    http://www.imach.uran.ru/cbignum

    Copyright 2010 by Raul N.Shakirov, IMach of RAS(UB).
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
#ifndef _CTHR_H
#define _CTHR_H

#ifndef __STDC__

/*
    Visual C++ & Borland C++ Builder
*/
#ifdef  _MT
#ifdef  _WIN32
#define _CTHR_PROCESS
#endif/*_WIN32*/
#ifdef  _WIN64
#define _CTHR_PROCESS
#endif/*_WIN64*/
#endif/*_MT*/

#endif/*__STDC__*/

#ifdef  _CTHR_PROCESS
#include <process.h>
#endif/*_CTHR_PROCESS*/

#endif/*_CTHR_H*/
