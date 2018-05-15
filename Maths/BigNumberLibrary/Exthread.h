/* --------------------------------------------------------------
    Macro for thread local storage (version 1.2c).

    http://www.imach.uran.ru/exarray

    Copyright 2010 by Dr. Raul N.Shakirov, IMach of RAS(UB).
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
#ifndef EXTHREAD_H
#define EXTHREAD_H

/* --------------------------------------------------------------
    EXTHREAD_LOCAL  - Prefix for thread local storage.
-------------------------------------------------------------- */

/*
    Compatible C++ 0x
*/
#if 0
#ifndef EXTHREAD_LOCAL
#ifdef  __cplusplus
#if     __cplusplus >= 200801L
#define EXTHREAD_LOCAL  thread_local
#endif/*__cplusplus*/
#endif/*__cplusplus*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Borland C++
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  __TURBOC__
#ifdef  __MT__
#if     __TURBOC__ >= 0x460
#define EXTHREAD_LOCAL  __thread
#endif/*__TURBOC__*/
#else /*__MT__*/
#define EXTHREAD_LOCAL
#endif/*__MT__*/
#endif/*__TURBOC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Visual C++, Intel C/C++ for Windows
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  _MSC_VER
#ifdef  _MT
#if     _MSC_VER >= 800
#define EXTHREAD_LOCAL  __declspec(thread)
#endif/*_MSC_VER*/
#else/* _MT*/
#define EXTHREAD_LOCAL
#endif/*_MT*/
#endif/*_MSC_VER*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    GNU gcc, Intel C/C++ for Linux
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  __GNUC__
#if defined(_REENTRANT) || defined(_LIBC_REENTRANT)
#if     __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#define EXTHREAD_LOCAL  __thread
#endif/*__GNUC__*/
#else /*_REENTRANT*/
#define EXTHREAD_LOCAL
#endif/*_REENTRANT*/
#endif/*__GNUC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Sun Studio C/C++
*/
#if 0
#ifndef EXTHREAD_LOCAL
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define EXTHREAD_LOCAL  __thread
#endif/*__SUNPRO*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    IBM XL C/C++ for Linux and AIX with -qtls option
*/
#if 0
#ifndef EXTHREAD_LOCAL
#ifdef  __xlC__
#define EXTHREAD_LOCAL  __thread
#endif/*__xlC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

#endif/*EXTHREAD_H*/
