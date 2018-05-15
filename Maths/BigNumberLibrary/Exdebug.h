/* --------------------------------------------------------------
    Macro for check of bounds in the debug mode (version 1.2c).

    http://www.imach.uran.ru/exarray

    Copyright 1998-2010 by Dr. Raul N.Shakirov, IMach of RAS(UB).
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
#ifndef EXDEBUG_H
#define EXDEBUG_H

#ifndef EXARRAY_H
#include "Exarray.h"
#endif/*EXARRAY_H*/

/* --------------------------------------------------------------
    EXDEBUG(a)      - Execute expression a in the debug mode,
                      when no NDEBUG macro is defined.
-------------------------------------------------------------- */

#ifndef NDEBUG
#define EXDEBUG(a)  ((void)(a))
#else /*NDEBUG*/
#define EXDEBUG(a)  ((void)0)
#endif/*NDEBUG*/

/* --------------------------------------------------------------
    EXARR(T,m,n)    - Define of debug array m of size n of type T
                      adding extra element for fail-safe purposes.
                      Debug array acts as ordinary C array and also
                      "knows" size of assigned array to perform
                      check when accessing elements of array.
                      If program attempts to access element
                      outside of array bounds, it terminates.

    EXPTR(T) p      - Debug pointer p to array of type T.
                      Debug pointer acts as ordinary C array and
                      "knows" size of assigned array to perform
                      check when accessing elements of array.
                      If program attempts to access element
                      outside of array bounds, it terminates,

    EXPTRTO(T,m,n)  - Construct debug pointer to existing
                      array m of n elements of type T.
                      Array may be represented thereby name,
                      C pointer or debug pointer. For debug
                      pointer check of bounds is carried out.

    EXPTRTYPE(m)    - Convert dynamic array, dynamic pointer,
                      regulated array or restricted pointer
                      m to format, which allows assignment
                      to debug pointer.

    EXPTRBASE(p)    - Convert debug array or pointer p to
                      ordinary C array or pointer without
                      index check.

    EXPTRCHECK(p,n) - Convert debug array or pointer p to
                      ordinary C array or pointer and check if
                      elements 0..n-1 are accessible.

    EXPTRINDEX(p,i) - Convert debug array or pointer p to
                      ordinary C array or pointer and check if
                      elements 0..i are accessible.

    EXPTRNEW(T,n)   - Allocate array for n elements of type T,
                      initialize (ref. ALLOW_EXARRAY_NEW_DELETE),
                      call constructors and return debug pointer.

    EXPTRDELETE(p)  - Call destructors and deallocate array,
                      allocated by EXPTRNEW.

    EXPTRALLOC(T,n) - Allocate array for n elements of type T,
                      initialize and return debug pointer.
                      Do not call constructors.

    EXPTRFREE(p)    - Deallocate array, allocated by EXPTRALLOC.
                      Do not call destructors.

    EXPTRALIGNED_ALLOC(T,n,a)   - EXPTRALLOC with alignment.
    EXPTRALIGNED_FREE(T)        - EXPTRFREE with alignment.

    Debug arrays and pointers implements check of indexes if no
    NCHECKPTR macro is defined, otherwise debug arrays and pointers
    are implemented as ordinary C pointers.

    Example:            Debug (no NCHECKPTR)            NCHECKPTR
    =================== ===========================     ===========
    EXARR(int,m,10);    int m___ [11];                  int m [11];
                        exptr<int> p=exptr<int>(m___,10)
    EXPTR(int) p;       exptr<int> p;                   int* p;
    p = EXPTRTO(m,10);  p = exptr<int>(m,10);           p = m;
    EXPTRTYPE(m);       m;                              m.base();
    EXPTRBASE(p);       p.base();                       p;
-------------------------------------------------------------- */

#ifdef  NCHECKPTR

#define EXARR(T,m,n)    T m[(n)+1]
#define EXPTR(T)        T*
#define EXPTRTO(T,m,n)  (m)
#define EXPTRTYPE(m)    ((m).base())
#define EXPTRBASE(p)    (p)
#define EXPTRCHECK(p,n) (p)
#define EXPTRINDEX(p,i) (p)
#define EXPTRNEW(T,n)   (EXNEW(T,(n)))
#define EXPTRDELETE(p)  (EXDELETE(p))
#define EXPTRALLOC(T,n) (EXALLOC(T,(n)))
#define EXPTRFREE(p)    (EXFREE(p))
#define EXPTRALIGNED_ALLOC(T,n,a)   (EXALIGNED_ALLOC(T,(n),(a)))
#define EXPTRALIGNED_FREE(p)        (EXALIGNED_FREE(p))

#else /*NCHECKPTR*/

#define EXARR(T,m,n)    T m##___ [(n)+1];\
                        exptr<T>m = exptr<T>(m##___,\
                                      sizeof(m##___)/sizeof(T)-1,0)
#define EXPTR(T)        exptr<T>
#define EXPTRTO(T,m,n)  (exptr<T>((m),(n)))
#define EXPTRTYPE(m)    (m)
#define EXPTRBASE(p)    ((p).base())
#define EXPTRCHECK(p,n) ((p).check(n))
#define EXPTRINDEX(p,i) ((p).index(i))
#define EXPTRNEW(T,n)   (exptr<T>().allocate(n))
#define EXPTRDELETE(p)  ((p).deallocate())
#define EXPTRALLOC(T,n) (exptr<T>().alloc(n))
#define EXPTRFREE(p)    ((p).free())
#define EXPTRALIGNED_ALLOC(T,n,a)   (exptr<T>().aligned_alloc(n,a))
#define EXPTRALIGNED_FREE(p)        ((p).aligned_free())

#endif/*NCHECKPTR*/

#endif/*EXDEBUG_H*/
