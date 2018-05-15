#include "Fraction.h"

cBigNumber gcd(cBigNumber x, cBigNumber y) {
  if (y == 0) {
    return x;
  }
  else {
    return gcd(y, x%y);
  }
}

Fraction::Fraction() {
  a = 0;
  b = 1;
}

Fraction::Fraction(cBigNumber a, cBigNumber b) {
  this->a = a;
  if (!b) {

    b = 1;
    return;
  }
  this->b = b;
}

Fraction::Fraction(int _val) {
  a = _val;
  b = 1;
}
Fraction::Fraction(long _val) {
  a = _val;
  b = 1;
}
Fraction::Fraction(long long _val) {
  a = _val;
  b = 1;
}
Fraction::Fraction(unsigned int _val) {
  a = _val;
  b = 1;
}
Fraction::Fraction(unsigned long _val) {
  a = _val;
  b = 1;
}
Fraction::Fraction(unsigned long long _val) {
  a = _val;
  b = 1;
}

Fraction::Fraction(cBigNumber _val) {
  a = _val;
  b = 1;
}

Fraction::Fraction(float d) {
  *this = approx(d, 8);
}
Fraction::Fraction(double d) {
  *this = approx(d, 16);
}
Fraction::Fraction(long double d) {
  *this = approx(d, 24);
}

Fraction::Fraction(string s) {
  string spl[2];
  int p = s.find('/');
  if (0 <= p && p < s.size() - 1) {
    spl[0] = s.substr(0, p);
    spl[1] = s.substr(p + 1, s.size() - p + 1);
    a = atoi(spl[0].c_str());
    b = atoi(spl[1].c_str());
    if (!b) {
      b = 1;
      return;
    }
  }
  else {
    *this = approx(s);
  }

  this->simplify();
}

Fraction::Fraction(const Fraction& _val) {
  a = _val.a;
  b = _val.b;
}

Fraction& Fraction::simplify() {
  cBigNumber d = gcd(a, b);

  a /= d;
  b /= d;

  if (b < 0) {
    a *= -1;
    b *= -1;
  }

  return *this;
}

long double Fraction::toDouble() const {
  cBigNumber na = a;
  cBigNumber nb = b;
  na.abs();
  nb.abs();
  long double d = 0;

  int prec = 53; //floating point precision (base 2)
  d = (na / nb).tolong();
  na = na % nb;
  long double dfc = 1;
  do {
    dfc /= 2;
    na *= 2;
    d += dfc * (na / nb).tolong();
    na = na % nb;
    --prec;
  } while (prec>0);

  if (a*b < 0) {
    d *= -1;
  }
  return d;
}
string Fraction::toStr(int prec) const {
  cBigNumber na = a;
  cBigNumber nb = b;

  stringstream ss;
  ss << a / b << ".";
  na = na % nb;

  do {
    na *= 10;
    ss << na / nb;
    na -= (na / nb)*nb;
    --prec;
  } while (prec>0);

  return ss.str();
}

//b rounded to length of a is a
bool approxEqual(Fraction a, Fraction b, int prec) {
  Fraction pm(5, 10); //0.5

  if (prec < 0) {
    pm.b *= cBigNumber(10).pow(-prec);
  }
  if (prec > 0) {
    pm.a *= cBigNumber(10).pow(prec);
  }
  Fraction f = a-pm;
  return (a - pm <= b && b<a + pm);
}

Fraction approx(string s) {
  string integer, fractional;
  size_t dot = s.find('.');
  if (dot == s.npos) {
    s += ".0";
    dot = s.find('.');
  }
  integer = s.substr(0, dot);
  fractional = s.substr(dot + 1, s.length() - dot - 1);
  Fraction mine(0, 1);
  Fraction p(1, 2); //p=a+c q=b+d
  Fraction maxe(1, 1);
  Fraction fract;
  fract.a = strTo<cBigNumber>(fractional);
  fract.b = pow(10l, fractional.length());
  bool fe = false;
  if (approxEqual(fract, mine, -int(fractional.length()))) {
    fe = true;
    p = mine;
  }
  if (approxEqual(fract, maxe, -int(fractional.length()))) {
    fe = true;
    p = maxe;
  }
  while (!fe && !approxEqual(fract, p, -int(fractional.length()))) {
    if (p>fract) {
      maxe = p;
      p.a += mine.a;
      p.b += mine.b;
    }
    else {
      mine = p;
      p.a += maxe.a;
      p.b += maxe.b;
    }
  }
  cBigNumber integ = strTo<cBigNumber>(integer);
  if (integer.length() && integer[0] == '-') {
    p = p*-1;
  };
  p += integ;
  return p;
}
Fraction approx(double d, int fprec) {
  cBigNumber integ = floor(d);
  d -= floor(d);
  Fraction fract;
  fract.b = cBigNumber(10).pow(fprec);
  fract.a = 0;
  for (int i = 0; i < fprec; i++) {
    d*=10;
    fract.a *= 10;
    fract.a += int(d);
    d -= floor(d);
  }
  return fract + integ;
}

