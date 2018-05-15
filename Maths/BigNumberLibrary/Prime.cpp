/* --------------------------------------------------------------
    Signed integers with unlimited range (version 2.1c).
    Primality tests.

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2017 by Dr. Raul N.Shakirov, IMach of RAS(UB).
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
#define DISALLOW_EXARRAY_SHORTTYPE
#include "Cbignum.h"

#ifndef _CBIGNUM_HARDWARE_DIV
#if     UCBNL_MAX > 0xFFFFFFFFUL
#if 1
#define _CBIGNUM_HARDWARE_DIV
#endif
#else //UCBNL_MAX
#if 0
#define _CBIGNUM_HARDWARE_DIV
#endif
#endif//UCBNL_MAX
#endif//_CBIGNUM_HARDWARE_DIV

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

//================================================
//      Table of prime numbers.
//      http://www.utm.edu/research/primes
//================================================

static const short primeTab[] = {
      2,      3,      5,      7,     11,    13,    17,    19,    23,    29,
     31,     37,     41,     43,     47,    53,    59,    61,    67,    71,
     73,     79,     83,     89,     97,   101,   103,   107,   109,   113,
    127,    131,    137,    139,    149,   151,   157,   163,   167,   173,
    179,    181,    191,    193,    197,   199,   211,   223,   227,   229,
    233,    239,    241,    251,    257,   263,   269,   271,   277,   281,
    283,    293,    307,    311,    313,   317,   331,   337,   347,   349,
    353,    359,    367,    373,    379,   383,   389,   397,   401,   409,
    419,    421,    431,    433,    439,   443,   449,   457,   461,   463,
    467,    479,    487,    491,    499,   503,   509,   521,   523,   541,
    547,    557,    563,    569,    571,   577,   587,   593,   599,   601,
    607,    613,    617,    619,    631,   641,   643,   647,   653,   659,
    661,    673,    677,    683,    691,   701,   709,   719,   727,   733,
    739,    743,    751,    757,    761,   769,   773,   787,   797,   809,
    811,    821,    823,    827,    829,   839,   853,   857,   859,   863,
    877,    881,    883,    887,    907,   911,   919,   929,   937,   941,
    947,    953,    967,    971,    977,   983,   991,   997,  1009,  1013,
   1019,   1021,   1031,   1033,   1039,  1049,  1051,  1061,  1063,  1069,
   1087,   1091,   1093,   1097,   1103,  1109,  1117,  1123,  1129,  1151,
   1153,   1163,   1171,   1181,   1187,  1193,  1201,  1213,  1217,  1223,
   1229,   1231,   1237,   1249,   1259,  1277,  1279,  1283,  1289,  1291,
   1297,   1301,   1303,   1307,   1319,  1321,  1327,  1361,  1367,  1373,
   1381,   1399,   1409,   1423,   1427,  1429,  1433,  1439,  1447,  1451,
   1453,   1459,   1471,   1481,   1483,  1487,  1489,  1493,  1499,  1511,
   1523,   1531,   1543,   1549,   1553,  1559,  1567,  1571,  1579,  1583,
   1597,   1601,   1607,   1609,   1613,  1619,  1621,  1627,  1637,  1657,
   1663,   1667,   1669,   1693,   1697,  1699,  1709,  1721,  1723,  1733,
   1741,   1747,   1753,   1759,   1777,  1783,  1787,  1789,  1801,  1811,
   1823,   1831,   1847,   1861,   1867,  1871,  1873,  1877,  1879,  1889,
   1901,   1907,   1913,   1931,   1933,  1949,  1951,  1973,  1979,  1987,
   1993,   1997,   1999,   2003,   2011,  2017,  2027,  2029,  2039,  2053,
   2063,   2069,   2081,   2083,   2087,  2089,  2099,  2111,  2113,  2129,
   2131,   2137,   2141,   2143,   2153,  2161,  2179,  2203,  2207,  2213,
   2221,   2237,   2239,   2243,   2251,  2267,  2269,  2273,  2281,  2287,
   2293,   2297,   2309,   2311,   2333,  2339,  2341,  2347,  2351,  2357,
   2371,   2377,   2381,   2383,   2389,  2393,  2399,  2411,  2417,  2423,
   2437,   2441,   2447,   2459,   2467,  2473,  2477,  2503,  2521,  2531,
   2539,   2543,   2549,   2551,   2557,  2579,  2591,  2593,  2609,  2617,
   2621,   2633,   2647,   2657,   2659,  2663,  2671,  2677,  2683,  2687,
   2689,   2693,   2699,   2707,   2711,  2713,  2719,  2729,  2731,  2741,
   2749,   2753,   2767,   2777,   2789,  2791,  2797,  2801,  2803,  2819,
   2833,   2837,   2843,   2851,   2857,  2861,  2879,  2887,  2897,  2903,
   2909,   2917,   2927,   2939,   2953,  2957,  2963,  2969,  2971,  2999,
   3001,   3011,   3019,   3023,   3037,  3041,  3049,  3061,  3067,  3079,
   3083,   3089,   3109,   3119,   3121,  3137,  3163,  3167,  3169,  3181,
   3187,   3191,   3203,   3209,   3217,  3221,  3229,  3251,  3253,  3257,
   3259,   3271,   3299,   3301,   3307,  3313,  3319,  3323,  3329,  3331,
   3343,   3347,   3359,   3361,   3371,  3373,  3389,  3391,  3407,  3413,
   3433,   3449,   3457,   3461,   3463,  3467,  3469,  3491,  3499,  3511,
   3517,   3527,   3529,   3533,   3539,  3541,  3547,  3557,  3559,  3571,
   3581,   3583,   3593,   3607,   3613,  3617,  3623,  3631,  3637,  3643,
   3659,   3671,   3673,   3677,   3691,  3697,  3701,  3709,  3719,  3727,
   3733,   3739,   3761,   3767,   3769,  3779,  3793,  3797,  3803,  3821,
   3823,   3833,   3847,   3851,   3853,  3863,  3877,  3881,  3889,  3907,
   3911,   3917,   3919,   3923,   3929,  3931,  3943,  3947,  3967,  3989,
   4001,   4003,   4007,   4013,   4019,  4021,  4027,  4049,  4051,  4057,
   4073,   4079,   4091,   4093,   4099,  4111,  4127,  4129,  4133,  4139,
   4153,   4157,   4159,   4177,   4201,  4211,  4217,  4219,  4229,  4231,
   4241,   4243,   4253,   4259,   4261,  4271,  4273,  4283,  4289,  4297,
   4327,   4337,   4339,   4349,   4357,  4363,  4373,  4391,  4397,  4409,
   4421,   4423,   4441,   4447,   4451,  4457,  4463,  4481,  4483,  4493,
   4507,   4513,   4517,   4519,   4523,  4547,  4549,  4561,  4567,  4583,
   4591,   4597,   4603,   4621,   4637,  4639,  4643,  4649,  4651,  4657,
   4663,   4673,   4679,   4691,   4703,  4721,  4723,  4729,  4733,  4751,
   4759,   4783,   4787,   4789,   4793,  4799,  4801,  4813,  4817,  4831,
   4861,   4871,   4877,   4889,   4903,  4909,  4919,  4931,  4933,  4937,
   4943,   4951,   4957,   4967,   4969,  4973,  4987,  4993,  4999,  5003,
   5009,   5011,   5021,   5023,   5039,  5051,  5059,  5077,  5081,  5087,
   5099,   5101,   5107,   5113,   5119,  5147,  5153,  5167,  5171,  5179,
   5189,   5197,   5209,   5227,   5231,  5233,  5237,  5261,  5273,  5279,
   5281,   5297,   5303,   5309,   5323,  5333,  5347,  5351,  5381,  5387,
   5393,   5399,   5407,   5413,   5417,  5419,  5431,  5437,  5441,  5443,
   5449,   5471,   5477,   5479,   5483,  5501,  5503,  5507,  5519,  5521,
   5527,   5531,   5557,   5563,   5569,  5573,  5581,  5591,  5623,  5639,
   5641,   5647,   5651,   5653,   5657,  5659,  5669,  5683,  5689,  5693,
   5701,   5711,   5717,   5737,   5741,  5743,  5749,  5779,  5783,  5791,
   5801,   5807,   5813,   5821,   5827,  5839,  5843,  5849,  5851,  5857,
   5861,   5867,   5869,   5879,   5881,  5897,  5903,  5923,  5927,  5939,
   5953,   5981,   5987,   6007,   6011,  6029,  6037,  6043,  6047,  6053,
   6067,   6073,   6079,   6089,   6091,  6101,  6113,  6121,  6131,  6133,
   6143,   6151,   6163,   6173,   6197,  6199,  6203,  6211,  6217,  6221,
   6229,   6247,   6257,   6263,   6269,  6271,  6277,  6287,  6299,  6301,
   6311,   6317,   6323,   6329,   6337,  6343,  6353,  6359,  6361,  6367,
   6373,   6379,   6389,   6397,   6421,  6427,  6449,  6451,  6469,  6473,
   6481,   6491,   6521,   6529,   6547,  6551,  6553,  6563,  6569,  6571,
   6577,   6581,   6599,   6607,   6619,  6637,  6653,  6659,  6661,  6673,
   6679,   6689,   6691,   6701,   6703,  6709,  6719,  6733,  6737,  6761,
   6763,   6779,   6781,   6791,   6793,  6803,  6823,  6827,  6829,  6833,
   6841,   6857,   6863,   6869,   6871,  6883,  6899,  6907,  6911,  6917,
   6947,   6949,   6959,   6961,   6967,  6971,  6977,  6983,  6991,  6997,
   7001,   7013,   7019,   7027,   7039,  7043,  7057,  7069,  7079,  7103,
   7109,   7121,   7127,   7129,   7151,  7159,  7177,  7187,  7193,  7207,
   7211,   7213,   7219,   7229,   7237,  7243,  7247,  7253,  7283,  7297,
   7307,   7309,   7321,   7331,   7333,  7349,  7351,  7369,  7393,  7411,
   7417,   7433,   7451,   7457,   7459,  7477,  7481,  7487,  7489,  7499,
   7507,   7517,   7523,   7529,   7537,  7541,  7547,  7549,  7559,  7561,
   7573,   7577,   7583,   7589,   7591,  7603,  7607,  7621,  7639,  7643,
   7649,   7669,   7673,   7681,   7687,  7691,  7699,  7703,  7717,  7723,
   7727,   7741,   7753,   7757,   7759,  7789,  7793,  7817,  7823,  7829,
   7841,   7853,   7867,   7873,   7877,  7879,  7883,  7901,  7907,  7919,
   7927 /* The latter prime number is included for algorithm purposes */
};

