/* --------------------------------------------------------------
    Compiler independent stream output (version 2.0).

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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Cios.h"
#include "Ctty.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

/* --------------------------------------------------------------
    Preprocessing options.
    _CTTY_R     use '\r' instead of '\b' to return cursor in
                cTTY_ProgressOutput
-------------------------------------------------------------- */

#if 0
#define _CTTY_R
#endif

/* --------------------------------------------------------------
    External references are declared as "C", in order
    to block name mangling. This facilitates porting of
    object modules among different compilers.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Helper functions.
-------------------------------------------------------------- */

static  const char*   pszOf1 = " of";           /* Prefix for max */
static  const char*   pszOf2 = "";              /* Suffix for max */
static  size_t        nOf    = 3;               /* Len of prefix and suffix */

void    cTTY_SetMaxOutput (const char* psz1, const char* psz2)
{
  nOf = strlen (pszOf1 = psz1) + strlen (pszOf2 = psz2);
}

char* cTTY_ltoa10 (CBNL num, char* buf)         /* Convert to string */
{                                               /* buf must be large enough */
  char *b = buf;
  if (num < 0) { *b++ = '-'; num = -num; }
  {
    char *p = b;

    do *p++ = (char) ((int) (num % 10) + '0');  /* Store digit */
    while ((num /= 10) > 0);                    /* Get next digit */

    *p-- = '\0';                                /* Terminate string */

    do { char c = *p; *p-- = *b; *b++ = c; }    /* Swap digits */
    while (b < p);                              /* until halfway */
  }
  return buf;
}

/* --------------------------------------------------------------
    Output support for C++
-------------------------------------------------------------- */

size_t  cTTY_PutStream (_CIOSTD ostream& os,    /* Output to stream */
                        const   char*   psz,    /* String or NULL */
                                CBNL    num,    /* Number */
                                CBNL    max,    /* Maximal number or 0 */
                                long    cop)    /* Options and char, but \r */
{                                               /* Returns width of line */
  size_t n = 0;                                 /* after last \n */
  unsigned width = (unsigned) os.width (0);     /* Set width */
/*
        Put string.
*/
  if (psz != NULL) { n = strlen (psz); os << psz; }
/*
        Put numbers.
*/
  char c = 0;                                   /* Suffix */
  if ((num | max | (cop & cTTY_numflags)) != 0L)
  {
    if ((cop & cTTY_numpercent) != 0L)
    {                                           /* Percent of numbers */
      if (max != 0 && max != 100) num = (num * 100 + max/2) / max;
      max = 0; c = '%';
    }
    else
    if ((cop & cTTY_numscale) != 0L)            /* Scale numbers */
    {
      CBNL abs = num;
      if (abs < 0) abs = -abs;
      if (abs < max) abs = max;
      if (abs >= 0x2600) { num >>=  9; max >>=  9; c = 'K'; abs >>=  9;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'M'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'G'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'T'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'P'; }}}}
                           ++num /= 2; ++max /= 2; }
    }                                           /* 0x2600 = 10240-512 */

    long unit = os.setf (_CIOSTD ios::unitbuf) & _CIOSTD ios::unitbuf;
    char fill = os.fill (' ');                  /* Set flag and fill char */
    char buf [BITS + 8];                        /* Buffer for conversion */

    {                                           /* Put num */
      size_t nn = strlen (cTTY_ltoa10 (num, buf));
      nn += cTTY_getnumspace (cop);
      size_t mm = cTTY_getnumwidth (cop);
      if (psz != NULL)
      {
        size_t m = cTTY_getstrwidth (cop) + mm;
        if (n + nn < m) nn = m - n;
      }
      if (nn < mm) nn = mm;
      n += nn;
      os << _CIOSTD setw ((unsigned)nn) << buf;
    }

    if (max != 0L)                              /* Put non-zero max */
    {
      size_t nn = strlen (cTTY_ltoa10 (max, buf));
      nn += cTTY_getnumspace (cop);
      size_t mm = cTTY_getnumwidth (cop);
      if (nn < mm) nn = mm;
      n += nn + nOf;
      os << pszOf1 << _CIOSTD setw ((unsigned)nn) << buf << pszOf2;
    }

    if (unit == 0) os.unsetf (_CIOSTD ios::unitbuf);
    os.fill (fill);                             /* Restore flag and char */

    if (c != 0) { ++n; os << c; }               /* Put suffix */
  }
/*
        Put character.
*/
  c = (char) cop;
  if (c != 0 && c != '\r') { ++n; os << c; }    /* Put character */

  os.width (width);                             /* Restore width */
  return n;
}

