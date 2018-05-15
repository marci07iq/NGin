/* --------------------------------------------------------------
    Dynamic arrays with check of bounds (version 1.2c).

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
#ifndef EXARRAY_H
#define EXARRAY_H

#include <stddef.h>
#include <limits.h>

/* --------------------------------------------------------------
    Templates are intended for creating of arrays with
    check of indexes and automated increasing of size.

    Dynamic array has property of automatic growing of
    size when index goes out of array bounds.

    Dynamic pointer is intended for addressing of elements
    of dynamic array with automating growing of size.

    Regulated array checks index and blocks addressing
    of elements out of bounds of array.

    Restricted pointer is intended for addressing of
    arrays (conventional, regulated and dynamic) with check
    of bounds providing that size of arrays is fixed.

    Declaration:
    exarray<type>  name;     Dynamic array.
    expoint<type>  name;     Dynamic pointer.
    exvector<type> name;     Regulated array.
    exptr<type>    name;     Restricted pointer.

    Regular C operation on arrays and pointers are implemented.
-------------------------------------------------------------- */

#ifdef  __cplusplus
template <class T> class exarray;
template <class T> class expoint;
template <class T> class exvector;
template <class T> class exptr;
#endif/*__cplusplus*/

/* ==============================================================
    Preprocessing options.

1.  Language options are not defined by default as they may
    cause problems if compiler does not comply to Standard C++.

    ALLOW_EXARRAY_ADDSUB        - permits operators + and -
            on dynamic array and integer index, returning
            dynamic pointer. If macro is not defined, these
            operators are permitted only for pointers.
            * Causes swelling of debugging information in
              Borland C++ 3.1.

    ALLOW_EXARRAY_SELECTION     - permits operator ->.
    ALLOW_EXARRAY_ITERATORS     - permits operator ->
            and also defines properties and methods for
            application of templates as iterators and
            containers of library STL.
            * Not supported by Borland C++ 3.1 and 4.5.
            * STL container methods are partially implemented.

    ALLOW_EXARRAY_EXCEPTIONS    - permits explicit processing
            of exceptions. Option is mandatory if destructors
            of elements of array can start exceptions.
            * Not supported by the compiler Borland C++ 3.1.
            * Visual C++ 6.0 does not implement correctly
              processing of exceptions in destructors of
              local objects and built-in arrays (but
              correctly deletes dynamic arrays).


    ALLOW_EXARRAY_NEW_DELETE    - redefine operators
            new[] and delete[] for EXNEW and EXDELETE.
            * Is always defined if the compiler is full compatible
              with C++ standard (__cplusplus >= 199711L), also for
              Visual C++ 5.0 and above, Borland C++ 4.5 and above.

    ALLOW_EXARRAY_PLACEMENT     - support for placement delete.
            * Is always defined if the compiler is full compatible
              with C++ standard (__cplusplus >= 199711L), also for
              Visual C++ 6.0 and above.

    ALLOW_EXARRAY_CHARTYPE      - type char is distinct
            from signed/unsigned char.
            * Is always defined if the compiler is full compatible
              with C++ standard (__cplusplus >= 199711L), also for
              Visual C++ 6.0 and above, Borland C++ 4.5 and above.

    ALLOW_EXARRAY_SIZETYPE      - type size_t is greater
            then all standard unsigned types, including long.
            * Defined for all compilers that defined macro
              SSIZE_MAX or _WIN64 and _I64_MAX greater ULONG_MAX,
              for example, Visual C++ in 64 bit mode.

    ALLOW_EXARRAY_PTRDIFFTYPE   - type ptrdiff_t is greater
            then all standard unsigned types, including long,
            but not greater size_t,
            * Defined for all compilers that defined macro
              SSIZE_MAX or _WIN64 and _I64_MAX greater ULONG_MAX,
              for example, Visual C++ in 64 bit mode.

    DISALLOW_EXARRAY_LONGTYPE   - disable use of long
            and unsigned long indexes to access array.
            * Also disables size_t and ptrdiff_t indexes on
              64-bit g++.

    DISALLOW_EXARRAY_INTTYPE    - disable use of int
            and unsigned int indexes to access array.
            * Also disables size_t and ptrdiff_t indexes on
              16 and 32-bit compilers.

    DISALLOW_EXARRAY_SHORTTYPE  - disable use of short
            and unsigned short indexes to access array.

    ALLOW_EXARRAY_USING         - include using directives
            for members of parent class template.
            * Is always defined if the compiler is full compatible
              with C++ standard (__cplusplus >= 199711L), also for
              Visual C++ 2005 and above, GNU g++ 3.4 and above.
            * Not supported by Borland C++ 3.1 and 4.5.

3.  Optimization options.

    ALLOW_EXARRAY_SCHEDULING    - questionable optimization of
            bound check code, targeted to superscalar Pentium
            and Pentium MMX. NOT recommended to use.
            * Deleterious for Borland C++ 3.1 and 4.5,
              in some cases - for Borland C++ Builder,
            * Effect for Visual C++ depends on application,
              but can cause severe memory allocation bug
              when compiling under Visual C++ 6.0.

    NCHECKPTR                   - suppress debugging facilities.
            * Defined under either NDEBUG or C mode.
            * Visual C++ defines NDEBUG in Release mode.
============================================================== */
/*
    #define ALLOW_EXARRAY_ADDSUB
    #define ALLOW_EXARRAY_SELECTION
    #define ALLOW_EXARRAY_ITERATORS
    #define ALLOW_EXARRAY_EXCEPTIONS
    #define ALLOW_EXARRAY_NEW_DELETE
    #define ALLOW_EXARRAY_PLACEMENT
    #define ALLOW_EXARRAY_CHARTYPE
    #define ALLOW_EXARRAY_SIZETYPE
    #define ALLOW_EXARRAY_PTRDIFFTYPE
    #define DISALLOW_EXARRAY_LONGTYPE
    #define DISALLOW_EXARRAY_INTTYPE
    #define DISALLOW_EXARRAY_SHORTTYPE
    #define ALLOW_EXARRAY_USING
    #define ALLOW_EXARRAY_SCHEDULING
    #define NCHECKPTR
*/

#ifndef __cplusplus

#define NCHECKPTR

#else /*__cplusplus*/

#ifdef  NDEBUG
#define NCHECKPTR
#endif/*NDEBUG*/

#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Auxiliary included file.
-------------------------------------------------------------- */

#ifdef  ALLOW_EXARRAY_ITERATORS
#include <iterator>
#define ALLOW_EXARRAY_SELECTION
#endif/*ALLOW_EXARRAY_ITERATORS*/

/* --------------------------------------------------------------
    Determining of compiler.

    Compiler                        _MSC_VER  _TURBOC_
    --------                        --------  --------
    C Compiler     6.0                   600
    C/C++ compiler 7.0                   700
    Visual C++, 32-bit, version 1.0      800
    Visual C++, 32-bit, version 2.0      900
    Visual C++, 32-bit, version 4.0     1000
    Visual C++, 32-bit, version 4.2     1020
    Visual C++, 32-bit, version 5.0     1100
    Visual C++, 32-bit, version 6.0     1200
    Visual C++ .NET,    version 7.0     1300
    Visual C++ 2005,    version 8.0     1400
    Visual C++ 2008,    version 9.0     1500
    Visual C++ 2010,    version 10.0    1600
    Borland C++ 3.1                              0x410
    Borland C++ 4.5                              0x460
    Borland C++ Builder 1.0                      0x520
-------------------------------------------------------------- */
/*
    Compatible C++ ISO/IEC 14882
*/
#ifdef  __cplusplus
#if     __cplusplus >= 199711L
#define ALLOW_EXARRAY_NEW_DELETE
#define ALLOW_EXARRAY_PLACEMENT
#define ALLOW_EXARRAY_CHARTYPE
#define ALLOW_EXARRAY_USING
#endif/*__cplusplus*/
#endif/*__cplusplus*/

/*
    Borland C++
*/
#ifdef  __TURBOC__

#if     __TURBOC__ <= 0x410
#ifdef  ALLOW_EXARRAY_EXCEPTIONS
#undef  ALLOW_EXARRAY_EXCEPTIONS
#endif/*ALLOW_EXARRAY_EXCEPTIONS*/
#endif/*__TURBOC__*/

#if     __TURBOC__ >= 0x460
#define ALLOW_EXARRAY_NEW_DELETE
#define ALLOW_EXARRAY_CHARTYPE
#endif/*__TURBOC__*/

#endif/*__TURBOC__*/

/*
    Visual C++, Intel C/C++ for Windows
*/
#ifdef  _MSC_VER

#if     _MSC_VER >= 1100
#define ALLOW_EXARRAY_NEW_DELETE
#if     _MSC_VER >= 1200
#define ALLOW_EXARRAY_PLACEMENT
#define ALLOW_EXARRAY_CHARTYPE
#if     _MSC_VER >= 1400
#define ALLOW_EXARRAY_USING
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/

#pragma warning(disable: 4514)  // Unreferenced inline function is removed.
#pragma warning(disable: 4710)  // Function not inlined.

#ifdef  ALLOW_EXARRAY_SELECTION
#pragma warning(disable: 4284)  // Will produce errors for infix notation.
#endif/*ALLOW_EXARRAY_SELECTION*/

#endif/*_MSC_VER*/

/*
    GNU gcc, Intel C/C++ for Linux
*/
#ifdef  __GNUC__

#if     __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define ALLOW_EXARRAY_USING
#endif/*__GNUC__*/

#endif/*__GNUC__*/

/* --------------------------------------------------------------
    Macro for linkage options.
    STDECL  - standard C linkage
    EXDECL  - external function with fixed number of parameters
    FCDECL  - external function with accelerated calling
    INDECL  - force-inline function of C++
    VDECL   - external variable
-------------------------------------------------------------- */

#ifdef  __STDC__
#define STDECL
#else /*__STDC__*/
#define STDECL  __cdecl
#ifdef  _MSC_VER
#if     _MSC_VER >= 1200
#define INDECL  __forceinline
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/
#endif/*__STDC__*/

#ifndef EXDECL
#define EXDECL  STDECL
#endif/*EXDECL*/

#ifndef FCDECL
#define FCDECL  STDECL
#endif/*FCDECL*/