//================================================
//      Wheel factorization table for module 210.
//      http://www.utm.edu/research/primes/glossary/WheelFactorization.html
//================================================

static const short wheelTab [] = { 210,
      1,  11,  13,  17,  19,  23,  29,  31,  37,  41,
     43,  47,  53,  59,  61,  67,  71,  73,  79,  83,
     89,  97, 101, 103, 107, 109, 113, 121, 127, 131,
    137, 139, 143, 149, 151, 157, 163, 167, 169, 173,
    179, 181, 187, 191, 193, 197, 199, 209
};

//================================================
//      Size of tables.
//================================================

#define SPRPnTabSize (7U)
#define primeTabSize (sizeof (primeTab) / sizeof (*primeTab) - 1)
#define wheelTabSize (sizeof (wheelTab) / sizeof (*wheelTab))

//================================================
//      Diagnostic messages.
//================================================

static const char szTooSmall[]= "The number should be 2 or greater\n";
static const char szOK      []= ": OK\n";
static const char szFail    []= ": Fail\n";
static const char szNotPrime[]= "Sorry, the number is NOT prime.\n";
static const char szPrime   []= "Congratulations! The number is prime!\n";
static const char szSmall   []= "Test for small modules passed.\n";
static const char szMayPrime[]= "The number MAY be prime.\n";
static const char szRmnPrime[]= "Modules Ok\n"\
                                "Good news! The number is prime if the "\
                                "generalized Riemann hypothesis is true.\n";
