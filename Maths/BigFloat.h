#pragma once

#include "Fraction.h"

typedef long double longDouble;

typedef Fraction closedLongDouble;

Fraction sqrt(Fraction _val, unsigned int _prec = 64);
Fraction pow(Fraction _val, int n);
Fraction minv(const Fraction _a, const Fraction _b);
Fraction maxv(const Fraction _a, const Fraction _b);