#ifndef INDECL
#define INDECL  inline
#endif/*INDECL*/

#define VDECL

/* ==============================================================
    Allocation of dynamic memory for C.
============================================================== */

/* --------------------------------------------------------------
    The following text may be doubled in other include files.
-------------------------------------------------------------- */

#ifndef EXALLOC_H_MAIN
#define EXALLOC_H_MAIN

/* --------------------------------------------------------------
    External references are declared as "C", in order
    to block name mangling. This facilitates porting of
    object modules among different compilers.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Structure, which set of variables is same as
    for dynamic array.
-------------------------------------------------------------- */

struct  exblock_t { void* e; size_t len; };

/* --------------------------------------------------------------
    Structure, which set of variables is same as
    for stack of dynamic arrays.
-------------------------------------------------------------- */

struct  exblockstack_t { exblock_t stack; size_t n; };

/* --------------------------------------------------------------
    Dynamic NULL-array.
-------------------------------------------------------------- */

extern struct  exblock_t VDECL exnull_c;

/* --------------------------------------------------------------
    Function set_exalloc_handler() sets handler of memory
    allocation errors, similar to set_new_handler().
-------------------------------------------------------------- */

extern  void (* set_exalloc_handler (void (*pvf) (void))) (void);

/* --------------------------------------------------------------
    Error handling pointers and information concerning
    with allocation of memory.

    exalloc_status.size_handler
        Pointer to function that handles unrecoverable memory
        allocation errors. The function must interrupt execution.

    exalloc_status.grow_handler
        Pointer to function that handles automatic expansion of
        dynamic array.

    exalloc_status.null_handler
        Pointer to function that handles NULL pointer indirection.
        The function must interrupt execution.

    exalloc_status.range_handler
        Pointer to function that handles index range error, if
        automatic expansion of dynamic array is not provided.
        The function must interrupt execution.
-------------------------------------------------------------- */

struct  exalloc_status_t
{
    void (* VDECL size_handler)(void);
    void (* VDECL grow_handler)(void);
    void (* VDECL null_handler)(void);
    void (* VDECL range_handler)(void);
    char*  pMinAlloc;               /* Minimal address */
    char*  pMaxAlloc;               /* Maximal address */
    int    nBlocksAllocated;        /* Number of allocated blocks */
    int    nBlocksFailed;           /* Number of allocation failures */
};
extern struct exalloc_status_t VDECL exalloc_status;

/* --------------------------------------------------------------
    Function exmalloc() allocates and clears block of memory.

    Function exmrealloc() allocates, reallocates, clears and
    frees block of memory.

    On failure the function calls memory allocation handler.

    Function exfree() frees block of memory.

    Functions exaligned_ allows to set alignment.
-------------------------------------------------------------- */

void*   FCDECL  exmalloc           (size_t blocksize);
void*   FCDECL  exaligned_malloc   (size_t blocksize, size_t blockalign);

void    FCDECL  exmrealloc         (void** p,
                                    size_t blocksize,
                                    size_t memset_start,
                                    size_t memset_stop);
void    FCDECL  exaligned_mrealloc (void** p,
                                    size_t blocksize, size_t blockalign,
                                    size_t memset_start,
                                    size_t memset_stop);

void    FCDECL  exfree             (void* p);
void    FCDECL  exaligned_free     (void* p);

/* --------------------------------------------------------------
    Function exrange() invokes:
    exalloc_status.null_handler  if p == NULL,
    exalloc_status.range_handler if p != NULL.
-------------------------------------------------------------- */

void    FCDECL  exrange (void* p);

/* --------------------------------------------------------------
    Constants for allocation of dynamic memory.

    EXSIZE_T_MAX        - maximal value, which can be given as
        result of integer calculations with control of overflow.

    EXARRAY_ALIGN       - alignment of block of memory, allocated
        for dynamic array. Available values - 1, 2, 4, 8, 16.

    EXCALCBLOCKSIZE_SYS - size of system heading of block of
        dynamic memory dependent from compiler (if size is
        unknown, it is possible to specify the top estimation,
        but no more than 16).

    EXCALCBLOCKSIZE_MOD - initial size of block of memory
       including system heading for algorithm of function
       excalcblocksize(), multiple of pow(64,n)+/-16 bytes.

    EXCALCBLOCKSIZE1.. 5 - initial size of block of memory
       excluding system heading obtained by algorithm of
       function excalcblocksize() and some subsequent
       sizes obtained by the same function.

    EXCALCBLOCKSIZE_MAX - maximal size of block of memory
       excluding system heading, which is permitted by
       memory allocation functions (operational system
       and equipment can impose additional restrictions).
-------------------------------------------------------------- */

#ifdef  _WIN64
#ifdef  _I64_MAX
#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        (_I64_MAX)
#endif/*EXSIZE_T_MAX*/
#if     _I64_MAX > ULONG_MAX
#define ALLOW_EXARRAY_SIZETYPE
#define ALLOW_EXARRAY_PTRDIFFTYPE
#endif/*_I64_MAX*/
#endif/*_I64_MAX*/
#endif/*_WIN64*/

#ifdef  SSIZE_MAX
#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        (SSIZE_MAX)
#endif/*EXSIZE_T_MAX*/
#if     SSIZE_MAX > ULONG_MAX
#define ALLOW_EXARRAY_SIZETYPE
#define ALLOW_EXARRAY_PTRDIFFTYPE
#endif/*SSIZE_MAX*/
#endif/*SSIZE_MAX*/

#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        ((sizeof(size_t)==2)? UINT_MAX: INT_MAX)
#endif/*EXSIZE_T_MAX*/

#define EXARRAY_ALIGN       ((sizeof(size_t)==2)? 1: 16)
#define EXCALCBLOCKSIZE_SYS (sizeof(size_t) * 2)
#define EXCALCBLOCKSIZE_MOD (sizeof(size_t) * 32 - 16)
#define EXCALCBLOCKSIZE_1   (EXCALCBLOCKSIZE_MOD * 1 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_2   (EXCALCBLOCKSIZE_MOD * 2 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_3   (EXCALCBLOCKSIZE_MOD * 3 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_4   (EXCALCBLOCKSIZE_MOD * 6 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_5   (EXCALCBLOCKSIZE_MOD * 9 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_MAX (EXSIZE_T_MAX - EXCALCBLOCKSIZE_SYS * 2)

/* --------------------------------------------------------------
    Function excalcblocksize() computes optimal size of
    memory block not less then required.
-------------------------------------------------------------- */

size_t  FCDECL  excalcblocksize (size_t blocksize);

/* --------------------------------------------------------------
    Inline functions of unsigned integer calculations with check
    of overflow:
      exmuladd (s,n,k)   = s * n + k.
      exmul    (s,n)     = s * n.
      exadd    (n,k)     = n + k.
    At overflow EXSIZE_T_MAX is returned.
      exchkadd (n,k,max) = n + k, if < = max, else error.
    For generation of optimal code use C++ compiler and
    substitute constants on place of s and k.
-------------------------------------------------------------- */

#ifndef __cplusplus
#define EXALLOC_OUT_OF_LINE
#endif/*__cplusplus*/

#ifdef  EXALLOC_OUT_OF_LINE

size_t  FCDECL  exmuladd (size_t s, size_t n, size_t k);
size_t  FCDECL  exmul    (size_t s, size_t n);
size_t  FCDECL  exadd    (size_t n, size_t k);
size_t  FCDECL  exchkadd (size_t n, size_t k, size_t max);

#else /*EXALLOC_OUT_OF_LINE*/
}

inline size_t exmuladd (size_t s, size_t n, size_t k)
{
    return ((n <= (EXSIZE_T_MAX - k) / s)? (n * s + k): EXSIZE_T_MAX);
}

inline size_t exmul (size_t s, size_t n)
{
    return ((s == 1)? n: (n <= EXSIZE_T_MAX / s)? n * s: EXSIZE_T_MAX);
}

inline size_t exadd (size_t n, size_t k)
{
    return ((n <= EXSIZE_T_MAX - k)? (n + k): EXSIZE_T_MAX);
}

inline size_t exchkadd (size_t n, size_t k, size_t max)
{
    if (n > max - k) (*exalloc_status.size_handler)();
    return (n + k);
}

extern "C" {
#endif/*EXALLOC_OUT_OF_LINE*/

/* --------------------------------------------------------------
    The next external references are liked in the default mode.
-------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Macro EXALLOC_EXTRA_ITEMS(T) defines number of auxiliary
    fail-safe elements in array of base type T.
    Possible values - 0 or above. Regular value - 1.
-------------------------------------------------------------- */

#ifndef EXALLOC_EXTRA_ITEMS
#define EXALLOC_EXTRA_ITEMS(T)  (1)
#endif/*EXALLOC_EXTRA_ITEMS*/

/* --------------------------------------------------------------
    Macro EXALLOC() allocates zero-filled block of memory
    for array of elements.

    Macro EXREALLOC() allocates, reallocates, clears and
    frees zero-filled block of memory.

    Size is set as number of elements n of type T.
    In addition to the requested number of elements memory
    for EXALLOC_EXTRA_ITEMS(T) zero-filled elements behind
    the last element is allocated.
    Constructors are not called.

    On failure memory allocation handler is called.

    Macro EXFREE() frees block of memory.
    Destructors are not called.

    Macro EXALIGNED_ allows to set alignment.
-------------------------------------------------------------- */

#define EXALLOC(T,n) ((T*) exmalloc (                                   \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T))))

#define EXREALLOC(T,p,n,nstart,nstop) (exmrealloc (                     \
                            (void**)&(p),                               \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            sizeof(T) * (size_t)(nstart),               \
                            sizeof(T) * (size_t)(nstop)))

#define EXFREE(p)           exfree (p)

#define EXALIGNED_ALLOC(T,n,a) ((T*) exaligned_malloc (                 \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            (a)))

#define EXALIGNED_REALLOC(T,p,n,nstart,nstop,a) (exaligned_mrealloc (   \
                            (void**)&(p),                               \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            (a),                                        \
                            sizeof(T) * (size_t)(nstart),               \
                            sizeof(T) * (size_t)(nstop)))

#define EXALIGNED_FREE(p)   exaligned_free (p)