static const char szFound   []= ": Found!\n";
static const char szSquare  []= "The number has square factor.\n";
static const char szFactor  []= "The number has factor.\n";
static const char szNoFactor[]= "The number has no factors.\n";
static const char szNoSmallF[]= "No small factor found.\n";

//================================================
//      Strong Probable Primality test.
//      http://primes.utm.edu/prove/prove2_3.html
//      Returns:
//      0 - composite
//      2 - prime by exact SPRP
//      3 - possibly prime by Miller
//      5 - possible prime by large SPRP >= 7927
//      6 - possible prime by small SPRP <= 7919
//      7 - possible prime by exact SPRP <= 17
//================================================

int     SPRP (  const cBigNumber& n,    // Number.
                      cBigNumber& b,    // Buffer for module.
                      CBNL start,       // Start of range (0..7..1000).
                      CBNL stop         // End of range+1 (1..7..1000..)
        )                               // or 0 for unlimited search.
{
//      Constants are local due to bug in gcc 2.96.

  static const cBigNumber SPRP2  (           "2047",  10);
  static const cBigNumber SPRP3  (        "1373653",  10);
  static const cBigNumber SPRP5  (       "25326001",  10);
  static const cBigNumber SPRP7x (     "3215031751",  10);
  static const cBigNumber SPRP7  (   "118670087467",  10);
  static const cBigNumber SPRP11 (  "2152302898747",  10);
  static const cBigNumber SPRP13 (  "3474749660383",  10);
  static const cBigNumber SPRP17 ("341550071728321",  10);

  cTTY cMsg (cBigNumberMessages);
  cTTY cPrg (cBigNumberProgress);

//      Check parameters.

  if (start < 0) start = 0;
  if (start > (CBNL)primeTabSize) start = primeTabSize;
  if (stop < 0) stop = 0;

  b = primeTab [(size_t)start];
  if (n < 2) { cMsg << szTooSmall; return 0; }

  cMsg << "\nStrong Probable Primality test\n";

//      Find in initial SPRP.

  switch (start)
  {
  case 0:   cMsg << "Module   2" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP2)                        { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 1:   cMsg << "Module   3" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP3)                        { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 2:   cMsg << "Module   5" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP5)                        { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 3:   cMsg << "Module   7" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP7 && n != SPRP7x)         { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 4:   cMsg << "Module  11" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP11)                       { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 5:   cMsg << "Module  13" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP13)                       { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}

  case 6:   cMsg << "Module  17" << "";
            if (!b_SPRP (n, b))     { cMsg << szFail  << szNotPrime; return 0;}
            cMsg << szOK;
            if (n < SPRP17)                       { cMsg << szPrime; return 2;}
            b = primeTab [(size_t)++start];
            if (start == stop)      { cMsg << szSmall << szMayPrime; return 7;}
  }

