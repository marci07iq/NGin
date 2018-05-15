/* --------------------------------------------------------------
    Dynamic arrays with check of bounds (version 1.2c).
    Memory allocation functions.

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
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#define EXALLOC_OUT_OF_LINE
#include "Exarray.h"

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

/* --------------------------------------------------------------
    Check if compiler supports alignment.
-------------------------------------------------------------- */

#ifndef _mm_malloc
#define _aligned_malloc(p,b,a)  malloc(b)
#define _aligned_realloc(p,b,a) realloc(p,b)
#define _aligned_free(p)        free(p)
#endif/*_mm_malloc*/

/* --------------------------------------------------------------
    External references are declared as "C", in order
    to block name mangling. This facilitates porting of
    object modules among different compilers.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Stub of dynamic array for assignment
    of NULL values to dynamic pointers.
-------------------------------------------------------------- */

struct  exblock_t VDECL exnull_c;

/* --------------------------------------------------------------
    Function set_exalloc_handler() sets handler of memory
    allocation errors, similar to set_new_handler().

    Parameters:
    pvf     Either pointer to function of type
            void name (void) or NULL if memory
            allocation errors are to be handled
            by (*exalloc_status.size_handler)();

    Returns:    The previous handler.
-------------------------------------------------------------- */

void (* set_exalloc_handler (void (*pvf) (void))) (void)
{
    static void (* pvfFailHandler)(void) = NULL;
    void (*pvfPrev) (void) = pvfFailHandler;
    pvfFailHandler = pvf;
    return pvfPrev;
}

/* --------------------------------------------------------------
    Function exalloc_size_fail() invokes memory allocation
    handler until is is not NULL, then calls abort().
-------------------------------------------------------------- */

void exalloc_size_fail (void)
{
    for (;;)
    {
        void (*pvf) (void) = set_exalloc_handler (NULL);
        set_exalloc_handler (pvf);
        if (pvf == NULL) break;
        (*pvf)();
    }
    abort();
}

/* --------------------------------------------------------------
    Function exalloc_stub() does nothing.
-------------------------------------------------------------- */

void exalloc_stub (void) {}

/* --------------------------------------------------------------
    Function exalloc_null_fail() calls abort.
-------------------------------------------------------------- */

static void exalloc_null_fail (void)
{
    #define NULL_POINTER_INDIRECTION 0
    assert (NULL_POINTER_INDIRECTION);
    abort();
}

/* --------------------------------------------------------------
    Function exalloc_range_fail() calls abort.
-------------------------------------------------------------- */

static void exalloc_range_fail (void)
{
    #define INDEX_RANGE_ERROR 0
    assert (INDEX_RANGE_ERROR);
    abort();
}

/* --------------------------------------------------------------
    Error handling pointers and information concerning
    with allocation of memory.

    Pointer to function that handles unrecoverable memory
    allocation errors. The function must interrupt execution.

    Pointer to function that handles automatic expansion of
    dynamic array.

    Pointer to function that handles NULL pointer indirection.
    The function must interrupt execution.

    Pointer to function that handles index range error, if
    automatic expansion of dynamic array is not provided.
    The function must interrupt execution.
-------------------------------------------------------------- */

struct exalloc_status_t VDECL exalloc_status =
    { exalloc_size_fail, exalloc_stub,
      exalloc_null_fail, exalloc_range_fail,
      0, 0, 0, 0 };

/* --------------------------------------------------------------
    Function exmalloc() allocates and clears block of memory.
    On failure the function calls memory allocation handler.

    Parameters:
    blocksize       Size of block in bytes.

    Returns:        Either pointer to allocated block or NULL.
-------------------------------------------------------------- */

void*   FCDECL  exmalloc (size_t blocksize)
{
    void* p = NULL;
    exaligned_mrealloc (&p, blocksize, 1, 0, blocksize);
    return (p);
}

/* --------------------------------------------------------------
    Function exaligned_malloc() allocates and clears aligned
    block of memory.
    On failure the function calls memory allocation handler.

    Parameters:
    blocksize       Size of block in bytes.
    blockalign      Alignment of block in bytes (2**n).

    Returns:        Either pointer to allocated block or NULL.
-------------------------------------------------------------- */

void*   FCDECL  exaligned_malloc (size_t blocksize, size_t blockalign)
{
    void* p = NULL;
    exaligned_mrealloc (&p, blocksize, blockalign, 0, blocksize);
    return (p);
}

/* --------------------------------------------------------------
    Function exmrealloc() allocates, reallocates, clears and
    frees block of memory.
    On failure the function calls memory allocation handler.

    Parameters:
    p               Address of pointer to block.
                    *p == NULL if block is allocated,
                    *p != NULL if block is reallocated.
                    Value of *p is modified after
                    reallocation.
    blocksize       New size of block in bytes or 0
                    if block must be freed.
    memset_start    First byte of zero-filling area.
    memset_stop     First byte behind zero-filling area.
-------------------------------------------------------------- */

void    FCDECL  exmrealloc (void** p, size_t blocksize,
                 size_t memset_start, size_t memset_stop)
{
    exaligned_mrealloc (p, blocksize, 1, memset_start, memset_stop);
}

/* --------------------------------------------------------------
    Function exaligned_mrealloc() allocates, reallocates,
    clears and frees aligned block of memory.
    On failure the function calls memory allocation handler.

    Parameters:
    p               Address of pointer to block.
                    *p == NULL if block is allocated,
                    *p != NULL if block is reallocated.
                    Value of *p is to be modified.
    blocksize       New size of block in bytes or 0
                    if block must be freed.
    blockalign      Alignment of block in bytes (2**n).
    memset_start    First byte of zero-filling area.
    memset_stop     First byte behind zero-filling area.
-------------------------------------------------------------- */