/* --------------------------------------------------------------
    The next declarations are not expected to be doubled
    in other include files.
-------------------------------------------------------------- */

#endif/*EXALLOC_H_MAIN*/

/* ==============================================================
    Allocation of dynamic memory for both C and C++.
============================================================== */

/* --------------------------------------------------------------
    Macro EXNEW() allocates zero-filled block of memory
    and calls constructors.

    Size is set as number of elements n of type T.
    In addition to the requested number of elements memory
    for EXALLOC_EXTRA_ITEMS(T) zero-filled elements behind
    the last element is allocated.

    On failure memory allocation handler is called.

    Macro EXDELETE() calls C++ destructors and frees
    block of memory.

    In C mode these macro are identical to EXALLOC and EXFREE.
-------------------------------------------------------------- */

#ifdef  __cplusplus

template <class T> class exnew_c
{
    T value;
    exnew_c                (const exnew_c<T>&);
    exnew_c<T>& operator = (const exnew_c<T>&);
public:
    exnew_c                () {}
    void* operator new (size_t blocksize)
                        { return exmalloc (blocksize);  }
    void  operator delete   (void *p)   { exfree (p);   }
#ifdef  ALLOW_EXARRAY_NEW_DELETE
    void* operator new[] (size_t blocksize)
                        { return exmalloc (blocksize);  }
    void  operator delete[] (void *p)   { exfree (p);   }
#endif/*ALLOW_EXARRAY_NEW_DELETE*/
};
#define EXNEW(T,n)  ((T*) new exnew_c<T> [::exchkadd(   \
                         (n), EXALLOC_EXTRA_ITEMS(T),   \
                         (EXSIZE_T_MAX / sizeof(T)))])
#define EXDELETE(p) (::delete[] (p))

#else /*__cplusplus*/

#define EXNEW(T,n)  EXALLOC (T,n)
#define EXDELETE(p) EXFREE (p)

#endif/*__cplusplus*/

/* ==============================================================
    Templates of arrays and pointers for C++.

    Dynamic array is stored in continuous area of memory
    consisting of three parts:

    1) Elements of an array. Memory allocated for each element
       is initialized by 0, then the default constructor the base
       class is called. Elements are accessible for reading and
       writing.

    2) Zero initialized part, sufficient for placing of several
       elements of base type (minimal number of elements is set
       by macro EXALLOC_EXTRA_ITEMS). If zero initialized part
       contains correct values of base type, it can be read at
       reference without check of bounds.

    3) Reserved memory, which size is determined automatically.

    Presence of zero initialized part is essential for reference
    to a array without check of bounds, since it makes regular
    consequence of the widespread error - reference under index
    equal number of elements.

    For purposes of economy of memory an "empty" dynamic array,
    i.e. array without elements, can share zero-initialized
    static memory with other "empty" arrays of the same type.
============================================================== */

#ifdef  __cplusplus

/* --------------------------------------------------------------
    Supplementary template for calling of constructors and
    destructors without allocation of memory.
-------------------------------------------------------------- */

template <class T> class exreloc_c
{
    T value;
    exreloc_c                (const exreloc_c<T>&);
    exreloc_c<T>& operator = (const exreloc_c<T>&);
public:
    exreloc_c                () {}
/*
    Placement-operators and delete for calling of constructors
    and destructors without allocation of memory.
*/
    void* operator new    (size_t, void* p)
                                    { return p;    }
#ifdef  ALLOW_EXARRAY_PLACEMENT
    void  operator delete (void*, void*)  { ; }
#endif/*ALLOW_EXARRAY_PLACEMENT*/
    void  operator delete (void*)         { ; }
};

/* --------------------------------------------------------------
    Template of array with variable number of elements.

    Template is intended for production of inherited classes.
    It is not possible to construct objects exblock directly.
-------------------------------------------------------------- */