//      Maximal module for SPRP is defined as 2 * ln(n) * ln(n)
//      ln   (n) == log2 (n) * ln (2)
//      log2 (n) <= (n-1).bits()

  cBigNumber maxb = n;
  maxb.setbits (--maxb);                // log2 (n)
  maxb *= maxb;                         // log2 (n) * log2 (n)
  maxb *=  960906028L;                  // 0.9609060278364     = 2*ln(2)*ln(2)
  maxb /= 1000000000L;                  // log2 (n) * log2 (n) * 2*ln(2)*ln(2)

//      Check search range.

  if (stop != 0)
  if (stop <= (CBNL)primeTabSize)
  {
    if (maxb.length() > 1 || maxb.loword() >= primeTab [(size_t)stop])
        maxb = primeTab [(size_t)stop - 1];
    else stop = 0;
  }
  else
  {
    b = ((stop - primeTabSize + 2 * wheelTabSize - 2) / wheelTabSize);
    b *= wheelTab [0];
    b += primeTab [primeTabSize];
    if (maxb >= b) maxb = b - wheelTab [0];
    else stop = 0;
  }

  if (maxb.length() > 1 || maxb.loword() >= primeTab [(size_t)start])
  {
    cMsg << "Modules " << primeTab [(size_t)start] << ".." << maxb << '\n';
  }