void    FCDECL  exaligned_mrealloc (void** p,
                 size_t blocksize, size_t blockalign,
                 size_t memset_start, size_t memset_stop)
{
    void* pp = *p;                /* Value of p */

    /* (Re)allocation if blocksize not 0 */

    if (blocksize)
    {
        /* Reallocate block and check for success */

        while (blocksize > EXCALCBLOCKSIZE_MAX - blockalign ||
              (pp = _aligned_realloc (*p, blocksize, blockalign))
              == NULL)
        {
            /* Call handler of memory allocation errors
               and continue */

            void (*pvf) (void) = set_exalloc_handler (NULL);
            set_exalloc_handler (pvf);
            if (pvf != NULL) { (*pvf)(); continue; }

            /* Call handler of unrecoverable memory
               allocation errors and return */

            (*exalloc_status.size_handler)();
            exalloc_status.nBlocksFailed++;
            return;
        }

        /* Check if block is aligned properly */

        if (((size_t)pp & (blockalign - 1)) != 0) {}

        /* Increase number of allocated blocks */

        if (pp != NULL)
        {
            exalloc_status.nBlocksAllocated++;
        }

        /* Zero filling */

        if (memset_stop > memset_start)
        {
             memset ((char*)pp + memset_start, 0,
                   memset_stop - memset_start);
        }

        /* Update allocation information */

        if (exalloc_status.pMinAlloc - 1 >= (char*) pp)
        {
            exalloc_status.pMinAlloc = (char*)pp;
        }
        if (exalloc_status.pMaxAlloc < (char*)pp + blocksize)
        {
            exalloc_status.pMaxAlloc = (char*)pp + blocksize;
        }
    }

    /* Freeing if blocksize is 0 and *p is not NULL */

    else if (pp)
    {
        _aligned_free (pp); pp = NULL;
        exalloc_status.nBlocksAllocated--;
    }

    /* Update *p */

    *p = pp;
}

/* --------------------------------------------------------------
    Function exfree() frees block of memory.

    Parameters:
    p           Address of block or NULL.
-------------------------------------------------------------- */

void    FCDECL  exfree (void* p)
{
    exaligned_free (p);
}

/* --------------------------------------------------------------
    Function exaligned_free() frees aligned block of memory.

    Parameters:
    p           Address of block or NULL
-------------------------------------------------------------- */

void    FCDECL  exaligned_free (void* p)
{
    if (p)
    {
        _aligned_free (p);
        exalloc_status.nBlocksAllocated--;
    }
}

/* --------------------------------------------------------------
    Function exrange() invokes either null or range
    error handler depending on value of p.

    Parameters:
    p           Address of block or NULL.
-------------------------------------------------------------- */

void    FCDECL  exrange (void* p)
{
    if (p == NULL)
            (*exalloc_status.null_handler)();
    else    (*exalloc_status.range_handler)();
}

/* --------------------------------------------------------------
    Function excalcblocksize() computes size of memory
    block not less then required and not greater then
    EXSIZE_T_MAX, multiplying EXCALCBLOCKSIZE_MOD
    intermittently by 2 and 1.5.
    For less fragmentation the system header size
    EXCALCBLOCKSIZE_SYS is taken into account.
    For the sake of L1 Pentium cache optimization
    EXCALCBLOCKSIZE_MOD is set as 64**n +/- 16.
    On overflow the function returns EXSIZE_T_MAX.

    Parameters:
    blocksize       Minimal size of block in bytes.

    Returns:        Either optimal size in bytes or
                    EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  excalcblocksize (size_t blocksize)
{
    size_t n = EXCALCBLOCKSIZE_MOD, k = 0;

    for (;; k = ~k, (k? (n <<= 1): (n += (n >> 1))))
    {
        n -= EXCALCBLOCKSIZE_SYS; if (n >= blocksize) break;
        n += EXCALCBLOCKSIZE_SYS; if ((ptrdiff_t) n < 0)
                               { n = EXSIZE_T_MAX; break; }
    }

    return (n);
}

/* --------------------------------------------------------------
    Function exmuladd() computes s*n+k within
    the range 0..EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  exmuladd (size_t s, size_t n, size_t k)
{
    return ((n <= (EXSIZE_T_MAX - k) / s)? (n * s + k): EXSIZE_T_MAX);
}

/* --------------------------------------------------------------
    Function exmul() computes s*n within
    the range 0..EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  exmul (size_t s, size_t n)
{
    return ((s == 1)? n: (n <= EXSIZE_T_MAX / s)? n * s: EXSIZE_T_MAX);
}

/* --------------------------------------------------------------
    Function exadd() computes n+k within
    the range 0..EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  exadd (size_t n, size_t k)
{
    return ((n <= EXSIZE_T_MAX - k)? (n + k): EXSIZE_T_MAX);
}

/* --------------------------------------------------------------
    Function exchkadd() computes n+k within
    the range 0..max.
    If overflow occurs the function calls
    (*exalloc_status.size_handler)().
-------------------------------------------------------------- */

size_t  FCDECL  exchkadd (size_t n, size_t k, size_t max)
{
    if (n > max - k) (*exalloc_status.size_handler)();
    return (n + k);
}

#ifdef  __cplusplus
}
#endif/*__cplusplus*/