template <class T> class exblock
{
protected:
    T*          e;      /* Base array */
    size_t      len;    /* Number of elements */
/*
    Static zero-initialized array containing
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) bytes.
*/
    static T* stub();
/*
    Allocation of memory for given number of elements
    with indicating of size of block.
*/
    void  reallocate (size_t n, size_t blocksize);
/*
    Either interrupting of program or returning pointer
    to static array when addressing by index i.
*/
        T*  range ()            const;
        T*  range (size_t i)            { return range()-i; }
const   T*  range (size_t i)    const   { return range()-i; }
/*
    Allocation of memory for element with given index i.
    Returns e.
*/
        T*  grow  (size_t i)    const;
/*
    Constructor and destructor are protected, as indirect
    creation of objects exblock must be blocked.
    Constructor of inherited class must set e = stub(), len = 0,
    and then may call appropriate method for memory allocation.
    Destructor must call method clear().
*/
    exblock ()  { /* e = stub(); len = 0; */    }
    ~exblock()  { /* clear(); */                }

private:
/*
    To block assigning of arrays and their transfer
    to functions by value here are declared private
    copy constructor and assignment operator.
    One can pass arrays exblock to function either
    by reference or to restricted pointer.
*/
    exblock                (const exblock<T>&);
    exblock<T>& operator = (const exblock<T>&);

public:
/*
    Calculation of minimal size of memory block
    by required number of elements in the array.
    On overflow returns EXSIZE_T_MAX.
*/
    size_t min_blocksize (size_t n) const
    {
        return (exmuladd (
            sizeof(T), n,
            sizeof(T) * EXALLOC_EXTRA_ITEMS(T)));
    }
/*
    Allocation of memory exactly for indicated
    number of elements, no reserving.
*/
    void  reallocate (size_t n)
    {
        reallocate (n, min_blocksize (n));
    }
/*
    Calculation of maximal possible number of
    elements in the array by size of memory block,
    which must be not less then min_blocksize (0).
*/
    size_t max_size (size_t blocksize) const
    {
        return (blocksize / sizeof(T) -
                EXALLOC_EXTRA_ITEMS(T));
    }
/*
    STL methods for size management:
      size     () Number of elements.
      capacity () Number of elements, for which memory is allocated.
      max_size () Maximal possible number of elements.
      empty    () Predicate size() == 0.
      resize  (n) Set number of elements.
*/
    size_t  size     () const   { return len;                           }
    size_t  capacity () const;
    size_t  max_size () const   { return max_size (EXCALCBLOCKSIZE_MAX -
                                                   EXARRAY_ALIGN);      }
    int     empty    () const   { return (size() == 0);                 }
    void    resize   (size_t n);
/*
    Set maximal index of element.
*/
    void    confine  (size_t i);
/*
    Heuristic methods of size management, optimized for performance.

      adjust  (i) Change size of array to allocate memory for
                  element with the specified index or remove
                  elements with twice large index.

      expand  (i) Increase size of a array to allocate memory
                  for element with the specified index.

      shrink  (i) Reduce size of array to remove elements with
                  index twice large then specified.

    Methods expand() and shrink() are the simplified variants
    of universal method adjust(). Methods expand() and adjust()
    guarantee presence in the array of element with the specified
    index. The method shrink() keeps an element with the specified
    index, if memory for it is already allocated, otherwise this
    element remains to be not allocated. All methods activate actual
    allocation of memory only if it is required. Resulting number
    of elements is defined by size of allocated memory and can be
    more then requested.
*/
protected:
    void  adjust_c (size_t i);               /* Non-optimized adjust */
    void  adjust_c (size_t i, const T** p);  /* Non-optimized adjust */
    void  shrink_c (size_t i);               /* Non-optimized shrink */
public:
    void  adjust   (size_t i)
                { size_t l2 = len/2; if (l2 <= i - l2) adjust_c (i);    }
    void  adjust   (size_t i, const T** p)
                { size_t l2 = len/2; if (l2 <= i - l2) adjust_c (i,p);  }
    void  expand   (size_t i)               const
                { if (len <= i) ((exblock<T>*)this)->adjust_c (i);      }
    void  expand   (size_t i, const T** p)  const
                { if (len <= i) ((exblock<T>*)this)->adjust_c (i,p);    }
    void  shrink   (size_t i)
                { if (i < len/2) shrink_c (i);  }
/*
    Debugging methods of resizing:

      checkadjust (i) Analogue of adjust()
      checkexpand (i) Analogue of expand()
      checkshrink (i) Analogue of shrink()
      checkindex  (i) Check if i < len

    At presence of macro NCHECKPTR first three methods are almost
    identical to analogues, and the method checkindex() does
    nothing.

    At absence of macro NCHECKPTR all methods establish the maximal
    index in the array by calling confine(), If method checkindex()
    discovers that new number of elements is more then current, it
    invokes function assigned to ::exalloc_status.range_handler.
*/
#ifdef  NCHECKPTR
    void  checkadjust (size_t i) { adjust (i);  }
    void  checkexpand (size_t i) { expand (i);  }
    void  checkshrink (size_t i) { shrink (i);  }
    void  checkindex  (size_t i) { (void) (i);  }
#else /*NCHECKPTR*/
    void  checkadjust (size_t i) { confine (i); }
    void  checkexpand (size_t i) { confine (i); }
    void  checkshrink (size_t i) { confine (i); }
    void  checkindex  (size_t i) { if (len <= i) range (i);
                                   confine (i); }
#endif/*NCHECKPTR*/
/*
    Access to array without check of bounds.
      item (i)      Element.
      base ()       Pointer to the base array.
    Reference to element and pointer are valid
    until size of array is not changed.
*/
      T&  item   (size_t i)             { return e[i];          }
const T&  item   (size_t i)     const   { return e[i];          }
      T*  base   ()                     { return (e);           }
const T*  base   ()             const   { return (e);           }
/*
    Access to array with check of bounds:
      at   (i)      Element.
    On exiting of index outside of array bounds function
    assigned to ::exalloc_status.range_handler is called.
*/
#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  at     (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }
INDECL const T&  at     (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  at     (size_t i)
                { if (len <= (size_t)i) range (i); return e[i]; }
INDECL const T&  at     (size_t i) const
                { if (len <= (size_t)i) range (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/
/*
    Access to array with check of bounds and auto increasing of size.
      access (i)    Element.
    On exiting of index outsize of array bounds size of array is
    increased automatically. On increasing memory is cleared and
    then for allocated elements default constructors are called.

    If increasing of size is impossible due to either integer
    overflow or lack of memory error handling are performed,
    interrupting execution of program, as in function exmrealloc().
    NOTE:   negative index is treated as big positive number,
            which for all base types, except for char in 16-bit
            mode results in to error handling.

    On successful allocation of memory method returns reference
    to element for either obtaining or assigning of its value.
    Reference is valid until size of array is not changed.
*/
#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  access (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = grow (i); return p[i]; }
INDECL const T&  access (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = grow (i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  access (size_t i)
                { if (len <= (size_t)i) grow (i); return e[i]; }
INDECL const T&  access (size_t i) const
                { if (len <= (size_t)i) grow (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Definitions for STL. Array may be interpreted as
    container with restricted number of elements.
*/
typedef       T         value_type;     /* Type of element */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Iterators for STL.
      begin ()    Iterator of the first element.
      end   ()    Iterator of element following the last.
    If array is empty, begin() == end().
    It is prohibited to refer to element end().
    Iterator is implemented either as restricted pointer
    providing for check of bounds or conventional pointer
    if macro NCHECKPTR is defined. Iterators are valid as
    long as size of array is unchanged.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();                }
const_iterator  begin ()  const { return base();                }

iterator        end   ()        { return base()+size();         }
const_iterator  end   ()  const { return base()+size();         }

#else /*NCHECKPTR*/

iterator        begin ()        { return exptr<T>(e,len);       }
const_iterator  begin ()  const { return exptr<T>(e,len);       }

iterator        end   ()        { return exptr<T>(e,len,len);   }
const_iterator  end   ()  const { return exptr<T>(e,len,len);   }

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    STL-like operations on array:
      clear()   Clearing of array.
      swap(m)   Swap arrays without copying of elements.
*/
    void    clear     ()        { reallocate (0, 0);    }
    void    swap      (exblock<T>&);
/*
    Service methods:
      setbase_c (p) Assign base array.
      setsize_c (n) Assign number of elements.
    One may:
    1) Assign NULL pointer providing that number of
       elements is set to 0.
    2) Assign base array obtained through call of either
       exaligned_malloc or exaligned_mrealloc with alignment
       EXARRAY_ALIGN providing that size of block and number
       of elements fit to the following condition:
       blocksize >= min_blocksize (n).
    Is is not allowed to assign array obtained by operator new,
    because according to standard of C++ new uses non-compatible
    format of memory block for base types with non-trivial
    destructors.
*/
    void    setbase_c (T* p)        { e   = p;  }
    void    setsize_c (size_t n)    { len = n;  }
};
/*
    Protected method stub() returns pointer
    to static zero-initialized array containing
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) bytes.
*/
template <class T> T* exblock<T>::stub()
{
    static char _stub [sizeof(T) *
            (EXALLOC_EXTRA_ITEMS(T)? EXALLOC_EXTRA_ITEMS(T): 1)];
    return (T*)(_stub);
}
/*
    Protected method reallocate (n, blocksize) sets size
    of memory block in bytes to blocksize and number of
    elements to n. Size of block and number of elements
    must fit to condition blocksize >= min_blocksize (n),
    except for deleting of array when blocksize == n == 0.

    On increasing of number of elements allocated memory
    is cleared and then default constructors of elements
    are called. On increasing of number of elements
    destructors are called. Call of destructors are
    implemented in two variants: the first supports
    throwing of exceptions whereas the second does not.

*/
template <class T> void exblock<T>::reallocate (size_t n, size_t blocksize)
{
/*
    if (blocksize < min_blocksize (n)) abort();
*/
    T* p = e;               /* Work copy of e */

    if (p == 0)             /* Diagnostic of reference */
    {                       /* by dynamic NULL pointer */
        if (this == (exblock<T>*)&exnull_c)
            ::exrange (0);
    }
    else if (len == 0)
    {                       /* If array is empty, */
        p = 0;              /* clear p */
    }
    else
    {
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        for (;;)
        {
        try
        {
#endif
                            /* Call of destructors on */
        while (len > n)     /* deleting or shortening */
            { --len; delete (exreloc_c<T> *) (p + len); }
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        break;
        }
        catch (...) { };
        }
#endif
    }
    ::exaligned_mrealloc (  /* Memory allocation */
          (void**)&p,       /* Pointer to block of memory */
          blocksize,        /* New size */
          EXARRAY_ALIGN,    /* Alignment */
          sizeof(T) * len,  /* Range for zero-filling */
          sizeof(T) * (n + EXALLOC_EXTRA_ITEMS(T))
    );
    if ((e = p) == 0)       /* Storing of new e */
    {                       /* For empty array e */
        e = stub();         /* is set as stub */
    }
    else
    {                       /* Call of constructors on */
        while (len < n)     /* creating or enlarging */
            { new (p + len) exreloc_c<T>; ++len; }
    }
}
/*
    Protected method range() is called for
    either interrupting of program or returning
    of "safe" pointer to static area of memory.
*/
template <class T> T* exblock<T>::range() const
{
    exrange (e);
    return stub();
}
/*
    Protected method grow (i) is called
    when addressing to element i >= len
    for automating increasing of size.
    Before allocation function assigned to
    ::exalloc_status.grow_handler is called.
*/
template <class T> T* exblock<T>::grow (size_t i) const
{
    (*::exalloc_status.grow_handler)();
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    ((exblock<T>*)this)->reallocate (max_size (blocksize), blocksize);
    return (e);
}
/*
    Method capacity() returns number of
    elements for which memory is allocated.
    The number is always not less then size().
*/
template <class T> size_t exblock<T>::capacity() const
{
    return (max_size (::excalcblocksize (min_blocksize (len))));
}
/*
    Method resize (n) sets given number of elements
    and reserves memory for successive growing of array.
*/
template <class T> void exblock<T>::resize  (size_t n)
{
    reallocate (n, ::excalcblocksize (min_blocksize (n)));
}
/*
    Method confine (i) sets maximal index of element
    not less the required and reserves memory for
    successive growing of array.
*/
template <class T> void exblock<T>::confine (size_t i)
{
    reallocate (i + 1, ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T)))));
}
/*
    Method adjust_c (i) sets maximal index of element not
    less then required, reserving elements for successive
    growing of array.
*/
template <class T> void exblock<T>::adjust_c (size_t i)
{
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    reallocate (max_size (blocksize), blocksize);
}
/*
    Modification of adjust_c (i), allowing to correct
    pointer into array while array is moving in memory.
    The pointer is corrected if an only if it points
    into the array.
*/
template <class T> void exblock<T>::adjust_c (size_t i, const T** p)
{
    if (e <= *p && *p < e + size())
    {
       size_t k = (size_t) ((*p) - e); adjust_c (i); *p = e + k;
    }
    else adjust_c (i);
}
/*
    Method shrink_c (i) reduces maximal index of element
    to be not greater then required.
*/
template <class T> void exblock<T>::shrink_c (size_t i)
{
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    size_t n = max_size (blocksize);
    if (len > n) reallocate (n, blocksize);
}
/*
    Swapping of arrays without copying of elements.
*/
template <class T> void  exblock<T>::swap (exblock<T>& m)
{
    T*     temp_base = base();
    size_t temp_size = size();
    setbase_c   (m.base());
    setsize_c   (m.size());
    m.setbase_c (temp_base);
    m.setsize_c (temp_size);
}

/* --------------------------------------------------------------
    Template of array with variable number of elements,
    allowing addressing through dynamic pointer with
    automatic increasing of size.

    Template is intended for production of inherited classes.
    It is not possible to construct objects explace directly.
-------------------------------------------------------------- */

template <class T> class explace: public exblock<T>
{
protected:
/*
    Constructor and destructor are protected, as indirect
    creation of objects explace must be blocked.
    Constructor of inherited class must set e = stub(), len = 0,
    and then may call appropriate method for memory allocation.
    Destructor must call method clear().
*/
    explace ()  { /* e = stub(); len = 0; */    }
    ~explace()  { /* clear(); */                }

private:
/*
    To block assigning of arrays and their transfer
    to functions by value here are declared private
    copy constructor and assignment operator.
    One can pass arrays explace to function either
    by reference or to dynamic or restricted pointer.
*/
    explace                (const explace<T>&);
    explace<T>& operator = (const explace<T>&);
};

/* --------------------------------------------------------------
    Template of regulated array with check of bounds.
-------------------------------------------------------------- */

template <class T> class exvector: public exblock<T>
{
#ifdef  ALLOW_EXARRAY_USING
protected:
    using exblock<T>::e;
    using exblock<T>::len;
    using exblock<T>::stub;
public:
    using exblock<T>::clear;
    using exblock<T>::swap;
    using exblock<T>::base;
    using exblock<T>::at;
private:
#endif/*ALLOW_EXARRAY_USING*/
/*
    To block assigning of arrays and their transfer
    to functions by value here are declared private
    copy constructor and assignment operator.
    One can pass regulated arrays to function either
    by reference or to restricted or const T* pointer.
*/
    exvector                (const exvector<T>&);
    exvector<T>& operator = (const exvector<T>&);

public:
/*
    Constructor of regulated array.
    As e must not be 0, it gets pointer to static array of type T.
*/
    exvector () { e = stub(); len = 0; }
/*
    Constructor of regulated array with given number of elements.
*/
/*explicit*/ exvector (size_t n);
/*
    Destructor frees memory.
*/
    ~exvector() { clear(); }
/*
    Automatic conversion to const T* provides for
    transferring of base array to const T* parameter
    of function and for purposes of logic operations.
    Logic value of regulated array is true,
*/
    operator const T*()   const { return base();        }
/*
    Access to array with check of bounds.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return at (i); }
INDECL const T& operator [] (size_t i)         const { return at (i); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return at ((size_t)i); }
INDECL const T& operator [] (ptrdiff_t i)      const { return at ((size_t)i); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return at ((size_t)i); }
INDECL const T& operator [] (unsigned long i)  const { return at ((size_t)i); }
INDECL       T& operator [] (long i)                 { return at ((size_t)i); }
INDECL const T& operator [] (long i)           const { return at ((size_t)i); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return at (i); }
INDECL const T& operator [] (unsigned int i)   const { return at (i); }
INDECL       T& operator [] (int i)                  { return at (i); }
INDECL const T& operator [] (int i)            const { return at (i); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return at (i); }
INDECL const T& operator [] (unsigned short i) const { return at (i); }
INDECL       T& operator [] (short i)                { return at (i); }
INDECL const T& operator [] (short i)          const { return at (i); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return at (0); }
INDECL const T& operator *  ()                 const { return at (0); }
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(at (0)); }
INDECL const T* operator -> ()        const   { return (T*)&(at (0)); }
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Arithmetical operations.
*/
#ifdef  ALLOW_EXARRAY_ADDSUB

private:
exptr<T> _add (size_t i) const  { return (exptr<T>(*(exblock<T>*)this, i));}
exptr<T> _sub (size_t i) const  { return (exptr<T>(*(exblock<T>*)this,-i));}
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      exptr<T>  operator +  (size_t i)                { return (_add (i)); }
const exptr<T>  operator +  (size_t i)         const  { return (_add (i)); }
      exptr<T>  operator -  (size_t i)                { return (_sub (i)); }
const exptr<T>  operator -  (size_t i)         const  { return (_sub (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      exptr<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const exptr<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      exptr<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      exptr<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const exptr<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      exptr<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const exptr<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      exptr<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      exptr<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      exptr<T>  operator +  (unsigned int i)          { return (_add (i)); }
const exptr<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      exptr<T>  operator +  (int i)                   { return (_add (i)); }
const exptr<T>  operator +  (int i)            const  { return (_add (i)); }
      exptr<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const exptr<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      exptr<T>  operator -  (int i)                   { return (_sub (i)); }
const exptr<T>  operator -  (int i)            const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      exptr<T>  operator +  (unsigned short i)        { return (_add (i)); }
const exptr<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      exptr<T>  operator +  (short i)                 { return (_add (i)); }
const exptr<T>  operator +  (short i)          const  { return (_add (i)); }
      exptr<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const exptr<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      exptr<T>  operator -  (short i)                 { return (_sub (i)); }
const exptr<T>  operator -  (short i)          const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#else /*ALLOW_EXARRAY_ADDSUB*/
/*
    Blocking of arithmetical operations.
    Prevents conversion to const T* for successive
    operations on conventional pointers.
*/
private:

#ifdef  ALLOW_EXARRAY_SIZETYPE
void    operator +  (size_t i)                  ;
void    operator +  (size_t i)          const   ;
void    operator -  (size_t i)                  ;
void    operator -  (size_t i)          const   ;
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
void    operator +  (ptrdiff_t i)               ;
void    operator +  (ptrdiff_t i)       const   ;
void    operator -  (ptrdiff_t i)               ;
void    operator -  (ptrdiff_t i)       const   ;
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#endif/*ALLOW_EXARRAY_ADDSUB*/

private:
/*
    Block indexes to obtain comprehensive diagnostic.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Constructor of regulated array with given number of elements.
    Implementation takes into account possibility of throwing
    of exception on either memory allocation or constructing
    of elements of base array - in such case empty array is
    constructed, which requires no destruction.
*/
template <class T> exvector<T>::exvector (size_t n)
{
/*
    Creating of empty array in temporary object.
*/
    exvector<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    Allocation of memory and constructing of base array in
    work object. On throwing of exception constructed part
    of array will be deleted by destructor of base object.
*/
    tmp.resize (n);
/*
    Swap arrays of work object and object being constructed.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    Template of dynamic array with unlimited number of elements.
-------------------------------------------------------------- */

template <class T> class exarray: public explace<T>
{
#ifdef  ALLOW_EXARRAY_USING
protected:
    using exblock<T>::e;
    using exblock<T>::len;
    using exblock<T>::stub;
public:
    using exblock<T>::clear;
    using exblock<T>::swap;
    using exblock<T>::base;
    using exblock<T>::access;
private:
#endif/*ALLOW_EXARRAY_USING*/
/*
    To block assigning of arrays and their transfer
    to functions by value here are declared private
    copy constructor and assignment operator.
    One can pass dynamic arrays to function either
    by reference or to dynamic, restricted or
    const T* pointer.
*/
    exarray                (const exarray<T>&);
    exarray<T>& operator = (const exarray<T>&);

public:
/*
    Constructor of dynamic array.
    As e must not be 0, it gets pointer to static array of type T.
*/
    exarray ()  { e = stub(); len = 0; }
/*
    Constructor of dynamic array with given number of elements.
*/
/*explicit*/ exarray (size_t n);
/*
    Destructor frees memory.
*/
    ~exarray()  { clear(); }
/*
    Automatic conversion to const T* provides for
    transferring of base array to const T* parameter
    of function and for purposes of logic operations.
    Logic value of dynamic array is true,
*/
    operator const T*()   const { return base();        }
/*
    Access to array with check of bounds and auto increasing of size.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return access(i);}
INDECL const T& operator [] (size_t i)         const { return access(i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return access((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return access((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return access((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return access((size_t)i);}
INDECL       T& operator [] (long i)                 { return access((size_t)i);}
INDECL const T& operator [] (long i)           const { return access((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return access(i);}
INDECL const T& operator [] (unsigned int i)   const { return access(i);}
INDECL       T& operator [] (int i)                  { return access(i);}
INDECL const T& operator [] (int i)            const { return access(i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return access(i);}
INDECL const T& operator [] (unsigned short i) const { return access(i);}
INDECL       T& operator [] (short i)                { return access(i);}
INDECL const T& operator [] (short i)          const { return access(i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return access(0);}
INDECL const T& operator *  ()                 const { return access(0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(access(0));}
INDECL const T* operator -> ()        const   { return (T*)&(access(0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Arithmetical operations.
*/
#ifdef  ALLOW_EXARRAY_ADDSUB

private:
expoint<T> _add (size_t i) const{ return (expoint<T>(*(explace<T>*)this, i));}
expoint<T> _sub (size_t i) const{ return (expoint<T>(*(explace<T>*)this,-i));}
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      expoint<T>  operator +  (size_t i)                { return (_add (i)); }
const expoint<T>  operator +  (size_t i)         const  { return (_add (i)); }
      expoint<T>  operator -  (size_t i)                { return (_sub (i)); }
const expoint<T>  operator -  (size_t i)         const  { return (_sub (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      expoint<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const expoint<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      expoint<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const expoint<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      expoint<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const expoint<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      expoint<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      expoint<T>  operator +  (unsigned int i)          { return (_add (i)); }
const expoint<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      expoint<T>  operator +  (int i)                   { return (_add (i)); }
const expoint<T>  operator +  (int i)            const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const expoint<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      expoint<T>  operator -  (int i)                   { return (_sub (i)); }
const expoint<T>  operator -  (int i)            const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      expoint<T>  operator +  (unsigned short i)        { return (_add (i)); }
const expoint<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      expoint<T>  operator +  (short i)                 { return (_add (i)); }
const expoint<T>  operator +  (short i)          const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const expoint<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      expoint<T>  operator -  (short i)                 { return (_sub (i)); }
const expoint<T>  operator -  (short i)          const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#else /*ALLOW_EXARRAY_ADDSUB*/
/*
    Blocking of arithmetical operations.
    Prevents conversion to const T* for successive
    operations on conventional pointers.
*/
private:

#ifdef  ALLOW_EXARRAY_SIZETYPE
void    operator +  (size_t i)                  ;
void    operator +  (size_t i)          const   ;
void    operator -  (size_t i)                  ;
void    operator -  (size_t i)          const   ;
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
void    operator +  (ptrdiff_t i)               ;
void    operator +  (ptrdiff_t i)       const   ;
void    operator -  (ptrdiff_t i)               ;
void    operator -  (ptrdiff_t i)       const   ;
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#endif/*ALLOW_EXARRAY_ADDSUB*/

private:
/*
    Block indexes to obtain comprehensive diagnostic.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Constructor of dynamic array with given number of elements.
    Implementation takes into account possibility of throwing
    of exception on either memory allocation  or constructing
    of elements of base array - in such case empty array is
    constructed, which requires no destruction.
*/
template <class T> exarray<T>::exarray (size_t n)
{
/*
    Creating of empty array in temporary object.
*/
    exarray<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    Allocation of memory and constructing of base array in
    work object. On throwing of exception constructed part
    of array will be deleted by destructor of base object.
*/
    tmp.resize (n);
/*
    Swap arrays of work object and object being constructed.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    Template of dynamic pointer.

    Dynamic pointer contains offset from base of dynamic array,
    which may go outside of array bounds. Offset may be negative,
    which is equivalent to large positive offset with the same
    binary code.
-------------------------------------------------------------- */

template <class T> class expoint
{
protected:
    explace<T>* a;  /* Dynamic array */
    size_t      k;  /* Offset from array */
                    /* Subtract offset */
    size_t  lf  (size_t n)    const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }

public:
/*
    Constructor of dynamic pointer with initial value 0
    If no dynamic array is assigned to pointer, attempt
    to address through pointer causes call of function
    assigned to ::exalloc_status.null_handler.
*/
    expoint ()          { a = (explace<T>*)&exnull_c; k = 0;   }
/*
    Constructor for assigning of numeric value.
    As opposite to conventional pointer, dynamic pointer can
    get any numeric value without explicit cast. Regardless of
    numeric value, attempt to address through pointer causes
    call of function assigned to ::exalloc_status.null_handler.
*/
    expoint (size_t i)  { a = (explace<T>*)&exnull_c; k = i;   }
/*
    Constructor for assigning of dynamic array.
    After assignment addressing to elements of array with
    automatic increasing of size becomes possible.
*/
    expoint (const explace<T>&m, size_t i=0) { a = (explace<T>*)&m; k = i; }
/*
    Copying constructor and destructor are defined implicitly.
    Assign operations are defined explicitly to allow assignment
    to constant pointers.
*/
      expoint<T>& operator = (size_t i)
        { a = (explace<T>*)&exnull_c; k = i;
          return *this;                                     }

const expoint<T>& operator = (size_t i)             const
        { return *(expoint<T>*)this = i;                    }

      expoint<T>& operator = (explace<T>& m)
        { a = &m; k = 0; return *this;                      }

const expoint<T>& operator = (explace<T>& m)        const
        { return *(expoint<T>*)this = *(explace<T>*)&m;     }

const expoint<T>& operator = (const explace<T>& m)  const
        { return *(expoint<T>*)this = *(explace<T>*)&m;     }

#ifdef __TURBOC__

      expoint<T>& operator = (      expoint<T>& m)
            { a = m.a; k = m.k; return *this;               }

const expoint<T>& operator = (      expoint<T>& m)  const
            { return *(expoint<T>*)this = *(expoint<T>*)&m; }

const expoint<T>& operator = (const expoint<T>& m)  const
            { return *(expoint<T>*)this = *(expoint<T>*)&m; }

#else /*__TURBOC__*/

      expoint<T>& operator = (const expoint<T>& m)  const
        { (*(expoint<T>*)this).a = m.a;
          (*(expoint<T>*)this).k = m.k;
          return *(expoint<T>*)this;                        }

#endif/*__TURBOC__*/
/*
    Access to dynamic array.
*/
      explace<T>& place()               { return *a; }
const explace<T>& place()       const   { return *a; }
/*
    Allocation of memory exactly for given number of elmentns.
*/
    void  reallocate (size_t n)         { a->reallocate(exadd(n,k));}
/*
    STL methods for size management.
*/
    size_t  size     ()         const   { return lf(a->size());     }
    size_t  capacity ()         const   { return lf(a->capacity()); }
    size_t  max_size ()         const   { return lf(a->max_size()); }
    int     empty    ()         const   { return (size() == 0);     }
    void    resize   (size_t n)         { a->resize (exadd(n,k));   }
/*
    Set maximal index.
*/
    void    confine  (size_t i)         { a->confine (exadd(i,k));  }
/*
    Heuristics methods for resizing.
*/
    void    adjust   (size_t i)         { a->adjust (exadd(i,k));   }
    void    adjust   (size_t i, const T** p)
                                        { a->adjust (exadd(i,k),p); }
    void    expand   (size_t i) const   { a->expand (exadd(i,k));   }
    void    expand   (size_t i, const T** p)
                                const   { a->expand (exadd(i,k),p); }
    void    shrink   (size_t i)         { a->shrink (exadd(i,k));   }
/*
    Access to array without check of bounds.
    Reference to element and pointer are valid
    until size of array is not changed.
*/
      T&  item   (size_t i)             { return *(a->base()+k+i);  }
const T&  item   (size_t i)     const   { return *(a->base()+k+i);  }
      T*  base   ()                     { return  (a->base()+k);    }
const T*  base   ()             const   { return  (a->base()+k);    }
/*
    Access to array with check of bounds.
    On exiting of index outside of array bounds function
    assigned to ::exalloc_status.range_handler is called.
*/
INDECL       T&  at     (size_t i)      { return (a->at (k+i));     }
INDECL const T&  at     (size_t i) const{ return (
                                 ((const explace<T>*)a)->at (k+i)); }
/*
    Access to array with check of index and auto increasing of size.
    On exiting of index outsize of array bounds size of array is
    increased automatically. On increasing memory is cleared and
    then for allocated elements default constructors are called.
    On successful allocation of memory method returns reference
    to element for either obtaining or assigning of its value.
    Reference is valid until size of array is not changed.
*/
INDECL       T&  access (size_t i)      { return (a->access (k+i));     }
INDECL const T&  access (size_t i) const{ return (
                                 ((const explace<T>*)a)->access (k+i)); }

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Definitions for STL. Dynamic pointer may be interpreted
    as container with restricted number of elements and as
    full-featured random access iterator.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* Category of iterator */
typedef       T         value_type;     /* Type of element */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Iterators for STL.
      begin ()    Iterator of the first element.
      end   ()    Iterator of element following the last.
    If pointer is outside of array, begin() == end().
    It is prohibited to refer to element end().
    Iterator is implemented either as restricted pointer
    providing for check of bounds or conventional pointer
    if macro NCHECKPTR is defined. Iterators are valid as
    long as size of array is unchanged.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();                }
const_iterator  begin ()  const { return base();                }

iterator        end   ()        { return base()+size();         }
const_iterator  end   ()  const { return base()+size();         }

#else /*NCHECKPTR*/

iterator        begin ()        { return exptr<T>(*a,k);        }
const_iterator  begin ()  const { return exptr<T>(*a,k);        }

iterator        end   ()        { return exptr<T>(*a,k+size()); }
const_iterator  end   ()  const { return exptr<T>(*a,k+size()); }

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    Automatic conversion to const T* provides for
    transferring of base array to const T* parameter
    of function and for purposes of logic operations.
    Logic value of dynamic array is true if pointer
    contains reference to dynamic array or non-null
    numeric value, otherwise false.
    Of pointer goes outsize of array bounds,
    no expanding of array is carried out.
*/
    operator const T*()   const { return base();        }
/*
    Access to array with check of bounds and auto increasing of size.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return access(i);}
INDECL const T& operator [] (size_t i)         const { return access(i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return access((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return access((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return access((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return access((size_t)i);}
INDECL       T& operator [] (long i)                 { return access((size_t)i);}
INDECL const T& operator [] (long i)           const { return access((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return access(i);}
INDECL const T& operator [] (unsigned int i)   const { return access(i);}
INDECL       T& operator [] (int i)                  { return access(i);}
INDECL const T& operator [] (int i)            const { return access(i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return access(i);}
INDECL const T& operator [] (unsigned short i) const { return access(i);}
INDECL       T& operator [] (short i)                { return access(i);}
INDECL const T& operator [] (short i)          const { return access(i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return access(0);}
INDECL const T& operator *  ()                 const { return access(0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(access(0));}
INDECL const T* operator -> ()        const   { return (T*)&(access(0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Arithmetical operations.
*/
private:

expoint<T> _add (size_t i) const{ return (expoint<T> (*(explace<T>*)a, k+i));}
expoint<T> _sub (size_t i) const{ return (expoint<T> (*(explace<T>*)a, k-i));}

expoint<T>& _ad (size_t i) const
           { ((expoint<T>*)this)->k+=i; return (*(expoint<T>*)this);         }
expoint<T>& _sb (size_t i) const
           { ((expoint<T>*)this)->k-=i; return (*(expoint<T>*)this);         }
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      expoint<T>  operator +  (size_t i)                { return (_add (i)); }
const expoint<T>  operator +  (size_t i)         const  { return (_add (i)); }
      expoint<T>  operator -  (size_t i)                { return (_sub (i)); }
const expoint<T>  operator -  (size_t i)         const  { return (_sub (i)); }
      expoint<T>& operator += (size_t i)                { return (_ad  (i)); }
const expoint<T>& operator += (size_t i)         const  { return (_ad  (i)); }
      expoint<T>& operator -= (size_t i)                { return (_sb  (i)); }
const expoint<T>& operator -= (size_t i)         const  { return (_sb  (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      expoint<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const expoint<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
      expoint<T>& operator += (ptrdiff_t i)             { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (ptrdiff_t i)      const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator -= (ptrdiff_t i)             { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (ptrdiff_t i)      const  { return (_sb  ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      expoint<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const expoint<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      expoint<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const expoint<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      expoint<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
      expoint<T>& operator += (unsigned long i)         { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (unsigned long i)  const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator += (long i)                  { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (long i)           const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator -= (unsigned long i)         { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (unsigned long i)  const  { return (_sb  ((size_t)i)); }
      expoint<T>& operator -= (long i)                  { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (long i)           const  { return (_sb  ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      expoint<T>  operator +  (unsigned int i)          { return (_add (i)); }
const expoint<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      expoint<T>  operator +  (int i)                   { return (_add (i)); }
const expoint<T>  operator +  (int i)            const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const expoint<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      expoint<T>  operator -  (int i)                   { return (_sub (i)); }
const expoint<T>  operator -  (int i)            const  { return (_sub (i)); }
      expoint<T>& operator += (unsigned int i)          { return (_ad  (i)); }
const expoint<T>& operator += (unsigned int i)   const  { return (_ad  (i)); }
      expoint<T>& operator += (int i)                   { return (_ad  (i)); }
const expoint<T>& operator += (int i)            const  { return (_ad  (i)); }
      expoint<T>& operator -= (unsigned int i)          { return (_sb  (i)); }
const expoint<T>& operator -= (unsigned int i)   const  { return (_sb  (i)); }
      expoint<T>& operator -= (int i)                   { return (_sb  (i)); }
const expoint<T>& operator -= (int i)            const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      expoint<T>  operator +  (unsigned short i)        { return (_add (i)); }
const expoint<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      expoint<T>  operator +  (short i)                 { return (_add (i)); }
const expoint<T>  operator +  (short i)          const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const expoint<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      expoint<T>  operator -  (short i)                 { return (_sub (i)); }
const expoint<T>  operator -  (short i)          const  { return (_sub (i)); }
      expoint<T>& operator += (unsigned short i)        { return (_ad  (i)); }
const expoint<T>& operator += (unsigned short i) const  { return (_ad  (i)); }
      expoint<T>& operator += (short i)                 { return (_ad  (i)); }
const expoint<T>& operator += (short i)          const  { return (_ad  (i)); }
      expoint<T>& operator -= (unsigned short i)        { return (_sb  (i)); }
const expoint<T>& operator -= (unsigned short i) const  { return (_sb  (i)); }
      expoint<T>& operator -= (short i)                 { return (_sb  (i)); }
const expoint<T>& operator -= (short i)          const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

      expoint<T>& operator ++ ()          { k++; return (*this);             }
const expoint<T>& operator ++ ()    const { return ++(*((expoint<T>*)this)); }

      expoint<T>& operator -- ()          { k--; return (*this);             }
const expoint<T>& operator -- ()    const { return --(*((expoint<T>*)this)); }

      expoint<T>  operator ++ (int)       { expoint<T>t=*this; k++; return t;}
      expoint<T>  operator ++ (int) const { return (*((expoint<T>*)this))++; }

      expoint<T>  operator -- (int)       { expoint<T>t=*this; k--; return t;}
      expoint<T>  operator -- (int) const { return (*((expoint<T>*)this))--; }

private:
/*
    Block indexes to obtain comprehensive diagnostic.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
void    operator += (unsigned long i)           ;
void    operator += (unsigned long i)   const   ;
void    operator += (long i)                    ;
void    operator += (long i)            const   ;
void    operator -= (unsigned long i)           ;
void    operator -= (unsigned long i)   const   ;
void    operator -= (long i)                    ;
void    operator -= (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
void    operator += (unsigned int i)            ;
void    operator += (unsigned int i)    const   ;
void    operator += (int i)                     ;
void    operator += (int i)             const   ;
void    operator -= (unsigned int i)            ;
void    operator -= (unsigned int i)    const   ;
void    operator -= (int i)                     ;
void    operator -= (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
void    operator += (unsigned short i)          ;
void    operator += (unsigned short i)  const   ;
void    operator += (short i)                   ;
void    operator += (short i)           const   ;
void    operator -= (unsigned short i)          ;
void    operator -= (unsigned short i)  const   ;
void    operator -= (short i)                   ;
void    operator -= (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
void    operator += (unsigned char i)           ;
void    operator += (unsigned char i)   const   ;
void    operator += (signed char i)             ;
void    operator += (signed char i)     const   ;
void    operator -= (unsigned char i)           ;
void    operator -= (unsigned char i)   const   ;
void    operator -= (signed char i)             ;
void    operator -= (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
void    operator += (char i)                    ;
void    operator += (char i)            const   ;
void    operator -= (char i)                    ;
void    operator -= (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};

/* --------------------------------------------------------------
    Template of restricted pointers with check of bounds.

    Restricted pointer contains base address and size
    of array and also offset from base address, which may
    go outside of array bounds. Offset may be negative,
    which is equivalent to large positive offset with
    the same binary code.
-------------------------------------------------------------- */

template <class T> class exptr
{
protected:
    T*      e;      /* Base array */
    size_t  len;    /* Number of elements */
    size_t  k;      /* Offset in array */
    T*      x;      /* Pointer for derived classes (here NULL) */
    size_t  lf  (size_t n)      const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }
/*
    Constructor 1:1.
*/
    exptr (const T* me, size_t mlen, size_t mk, const T* mx)
            { e = (T*)me; len = mlen; k = mk; x = (T*)mx;       }
/*
    Either interrupting of program or returning
    of "safe" pointer to static area of memory.
*/
      T*  range  ()             const;
      T*  range  (size_t i)             { return range()-i;     }
const T*  range  (size_t i)     const   { return range()-i;     }

public:
/*
    Check of size to obtain conventional pointer.
*/
protected:
      T*  check_ (size_t n)     const;
public:
      T*  check  (size_t n)             { return check_(n);     }
const T*  check  (size_t n)     const   { return check_(n);     }
/*
    Check of index range to obtain conventional pointer.
*/
protected:
      T*  index_ (size_t i)     const;
public:
      T*  index  (size_t i)             { return index_(i);     }
const T*  index  (size_t i)     const   { return index_(i);     }
/*
    Constructor of restricted pointer with initial value 0
    If no array is assigned to pointer, attempt
    to address through pointer causes call of function
    assigned to ::exalloc_status.null_handler.
*/
    exptr ()          { e = NULL; len = 0; k = 0; x = NULL;     }
/*
    Constructor for assigning of conventional pointer.
    If initial pointer is NULL then on addressing
    through restricted pointer function assigned to
    ::exalloc_status.null_handler is called.
    If initial pointer is not NULL then after assignment
    one can address element 0, for other indexes function
    assigned to ::exalloc_status.range_handler is called.
    Logical value of restricted pointer is same as for
    initial pointer.
*/
    exptr (const T* p)
            { e = (T*)p; len = (e != NULL); k = 0; x = NULL;    }
/*
    Constructor for assigning of conventional array of
    given length. If initial pointer is NULL then
    on addressing through restricted pointer function
    assigned to ::exalloc_status.null_handler is called.
    If initial pointer is not NULL then after assignment
    it is possible to address elements of array with check
    of bounds. On addressing outside of array bounds function
    assigned to ::exalloc_status.range_handler is called.
    Logical value of restricted pointer is same as for
    initial pointer.
*/
    exptr (const T* m, size_t n, size_t i = 0)
            { if ((e = (T*)m) == NULL) n = 0; len = n; k = i; x = NULL; }
/*
    Constructors for assigning of dynamic array and pointer.
    Size of array must no be changed later.
    On addressing outside of array bounds function assigned
    to ::exalloc_status.range_handler is called.
*/
    exptr (const exblock<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
    exptr (const expoint<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
/*
    Constructor for extracting of sub-array with check of bounds.
*/
    exptr (const exptr<T>&m, size_t n, size_t i = 0)
            { e = (T*)m.check(n); len = n; k = i; x = m.x;          }
/*
    Copying constructor and destructor are defined implicitly.
    Assign operations are defined explicitly to allow assignment
    to constant pointers.
*/
      exptr<T>& operator = (T* p)
            { e = p; len = (e != NULL); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (const T* p)             const
            { return *(exptr<T>*)this = (T*)p;              }

      exptr<T>& operator = (exblock<T>& m)
            { e = m.base(); len = m.size(); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (exblock<T>& m)          const
            { return *(exptr<T>*)this = *(exblock<T>*)&m;   }

const exptr<T>& operator = (const exblock<T>& m)    const
            { return *(exptr<T>*)this = *(exblock<T>*)&m;   }

      exptr<T>& operator = (expoint<T>& m)
            { e = m.base(); len = m.size(); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (expoint<T>& m)          const
            { return *(exptr<T>*)this = *(expoint<T>*)&m;   }

const exptr<T>& operator = (const expoint<T>& m)    const
            { return *(exptr<T>*)this = *(expoint<T>*)&m;   }

#ifdef  __TURBOC__

      exptr<T>& operator = (exptr<T>& m)
            { e = m.e; len = m.len; k = m.k; x = m.x;
              return *this;                                 }

const exptr<T>& operator = (exptr<T>& m)            const
            { return *(exptr<T>*)this = *(exptr<T>*)&m;     }

const exptr<T>& operator = (const exptr<T>& m)      const
            { return *(exptr<T>*)this = *(exptr<T>*)&m;     }

#else /*__TURBOC__*/

      exptr<T>& operator = (const exptr<T>& m)      const
            { (*(exptr<T>*)this).e   = m.e;
              (*(exptr<T>*)this).len = m.len;
              (*(exptr<T>*)this).k   = m.k;
              (*(exptr<T>*)this).x   = m.x;
              return *(exptr<T>*)this;                      }

#endif/*__TURBOC__*/
/*
    Methods for allocating and freeing of memory with
    calling of constructors and destructors.
*/
    exptr<T>& allocate (size_t n)
        { e = EXNEW (T, n); len = n; return *this; }

    void deallocate ()
        { EXDELETE (e); e = NULL; len = 0; }
/*
    Methods for allocating and freeing of memory without
    calling of constructors and destructors.
*/
    exptr<T>& alloc (size_t n)
        { e = EXALLOC (T, n); len = n; return *this; }

    void free ()
        { EXFREE (e); e = NULL; len = 0; }

    exptr<T>& aligned_alloc (size_t n, size_t blockalign)
        { e = EXALIGNED_ALLOC (T, n, blockalign); len = n; return *this; }

    void aligned_free ()
        { EXALIGNED_FREE (e); e = NULL; len = 0; }
/*
    Access to array without check of bounds.
*/
      T&  item   (size_t i)             { return *(e+k+i);      }
const T&  item   (size_t i)     const   { return *(e+k+i);      }
      T*  base   ()                     { return  (e+k);        }
const T*  base   ()             const   { return  (e+k);        }
/*
    Access to array with check of bounds.
    On exiting of index outside of array bounds function
    assigned to ::exalloc_status.range_handler is called.
*/
private:

#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  at_  (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }
INDECL const T&  at_  (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  at_  (size_t i)
                { if (len <= (size_t)i) range (i); return e[i]; }
INDECL const T&  at_  (size_t i) const
                { if (len <= (size_t)i) range (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/

public:

INDECL       T&  at   (size_t i)        { return at_(k+i);  }
INDECL const T&  at   (size_t i) const  { return at_(k+i);  }

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Definitions for STL. Restricted pointer may be interpreted
    as container with restricted number of elements and as
    full-featured random access iterator.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* Category of iterator */
typedef       T         value_type;     /* Type of element */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Iterators for STL.
      begin ()    Iterator of the first element.
      end   ()    Iterator of element following the last.
    If pointer is outside of array, begin() == end().
    It is prohibited to refer to element end().
    Iterator is implemented either as restricted pointer
    providing for check of bounds or conventional pointer
    if macro NCHECKPTR is defined.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();        }
const_iterator  begin ()  const { return base();        }

iterator        end   ()        { return base()+size(); }
const_iterator  end   ()  const { return base()+size(); }

#else /*NCHECKPTR*/

iterator        begin ()        { return *this;         }
const_iterator  begin ()  const { return *this;         }

iterator        end   ()        { return exptr<T>(e,k,(len<(size_t)k?k:len));}
const_iterator  end   ()  const { return exptr<T>(e,k,(len<(size_t)k?k:len));}

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    Automatic conversion to const T* provides for
    transferring of base array to const T* parameter
    of function and for purposes of logic operations.
    Logic value of dynamic array is true if pointer
    contains reference to array or non-null numeric
    value, otherwise false.
*/
    operator const T*()     const { return base();      }
/*
    Access to array with check of bounds.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return at (i);}
INDECL const T& operator [] (size_t i)         const { return at (i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return at ((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return at ((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return at ((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return at ((size_t)i);}
INDECL       T& operator [] (long i)                 { return at ((size_t)i);}
INDECL const T& operator [] (long i)           const { return at ((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return at (i);}
INDECL const T& operator [] (unsigned int i)   const { return at (i);}
INDECL       T& operator [] (int i)                  { return at (i);}
INDECL const T& operator [] (int i)            const { return at (i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return at (i);}
INDECL const T& operator [] (unsigned short i) const { return at (i);}
INDECL       T& operator [] (short i)                { return at (i);}
INDECL const T& operator [] (short i)          const { return at (i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return at (0);}
INDECL const T& operator *  ()                 const { return at (0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(at (0));}
INDECL const T* operator -> ()         const  { return (T*)&(at (0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Arithmetical operations.
*/
private:

exptr<T>   _add (size_t i) const    { return exptr<T>(e,len,k+i,x);     }
exptr<T>   _sub (size_t i) const    { return exptr<T>(e,len,k-i,x);     }

exptr<T>&  _ad  (size_t i) const
                { ((exptr<T>*)this)->k+=i; return (*(exptr<T>*)this);   }
exptr<T>&  _sb  (size_t i) const
                { ((exptr<T>*)this)->k-=i; return (*(exptr<T>*)this);   }
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      exptr<T>    operator +  (size_t i)                { return (_add (i)); }
const exptr<T>    operator +  (size_t i)         const  { return (_add (i)); }
      exptr<T>    operator -  (size_t i)                { return (_sub (i)); }
const exptr<T>    operator -  (size_t i)         const  { return (_sub (i)); }
      exptr<T>&   operator += (size_t i)                { return (_ad  (i)); }
const exptr<T>&   operator += (size_t i)         const  { return (_ad  (i)); }
      exptr<T>&   operator -= (size_t i)                { return (_sb  (i)); }
const exptr<T>&   operator -= (size_t i)         const  { return (_sb  (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      exptr<T>    operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const exptr<T>    operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      exptr<T>    operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
      exptr<T>&   operator += (ptrdiff_t i)             { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (ptrdiff_t i)      const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator -= (ptrdiff_t i)             { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (ptrdiff_t i)      const  { return (_sb  ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      exptr<T>    operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const exptr<T>    operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      exptr<T>    operator +  (long i)                  { return (_add ((size_t)i)); }
const exptr<T>    operator +  (long i)           const  { return (_add ((size_t)i)); }
      exptr<T>    operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      exptr<T>    operator -  (long i)                  { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (long i)           const  { return (_sub ((size_t)i)); }
      exptr<T>&   operator += (unsigned long i)         { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (unsigned long i)  const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator += (long i)                  { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (long i)           const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator -= (unsigned long i)         { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (unsigned long i)  const  { return (_sb  ((size_t)i)); }
      exptr<T>&   operator -= (long i)                  { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (long i)           const  { return (_sb  ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      exptr<T>    operator +  (unsigned int i)          { return (_add (i)); }
const exptr<T>    operator +  (unsigned int i)   const  { return (_add (i)); }
      exptr<T>    operator +  (int i)                   { return (_add (i)); }
const exptr<T>    operator +  (int i)            const  { return (_add (i)); }
      exptr<T>    operator -  (unsigned int i)          { return (_sub (i)); }
const exptr<T>    operator -  (unsigned int i)   const  { return (_sub (i)); }
      exptr<T>    operator -  (int i)                   { return (_sub (i)); }
const exptr<T>    operator -  (int i)            const  { return (_sub (i)); }
      exptr<T>&   operator += (unsigned int i)          { return (_ad  (i)); }
const exptr<T>&   operator += (unsigned int i)   const  { return (_ad  (i)); }
      exptr<T>&   operator += (int i)                   { return (_ad  (i)); }
const exptr<T>&   operator += (int i)            const  { return (_ad  (i)); }
      exptr<T>&   operator -= (unsigned int i)          { return (_sb  (i)); }
const exptr<T>&   operator -= (unsigned int i)   const  { return (_sb  (i)); }
      exptr<T>&   operator -= (int i)                   { return (_sb  (i)); }
const exptr<T>&   operator -= (int i)            const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      exptr<T>    operator +  (unsigned short i)        { return (_add (i)); }
const exptr<T>    operator +  (unsigned short i) const  { return (_add (i)); }
      exptr<T>    operator +  (short i)                 { return (_add (i)); }
const exptr<T>    operator +  (short i)          const  { return (_add (i)); }
      exptr<T>    operator -  (unsigned short i)        { return (_sub (i)); }
const exptr<T>    operator -  (unsigned short i) const  { return (_sub (i)); }
      exptr<T>    operator -  (short i)                 { return (_sub (i)); }
const exptr<T>    operator -  (short i)          const  { return (_sub (i)); }
      exptr<T>&   operator += (unsigned short i)        { return (_ad  (i)); }
const exptr<T>&   operator += (unsigned short i) const  { return (_ad  (i)); }
      exptr<T>&   operator += (short i)                 { return (_ad  (i)); }
const exptr<T>&   operator += (short i)          const  { return (_ad  (i)); }
      exptr<T>&   operator -= (unsigned short i)        { return (_sb  (i)); }
const exptr<T>&   operator -= (unsigned short i) const  { return (_sb  (i)); }
      exptr<T>&   operator -= (short i)                 { return (_sb  (i)); }
const exptr<T>&   operator -= (short i)          const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

      exptr<T>&   operator ++ ()          { k++; return (*this);             }
const exptr<T>&   operator ++ ()    const { return ++(*((exptr<T>*)this));   }

      exptr<T>&   operator -- ()          { k--; return (*this);             }
const exptr<T>&   operator -- ()    const { return --(*((exptr<T>*)this));   }

      exptr<T>    operator ++ (int)       { exptr<T>t=*this; k++; return t;  }
      exptr<T>    operator ++ (int) const { return (*((exptr<T>*)this))++;   }

      exptr<T>    operator -- (int)       { exptr<T>t=*this; k--; return t;  }
      exptr<T>    operator -- (int) const { return (*((exptr<T>*)this))--;   }

private:
/*
    Block indexes to obtain comprehensive diagnostic.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
void    operator += (unsigned long i)           ;
void    operator += (unsigned long i)   const   ;
void    operator += (long i)                    ;
void    operator += (long i)            const   ;
void    operator -= (unsigned long i)           ;
void    operator -= (unsigned long i)   const   ;
void    operator -= (long i)                    ;
void    operator -= (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
void    operator += (unsigned int i)            ;
void    operator += (unsigned int i)    const   ;
void    operator += (int i)                     ;
void    operator += (int i)             const   ;
void    operator -= (unsigned int i)            ;
void    operator -= (unsigned int i)    const   ;
void    operator -= (int i)                     ;
void    operator -= (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
void    operator += (unsigned short i)          ;
void    operator += (unsigned short i)  const   ;
void    operator += (short i)                   ;
void    operator += (short i)           const   ;
void    operator -= (unsigned short i)          ;
void    operator -= (unsigned short i)  const   ;
void    operator -= (short i)                   ;
void    operator -= (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
void    operator += (unsigned char i)           ;
void    operator += (unsigned char i)   const   ;
void    operator += (signed char i)             ;
void    operator += (signed char i)     const   ;
void    operator -= (unsigned char i)           ;
void    operator -= (unsigned char i)   const   ;
void    operator -= (signed char i)             ;
void    operator -= (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
void    operator += (char i)                    ;
void    operator += (char i)            const   ;
void    operator -= (char i)                    ;
void    operator -= (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Protected method range() is called for
    either interrupting of program or returning
    of "safe" pointer to static area of memory.
*/
template <class T> T* exptr<T>::range() const
{
    exrange (e);
    static char _stub [sizeof(T)];
    return (T*)(_stub);
}
/*
    Check of size to obtain conventional pointer.
*/
template <class T> T* exptr<T>::check_ (size_t n) const
{
    if ((len <= (size_t)k || len - (size_t)k < n) && n > 0) range();
    return (e + k);
}
/*
    Check of index range to obtain conventional pointer.
*/
template <class T> T* exptr<T>::index_ (size_t i) const
{
    if (len <= (size_t)k || len - (size_t)k <= i) range();
    return (e + k);
}

/* --------------------------------------------------------------
    Compare operators.
-------------------------------------------------------------- */

template <class T> inline int   operator == (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exvector<T>& m) { return (t.base() >= m.base()); }

/* --------------------------------------------------------------
    Subtract operators.
-------------------------------------------------------------- */

template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exvector<T>& m) { return (t.base() - m.base()); }

/* --------------------------------------------------------------
    Template of stack of arrays with variable length.

    Template describes stack, from where one can obtain
    (get) array exblock<T> and where one can return
    (put) array exblock<T> if it is no more necessary.
    One can use methods get and put in constructors and
    destructors of object, derived from template exblock.
    These methods works faster then conventional methods
    of memory allocation.
-------------------------------------------------------------- */

template <class T> class exblockstack
{
    exarray<exblock_t> stack;   /* Stack of arrays exblock_t<T> */
    size_t n;                   /* Number of arrays */
/*
    To block assigning of stacks here are declared private
    copy constructor and assignment operator.
*/
    exblockstack                (const exblockstack<T>&);
    exblockstack<T>& operator = (const exblockstack<T>&);

public:
/*
    Constructor creates empty stack.
*/
    exblockstack()       { n = 0;    }  /* Constructor */

    void get (exblock<T>&);             /* Get array */
    void put (exblock<T>&);             /* Put array */
    size_t size() const { return n; }   /* Number of arrays */

   ~exblockstack();                     /* Destructor */
};
/*
    Method get extracts array exblock<T> from stack.
    If stack is empty method creates empty array.
*/
template <class T> void exblockstack<T>::get (exblock<T>& array)
{
    if (n)                              /* Stack is not empty: */
    {                                   /* get array */
        exblock<T>* p = (exblock<T>*) & (stack.item (--n));
        array.setbase_c (p->base()); p->setbase_c (NULL);
        array.setsize_c (p->size()); p->setsize_c (0);
    }
    else                                /* Stack is empty: */
    {                                   /* create array */
        array.setbase_c (NULL);
        array.setsize_c (0);
    }
}
/*
    Method put pushes array into stack.
*/
template <class T> void exblockstack<T>::put (exblock<T>& array)
{
    stack.expand (n);
    exblock<T>* p = (exblock<T>*) & (stack.item (n++));
    p->setbase_c (array.base()); array.setbase_c (NULL);
    p->setsize_c (array.size()); array.setsize_c (0);
}
/*
    Destructor deletes all arrays contained in stack.
*/
template <class T> exblockstack<T>::~exblockstack()
{
    while (n) ((exblock<T>*) & (stack.item (--n)))->clear();
}

#endif/*__cplusplus*/

#endif/*EXARRAY_H*/