//      Find in the table of prime numbers.

  {
    CBNL maxl = primeTab [primeTabSize - 1];
    if (maxb.length() <= 1 && maxl > maxb.loword()) maxl = maxb.loword();

    size_t i = (size_t)start;
    for (; b = primeTab [i], primeTab [i] <= maxl; i++)
    {
      cPrg <<= primeTab [i];
      if (!b_SPRP (n, b)) { cMsg << b << szFail << szNotPrime; return 0; }
    }
  }

//      Find in the table of wheel factorization.
//      For primeTabSize = 1000 initial i = 36, b = 7927.

  {
    size_t i = 1;
    for (; wheelTab [i] != primeTab [primeTabSize] % wheelTab [0]; i++)
      continue;

    for (b = primeTab [primeTabSize]; b <= maxb; )
    {
      if (b_SPRP (b, 2) && b_SPRP (b, 3))
      {
        cPrg <<= b;
        if (!b_SPRP (n, b)) { cMsg << b << szFail << szNotPrime; return 0; }
      }
      if (++i < wheelTabSize)
      {
        b += wheelTab [i] - wheelTab [i - 1];
      }
      else
      {
        i = 1;
        b += wheelTab [0] + wheelTab [1] - wheelTab [wheelTabSize - 1];
      }
    }
  }

//      No module found.

  if (stop == 0) { cMsg << szRmnPrime; return 3; }
        cMsg << szSmall << szMayPrime; return 5 + (stop <= (CBNL)primeTabSize);
}

int SPRP        (const cBigNumber& n)
{
  cBigNumber b;
  return SPRP (n, b, 0, 0);
}

int SPRP        (const cBigNumber& n, cBigNumber& b)
{
  return SPRP (n, b, 0, 0);
}

int FastSPRP    (const cBigNumber& n)
{
  cBigNumber b;
  return SPRP (n, b, 0, SPRPnTabSize);
}

int FastSPRP    (const cBigNumber& n, cBigNumber& b)
{
  return SPRP (n, b, 0, SPRPnTabSize);
}

int PostSPRP    (const cBigNumber& n)
{
  cBigNumber b;
  return SPRP (n, b, SPRPnTabSize, 0);
}

int PostSPRP    (const cBigNumber& n, cBigNumber& b)
{
  return SPRP (n, b, SPRPnTabSize, 0);
}

//================================================
//      Factor Primality test.
//      http://primes.utm.edu/prove/prove2_1.html
//      Returns:
//      0 - factor is found (number is composite)
//      1 - no factors      (number is prime)
//     10 - no factors in the range indicated
//================================================