void    cTTY_CleanStream (_CIOSTD ostream& os,  /* Put spaces and backspaces */
                                size_t  nSpace, /* Number of spaces */
                                size_t  nBS)    /* Number of backspaces */
{
  long unit = os.setf (_CIOSTD ios::unitbuf) & _CIOSTD ios::unitbuf;
  char fill = os.fill (' ');                    /* Set flag and fill char */
  unsigned width = (unsigned) os.width (0);     /* Set width */

  for (; nSpace > 0; nSpace--) os << ' ';       /* Must put chars because of */
#ifndef _CTTY_R
  for (; nBS    > 0; nBS--   ) os << '\b';      /* some gcc 2.96 strangeness */
#else /*_CTTY_R*/
  if (nBS) { nBS = 0; os << '\r'; }
#endif/*_CTTY_R*/
  os << _CIOSTD flush;

  if (unit == 0) os.unsetf (_CIOSTD ios::unitbuf);
  os.fill (fill);                               /* Restore flag and char */
  os.width (width);                             /* Restore width */
}

void   cTTY_FlushStream (_CIOSTD ostream& os)   /* Flush stream */
{
  os << _CIOSTD flush;
}

#define cTTY_out        _CIOSTD cout
#define cTTY_err        _CIOSTD cerr

/* --------------------------------------------------------------
    Implementation of handler functions.
-------------------------------------------------------------- */

static  void*   pWidth  = NULL;                 /* Standard output object */
static  size_t  nWidth  = 0;                    /* Standard output width */
static  void*   pChars  = NULL;                 /* Progress output object */
static  size_t  nChars  = 0;                    /* Progress output width */
static  size_t  nClean  = 0;                    /* Progress output clear */

int     cTTY_StandardOutput    (void*   pt,     /* Object or NULL */
                        const   char*   psz,    /* String or NULL */
                                CBNL    num,    /* Number */
                                CBNL    max,    /* Maximal number or 0 */
                                long    cop)    /* Options and character */
{                                               /* Returns 0 */
  while (nClean != 0) cTTY_ProgressOutput (NULL, NULL, 0, 0, '\r');
                                                /* Clear progress */
  const char *p = NULL;
  if (psz)
  {
    if ((p = strrchr (psz, '\r')) != NULL)
    do
      cTTY_StandardOutput (pt, NULL, 0, 0, *psz);
    while (psz++ != p);

    if ((p = strrchr (psz, '\n')) != NULL)
    {
      nWidth = (size_t)(psz - p - 1);
    }
  }

  size_t n = cTTY_PutStream (cTTY_out, psz, num, max, cop);
  if (n != 0) { pWidth = pt; nWidth += n; }

  switch ((char)cop)
  {
    case '\r':  if (nWidth == 0 || pWidth != pt && pt != NULL) break;
                cTTY_PutStream (cTTY_out, NULL, 0, 0, '\n');
    case '\n':  n = nWidth = 0;
    default:    if (n == 0 || p != NULL) cTTY_FlushStream (cTTY_out);
  }

  return 0;
}

int     cTTY_ProgressOutput    (void*   pt,     /* Object or NULL */
                        const   char*   psz,    /* String or NULL */
                                CBNL    num,    /* Number */
                                CBNL    max,    /* Maximal number or 0 */
                                long    cop)    /* Options and character */
{                                               /* Returns 0 */
  const char *p = NULL;
  if (psz)
  {
    if ((p = strrchr (psz, '\r')) != NULL)
    do
      cTTY_ProgressOutput (pt, NULL, 0, 0, *psz);
    while (psz++ != p);

    if ((p = strrchr (psz, '\n')) != NULL)
    {
      nWidth = 0; nChars = (size_t)(psz - p - 1); nClean = 0;
    }
    else
    {
      size_t n = strlen (psz);
      if (n + nChars > 79)
      {
        psz += n;
        if (nChars <= 77)
        {
          psz += nChars - 77;
          nChars += cTTY_PutStream (cTTY_err, "..", 0, 0, 0);
          pChars = pt;
        }
      }
    }
  }

  size_t n = cTTY_PutStream (cTTY_err, psz, num, max, cop);
  if (n != 0) { pChars = pt; nChars += n; }

  switch ((char)cop)
  {
    case '\r': if (nClean < nChars) nClean = nChars;
               if (nClean == 0 || pChars != pt && pt != NULL) break;
               cTTY_CleanStream (cTTY_err, nClean - nChars, nClean);
               nClean = nChars; nChars = 0;     /* Store length of line */
               if (nClean > 79) nClean = 0;
               break;
    case '\n': n = nWidth = nChars = nClean = 0;
    default:   if (n == 0 || p != NULL) cTTY_FlushStream (cTTY_err);
  }

  return 0;
}

#ifdef  __cplusplus
}
#endif/*__cplusplus*/