void Fraction::get(DataElement* data) {
  DataElement* ae = new DataElement();
  ae->_core->fromType(a);
  data->addChild(ae);

  DataElement* be = new DataElement();
  be->_core->fromType(b);
  data->addChild(be);
}
void Fraction::set(DataElement* data) {
  a = data->_children[0]->_core->toType<cBigNumber>();
  b = data->_children[1]->_core->toType<cBigNumber>();
}

Fraction Fraction::operator+=(const Fraction rhs) {
  a = a*rhs.b + rhs.a*b;
  b = b*rhs.b;
  return simplify();
}
Fraction Fraction::operator-=(const Fraction rhs) {
  a = a*rhs.b - rhs.a*b;
  b = b*rhs.b;
  return simplify();
}
Fraction Fraction::operator*=(const Fraction rhs) {
  a = a*rhs.a;
  b = b*rhs.b;
  return simplify();
}
Fraction Fraction::operator/=(const Fraction rhs) {
  a = a*rhs.b;
  b = b*rhs.a;
  return simplify();
}
Fraction Fraction::operator%=(const Fraction rhs) {
  Fraction div = *this;
  div /= rhs;
  cBigNumber intdiv = div.a / div.b;
  *this -= rhs*intdiv;
  return simplify();
}

Fraction operator+ (const Fraction x, const Fraction y) {
  return Fraction(x) += y;
}
Fraction operator- (const Fraction x, const Fraction y) {
  return Fraction(x) -= y;
}
Fraction operator- (const Fraction x) {
  return Fraction(x) *= -1;
}
Fraction operator* (const Fraction x, const Fraction y) {
  return Fraction(x) *= y;
}
Fraction operator/ (const Fraction x, const Fraction y) {
  return Fraction(x) /= y;
}

bool operator==(const Fraction lhs, const Fraction rhs) {
  return (lhs.a*rhs.b == lhs.b*rhs.a);
}
bool operator< (const Fraction lhs, const Fraction rhs) {
  return (lhs.a*rhs.b < lhs.b*rhs.a);
}
bool operator> (const Fraction lhs, const Fraction rhs) {
  return (lhs.a*rhs.b > lhs.b*rhs.a);
}
bool operator<=(const Fraction lhs, const Fraction rhs) {
  return (lhs.a*rhs.b <= lhs.b*rhs.a);
}
bool operator>=(const Fraction lhs, const Fraction rhs) {
  return (lhs.a*rhs.b >= lhs.b*rhs.a);
}

string to_string(const Fraction _val) {
  return _val.toStr(8);
}
double toDouble(const Fraction _val) {
  return _val.toDouble();
}
Fraction abs(const Fraction _val) {
  Fraction n = _val;
  n.a = n.a.abs();
  n.b = n.b.abs();
  return n;
}
cBigNumber floor(const Fraction _val) {
  return _val.a / _val.b;
}
cBigNumber ceil(const Fraction _val) {
  return -floor(-_val);
}


ostream& operator<<(ostream& o, const Fraction _val) {
  return o << _val.toDouble();
}
istream& operator>>(istream& i, Fraction& _val) {
  string s;
  i >> s;
  _val = Fraction(s);
  return i;
}

template <>
long double to_doubleT<long double, Fraction>(Fraction _val) {
  return _val.toDouble();
}
template <>
double to_doubleT<double, Fraction>(Fraction _val) {
  return double(_val.toDouble());
}
template <>
float to_doubleT<float, Fraction>(Fraction _val) {
  return float(_val.toDouble());
}