int TestFactor (const cBigNumber& n,    // Number.
                      cBigNumber& b,    // Buffer for factor.
                      CBNL start,       // Start of range (0..1000).
                      CBNL stop         // End of range+1 (1..1000..)
        )                               // or 0 for unlimited search.
{
  cTTY cMsg (cBigNumberMessages);
  cTTY cPrg (cBigNumberProgress);

//      Check parameters.

  if (start < 0) start = 0;
  if (start > (CBNL)primeTabSize) start = primeTabSize;
  if (stop < 0) stop = 0;

  b = primeTab [(size_t)start];
  if (n < 2) { cMsg << szTooSmall; return 0; }

//      Determine maximal possible divider.

  cBigNumber maxb = n;
  b.setsqrtrm (maxb);
  if (maxb == 0) { cMsg << b << szFound << szSquare << szNotPrime; return 0; }
  maxb = b;

//      Check search range.

  if (stop)
  if (stop <= (CBNL)primeTabSize)
  {
    if (maxb.length() > 1 || maxb.loword() >= primeTab [(size_t)stop])
        maxb = primeTab [(size_t)stop - 1];
    else stop = 0;
  }
  else
  {
    b = ((stop - primeTabSize + 2 * wheelTabSize - 2) / wheelTabSize);
    b *= wheelTab [0];
    b += primeTab [primeTabSize];
    if (maxb >= b) maxb = b - wheelTab [0];
    else stop = 0;
  }

  if (maxb.length() > 1 || maxb.loword() >= primeTab [(size_t)start])
  {
    cMsg << "\nSearching ";
    if (stop) cMsg << "small ";
    cMsg << "factor in a range "
         << primeTab [(size_t)start] << ".." << maxb << '\n';
  }

//      Need not try 2 for odd numbers.

  b = 2;
  if (start == 0) start = n.loword() & 1;

//      Select either single word or generic factoring code.

#ifdef  _CBIGNUM_HARDWARE_DIV
  if (n.length() == 1)
  {
    CBNL kmax = 0x100000L, k = kmax - primeTabSize;

//      Find in the table of prime numbers.

    {
      CBNL maxl = primeTab [primeTabSize - 1];
      if (maxl > maxb.loword()) maxl = maxb.loword();

      size_t i = (size_t)start;
      for (; (CBNL&)b.loword() = primeTab [i], primeTab [i] <= maxl; i++)
      {
        if (n.loword() % primeTab [i] == 0)
           { cMsg << b << szFound << szFactor << szNotPrime; return 0; }
      }
    }

//      Find in the table of wheel factorization.
//      For primeTabSize = 1000 initial i = 36, b = 7927.

    {
      size_t i = 1;
      for (; wheelTab [i] != primeTab [primeTabSize] % wheelTab [0]; i++)
        continue;

      CBNL lb = primeTab [primeTabSize];
      while ((CBNL&)b.loword() = lb, lb <= maxb.loword())
      {
        if (n.loword() % lb == 0)
           { cMsg << b << szFound << szFactor << szNotPrime; return 0; }
        if (--k < 0) { cPrg <<= lb; k = kmax; }
        if (++i < wheelTabSize)
        {
          lb += wheelTab [i] - wheelTab [i - 1];
        }
        else
        {
          i = 1;
          lb += wheelTab [0] + wheelTab [1] - wheelTab [wheelTabSize - 1];
        }
      }
    }
  }
  else// Generic code.
#endif//_CBIGNUM_HARDWARE_DIV
  {
    CBNL kmax = 0x10000L >> cULongBits ((unsigned CBNL)maxb.length()), k = kmax;

//      Find in the table of prime numbers.

    {
      CBNL maxl = primeTab [primeTabSize - 1];
      if (maxb.length() <= 1 && maxl > maxb.loword()) maxl = maxb.loword();

      size_t i = (size_t)start;
      for (; b = primeTab [i], primeTab [i] <= maxl; i++)
      {
        if (n % b.loword() == 0)
           { cMsg << b << szFound << szFactor << szNotPrime; return 0; }
        if (--k < 0) { cPrg <<= primeTab [i]; k = kmax; }
      }
    }

//      Find in the table of wheel factorization.
//      For primeTabSize = 1000 initial i = 36, b = 7927.

    {
      size_t i = 1;
      for (; wheelTab [i] != primeTab [primeTabSize] % wheelTab [0]; i++)
        continue;

      for (b = primeTab [primeTabSize]; b <= maxb; )
      {
        if (n % b == 0)
           { cMsg << b << szFound << szFactor << szNotPrime; return 0; }
        if (--k < 0) { cPrg <<= b; k = kmax; }
        if (++i < wheelTabSize)
        {
          b += wheelTab [i] - wheelTab [i - 1];
        }
        else
        {
          i = 1;
          b += wheelTab [0] + wheelTab [1] - wheelTab [wheelTabSize - 1];
        }
      }
    }
  }

//      No factor found.

  if (stop == 0) { cMsg << szNoFactor << szPrime;    return 1; }
                   cMsg << szNoSmallF << szMayPrime; return 10;
}

