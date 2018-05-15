#pragma once

#include "../Network/Network.h"

cBigNumber gcd(cBigNumber x, cBigNumber y);

class Fraction {
public:
	cBigNumber a, b;

  Fraction();

  Fraction(int _val);
  Fraction(long _val);
  Fraction(long long _val);
  Fraction(unsigned int _val);
  Fraction(unsigned long _val);
  Fraction(unsigned long long _val);

  Fraction(cBigNumber _val);

  Fraction(cBigNumber a, cBigNumber b);

  Fraction(float d);
  Fraction(double d);
  Fraction(long double d);

  Fraction(string s);

  Fraction(const Fraction& _val);

  long double toDouble() const;
  string toStr(int prec) const;
  
  Fraction& simplify(); //Self simplify, and return

  void get(DataElement* data);
  void set(DataElement* data);

  Fraction operator+=(const Fraction rhs);
  Fraction operator-=(const Fraction rhs);
  Fraction operator*=(const Fraction rhs);
  Fraction operator/=(const Fraction rhs);
  Fraction operator%=(const Fraction rhs);
};

//b rounded to length of a is a
bool approxEqual(Fraction a, Fraction b, int prec);
Fraction approx(string s);
Fraction approx(double d, int fprec);

Fraction operator+ (const Fraction x, const Fraction y);
Fraction operator- (const Fraction x, const Fraction y);
Fraction operator- (const Fraction x);
Fraction operator* (const Fraction x, const Fraction y);
Fraction operator/ (const Fraction x, const Fraction y);

bool operator== (const Fraction lhs, const Fraction rhs);
bool operator<  (const Fraction lhs, const Fraction rhs);
bool operator>  (const Fraction lhs, const Fraction rhs);
bool operator<= (const Fraction lhs, const Fraction rhs);
bool operator>= (const Fraction lhs, const Fraction rhs);

string to_string(const Fraction _val);
double toDouble(const Fraction _val);
Fraction abs(const Fraction _val);
cBigNumber floor(const Fraction _val);
cBigNumber ceil(const Fraction _val);

ostream& operator<<(ostream& o, const Fraction _val);
istream& operator>>(istream& i, Fraction& _val);

template<typename T, typename U>
T to_doubleT(U _val) {
  return T(_val);
}
template <>
long double to_doubleT<long double, Fraction>(Fraction _val) ;
template <>
double to_doubleT<double, Fraction>(Fraction _val);
template <>
float to_doubleT<float, Fraction>(Fraction _val);