int TestFactor      (const cBigNumber& n)
{
  cBigNumber b;
  return TestFactor (n, b, 0, 0);
}

int TestFactor      (const cBigNumber& n, cBigNumber& b)
{
  return TestFactor (n, b, 0, 0);
}

int TestSmallFactor (const cBigNumber& n)
{
  cBigNumber b;
  return TestFactor (n, b, 0, primeTabSize);
}

int TestSmallFactor (const cBigNumber& n, cBigNumber& b)
{
  return TestFactor (n, b, 0, primeTabSize);
}

int TestLargeFactor (const cBigNumber& n)
{
  cBigNumber b;
  return TestFactor (n, b, primeTabSize, 0);
}

int TestLargeFactor (const cBigNumber& n, cBigNumber& b)
{
  return TestFactor (n, b, primeTabSize, 0);
}

//================================================
//      Combined (Factor + Miller) Primality test.
//      Returns:
//      0 - composite
//      1 - prime by factoring
//      2 - prime by exact SPRP
//      3 - probable prime by Miller
//      5 - probable prime by large SPRP >= 7927
//      6 - probable prime by small SPRP <= 7919
//      7 - probable prime by exact SPRP <= 17
//     11 - prime by factoring, but composite by SPRP (error)
//     12 - prime by exact SPRP, but has factor       (error)
//     13 - probable prime by Miller, but has factor  (error)
//================================================

int   IsPrime  (const cBigNumber& n,    // Number.
                      CBNL start,       // Start of range (0..1000).
                      CBNL stop,        // End of range+1 (1..1000..)
                                        // or 0 for unlimited search.
                      int nCheck)       // Either improve or check results
{                                       // with equal or more return code.
  cBigNumber b;                         // Buffer.

//      Find for small factor.

  int retcode1 = TestSmallFactor (n, b);
  if (retcode1 < nCheck)                return retcode1;
  if (retcode1 == 0 && n < 2)           return retcode1;

//      Strong Probable Primality test.

  int retcode2 = SPRP (n, b, start, stop);
  if (retcode1 == 1 && retcode2 == 0)   return retcode1 + 10;
  if (retcode2 < nCheck)                return retcode2;

//      Find for large factor.

  if (retcode1 > 1) retcode1 = TestLargeFactor (n, b);
  if (retcode1 == 1 && retcode2 == 0)   return retcode1 + 10;
  if (retcode1 == 0 &&
     (retcode2 == 2 || retcode2 == 3))  return retcode2 + 10;

  return retcode1;
}

int   IsPrime (const cBigNumber& n)
{
  return IsPrime (n, 0, 0, 0);
}

int   IsProvedPrime (const cBigNumber& n)
{
  return IsPrime (n, 0, SPRPnTabSize, 3);
}

int   IsMillerPrime (const cBigNumber& n)
{
  return IsPrime (n, 0, 0, 4);
}

int   IsStrongPrime (const cBigNumber& n)
{
  return IsPrime (n, 0, SPRPnTabSize, 10);
}
