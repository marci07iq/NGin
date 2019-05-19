#pragma once

#include "helperFunctions.h"
#include "Transpose.h"

template<typename T> class Polynomial;

typedef Polynomial<int> PolynomialI;
typedef Polynomial<float> PolynomialF;
typedef Polynomial<long double> PolynomialD;
//typedef Polynomial<Fraction> PolynomialR;

template<typename T>
class Polynomial{
public:
	vector<T> Coefficient;
	size_t degree;

	Polynomial() {
    this->degree = 0;
    this->Coefficient = vector<T>(0);
  }
	Polynomial(int d) {
    this->Coefficient.resize(d + 1);
    this->degree = d;
    for (int i = 0; i <= d; ++i) {
      this->Coefficient[i] = 0;
    }
  }
	Polynomial(vector<T> c) {
    this->degree = c.size() - 1;
    this->Coefficient = c;
  }
	Polynomial(T a, int b) {
    this->Coefficient.resize(b + 1);
    this->degree = b;
    this->Coefficient[this->degree] = a;
  }
	Polynomial Trim() {
    Polynomial p = *this;
    while (p.Coefficient.size() > 1 && p.Coefficient[p.Coefficient.size() - 1] == 0) {
      p.degree--;
      p.Coefficient.resize(p.Coefficient.size() - 1);

    }
    return p;
  }

  T at(T val) {
    T value = Coefficient[degree];
    for (int i = degree-1; i >= 0; i--) {
      value = val * value + Coefficient[i];
    }
    return value;
  }
  Polynomial<T> derivative() {
    Polynomial<T> d(degree-1);;
    for (int i = 1; i <= degree; ++i) {
      d.Coefficient[i-1] = i * Coefficient[i];
    }
    return d;
  }
  Polynomial<T> eliminateRoot(T root)
  {
    Polynomial<T> temp(Coefficient);
    vector<T> coeffs(temp.degree, 0);

    for (int i = this->degree; i >= 1; i--)
    {
      coeffs[i - 1] = temp.Coefficient[i];
      temp.Coefficient[i - 1] = temp.Coefficient[i - 1] + root*temp.Coefficient[i];
      temp.Coefficient[i] = 0;
    }

    return Polynomial<T>(coeffs);
  }

  vector<T> solve() {
    if (degree > 2) {
      return solveInterval();
    }
    /*if (degree == 4) {
      return solveQuartic(Coefficient[4], Coefficient[3], Coefficient[2], Coefficient[1], Coefficient[0]);
    }
    if (degree == 3) {
      return solveCubic(Coefficient[3], Coefficient[2], Coefficient[1], Coefficient[0]);
    }*/
    if (degree == 2) {
      return solveQuadratic<T>(Coefficient[2], Coefficient[1], Coefficient[0]);
    }
    if (degree == 1) {
      return {- Coefficient[0] / Coefficient[1]};
    }
    return {};
  }
  T minimize() { //not counting lim +- infty
    T res = 0;
    if (degree > 0) {
      vector<T> ders = derivative().solve();
      for (auto&& it : ders) {
        if (at(it) < at(res)) {
          res = it;
        }
      }
    }
    else if (degree == 0) {
      res = Coefficient[0];
    }
    return res;
  }
  T maximize() { //not counting lim +- infty
    vector<T> ders = derivative().solve();
    T res = 0;
    for (auto&& it : ders) {
      if (at(it) > at(res)) {
        res = it;
      }
    }
    return res;
  }

  T find_0_to_deg_1_max()
  {
    T A = abs(this->Coefficient[0]);
    for (int i = 0; i < this->degree; i++)
    {
      if (A < abs(this->Coefficient[i]))
      {
        A = abs(this->Coefficient[i]);
      }
    }
    return A;
  }
  T find_1_to_deg_max()
  {
    T B = abs(this->Coefficient[1]);
    for (int i = 1; i <= this->degree; i++)
    {
      if (B < abs(this->Coefficient[i]))
      {
        B = abs(this->Coefficient[i]);
      }
    }
    return B;
  }
  vector<T> solveNewton() {
    Polynomial<T> mec = *this;
    bool inside = true;
    vector<T> roots;
    int numoftries, tries = 0;
    double A, B, max, min, x, valofmin, valofmax, valofminneg, valofmaxneg, valofx, root;

    while (mec.degree > 1 && tries<MaxTries)
    {
      A = mec.find_0_to_deg_1_max();
      B = mec.find_1_to_deg_max();
      min = 1 / (1 + B / abs(mec.Coefficient[0]));
      max = 1 + A / abs(mec.Coefficient[mec.degree]);

      valofmin = mec.at(min);
      valofmax = mec.at(max);
      valofminneg = mec.at(-min);
      valofmaxneg = mec.at(-max);

      Polynomial<T> derivative = mec.derivative();

      if (abs(valofmin) < SolvePrec)
      {
        root = min;
      }
      else if (abs(valofmax) < SolvePrec)
      {
        root = max;
      }
      else if (abs(valofminneg) < SolvePrec)
      {
        root = -min;
      }
      else if (abs(valofmaxneg) < SolvePrec)
      {
        root = -max;
      }
      else
      {

        tries = 0;

        do
        {

          numoftries = 0;

          do
          {
            x = (max - min)*ran1() + min;
          } while (abs(derivative.at(x)) < 0.001);

          while ((abs(mec.at(x)) > SolvePrec) && inside)
          {
            x = x - mec.at(x) / derivative.at(x);
            numoftries++;
            if (numoftries > 1000)
            {
              inside = false;
            }
          }

          if (inside = true)
          {
            numoftries = 0;
            root = x;
          }
          else
          {
            inside = true;
            numoftries = 0;

            do
            {
              x = (-max + min)*ran1() - min;
            } while (abs(derivative.at(x)) > 0.001);

            while ((abs(mec.at(x)) > SolvePrec) && inside)
            {
              x = x - mec.at(x) / derivative.at(x);
              numoftries++;
              if (numoftries > 1000)
              {
                inside = false;
              }
            }

            if (inside = true)
            {
              numoftries = 0;
              root = x;
            }

          }

          tries++;

        } while ((abs(mec.at(root)) > SolvePrec) && tries<MaxTries);

      }

      if (tries < MaxTries)
      {
        roots.push_back(root);
        mec = mec.eliminateRoot(root);
      }

    }

    if (tries < MaxTries)
    {
      roots.push_back(-mec.Coefficient[0] / mec.Coefficient[1]);
    }
    return roots;
  }

  vector<T> solveInterval() {
    vector<T> roots;

    T RoucheBound = 2 + find_0_to_deg_1_max()/abs(Coefficient[degree]);
    T intervalmin = -RoucheBound;
    T intervalmax = RoucheBound;

    vector<T> inflex = derivative().solve();
    inflex.push_back(intervalmin);
    inflex.push_back(intervalmax);

    sort(inflex.begin(), inflex.end());

    auto it = inflex.begin();
    auto nit = it;
    ++nit;
    
    while (nit != inflex.end()) {
      if (at(*it) * at(*nit) <= 0) { //Oppsite sign, search
        T min = *it;
        T max = *nit;
        T mid = (min + max) / 2;
        while ((max - min) > SolvePrec) {
          if (at(min) * at(mid) <= 0) { 
            max = mid;
          }
          else {
            min = mid;
          }
          mid = (min + max) / 2;
        }
        roots.push_back(mid);
        it = nit;
        ++nit;
      }
      else {
        if (abs(at(*nit)) < SolvePrec) {
          roots.push_back(*nit);
        }
        ++nit;
      }
    }
    return roots;
  }
};

template<typename T> Polynomial<T> operator+ (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p3(max(lhs.degree, rhs.degree));
  for (size_t i = 0; i <= p3.degree; i++) {
    p3.Coefficient[i] = ((lhs.Coefficient.size() > i) ? lhs.Coefficient[i] : 0) + ((rhs.Coefficient.size() > i) ? rhs.Coefficient[i] : 0);
  }
  return (p3).Trim();
}
template<typename T> Polynomial<T> operator- (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p3(max(lhs.degree, rhs.degree));
  for (int i = 0; i <= p3.degree; i++) {
    p3.Coefficient[i] = ((lhs.Coefficient.size() > i) ? lhs.Coefficient[i] : 0) - ((rhs.Coefficient.size() > i) ? rhs.Coefficient[i] : 0);
  }
  return (p3).Trim();
}

template<typename T> Polynomial<T> operator* (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p3(lhs.degree + rhs.degree);

  for (int i = 0; i <= lhs.degree; i++) {
    for (int j = 0; j <= rhs.degree; j++) {
      p3.Coefficient[i + j] = p3.Coefficient[i + j] + lhs.Coefficient[i] * rhs.Coefficient[j];
    }
  }
  return (p3).Trim();
}
template<typename T> vec3<Polynomial<T>> operator* (Polynomial<T> lhs, vec3<T> rhs) {
  vec3<Polynomial<T>> p3(lhs.degree);

  for(int j = 0; j< 3; j++) {
    for (int i = 0; i <= lhs.degree; i++) {
      p3[j].Coefficient[i] = rhs[j] * lhs.Coefficient[i];
    }
    p3[j].Trim();
  }

  return p3;
}
template<typename T> vec3<Polynomial<T>> operator* (vec3<T> lhs, Polynomial<T> rhs) {
  return rhs*lhs;
}
template<typename T> Polynomial<T> operator* (Polynomial<T> lhs, float rhs) {
  Polynomial<T> p3(lhs.degree);

  for (int i = 0; i <= lhs.degree; i++) {
    p3.Coefficient[i] = lhs.Coefficient[i] * rhs;
  }

  return (p3).Trim();
}
template<typename T> Polynomial<T> operator* (float lhs, Polynomial<T> rhs) {
  return rhs * lhs;
}

template<typename T> Polynomial<T> operator/(Polynomial<T> lhs, Polynomial<T> rhs) {
  return PolyDiv(lhs, rhs).first;
}
template<typename T> Polynomial<T> operator%(Polynomial<T> lhs, Polynomial<T> rhs) {
  return PolyDiv(lhs, rhs).second;
}

template<typename T> pair<Polynomial<T>, Polynomial<T>> PolyDiv(Polynomial<T> lhs, Polynomial<T> rhs) {
  pair<Polynomial<T>, Polynomial<T>> p = make_pair(Polynomial<T>(), Polynomial<T>());

  if (lhs.Coefficient.size()<rhs.Coefficient.size()) {
    p = make_pair(Polynomial<T>(), lhs);
  }
  else {
    Polynomial<T> q(T(1), lhs.degree - rhs.degree);
    Polynomial<T> nlhs = (lhs - q*(rhs*(lhs.Coefficient[lhs.degree] / rhs.Coefficient[rhs.degree]))).Trim();
    p = PolyDiv(nlhs, rhs);
    p.first = p.first + Polynomial<T>(lhs.Coefficient[lhs.degree] / rhs.Coefficient[rhs.degree], lhs.degree - rhs.degree);
  }
  return p;
}

template<typename T> bool operator> (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p = (lhs - rhs).Trim();
  if (p.degree == 0) {
    return false;
  }
  if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b > 0) {
    return true;
  }
  return false;
}
template<typename T> bool operator< (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p = (lhs - rhs).Trim();
  if (p.degree == 0) {
    return false;
  }
  if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b < 0) {
    return true;
  }
  return false;
}
template<typename T> bool operator>= (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p = (lhs - rhs).Trim();
  if (p.degree == 0) {
    return true;
  }
  if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b > 0) {
    return true;
  }
  return false;
}
template<typename T> bool operator<= (Polynomial<T> lhs, Polynomial<T> rhs) {
  Polynomial<T> p = (lhs - rhs).Trim();
  if (p.degree == 0) {
    return true;
  }
  if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b < 0) {
    return true;
  }
  return false;
}

template<typename T> istream& operator>>(istream& i, Polynomial<T> &p) {
  stack<Fraction> s;
  string h;
  char c = ' ';
  while (c != '\n') {

    i.get(c);
    if (c == ' ' || c == '\n') {
      s.push(Fraction(h));
      h.clear();
    }
    else {
      h.push_back(c);
    }
  }
  while (s.size()) {
    p.Coefficient.push_back(s.top());
    s.pop();
  }
  p.degree = p.Coefficient.size() - 1;
  return i;
}
template<typename T> ostream& operator<<(ostream& o, Polynomial<T>& p) {
  for (int i = p.degree; i > 0; --i) {
    if (p.Coefficient[i].a * p.Coefficient[i].b != 0) {
      o << p.Coefficient[i] << "x";

      if (i > 1) {
        o << '^' << i;
      }
    }
  }
  if (p.Coefficient.size()) {
    o << p.Coefficient[0] END;
  }
  return o;
}

template<typename T> struct Term {
public:
  T coefficient;
  std::map<char, int> terms;
  Term();
  Term(T t, string s);
  int getPower(char c) {
    return terms[c];
  }
  Polynomial<T> toPolynomial(char t) {
    Polynomial<T> res(coefficient, getPower(t));
    return res;
  }
};

template<typename T> bool operator== (Term<T>& lhs, Term<T>& rhs);

template<typename T> class Equation {
public:
  list<Term<T>> terms;
  Equation();
  Equation(vector<pair<T, string> > newterms);
  void addTerm(Term<T>& t);
  void addTerm(T coefficient, string terms);
  void addTerms(vector<pair<T, string> > newterms);
  void addTerms(Equation& newterms);

  int countTerms(char t);

  typename list<Term<T>>::iterator getFirstTerm(char t);

  void substitute(Equation<T> eq, char t);

  Polynomial<T> getPolynomial(char c);

  template<typename T> Equation<T>& operator+=(Equation<T>& rhs) {
    addTerms(rhs);
    return *this;
  }
  template<typename T> Equation<T>& operator*=(Equation<T>& rhs) {
    Equation<T> lhs = *this;
    terms.clear();
    auto it = lhs.terms.begin();
    while (it != lhs.terms.end()) {
      addTerms(operator*(rhs, *it));
      ++it;
    }
    return *this;
  }
  template<typename T> Equation<T>& operator*=(T& rhs) {
    auto it = terms.begin();

    while (it != terms.end()) {
      it->coefficient *= rhs;
      ++it;
    }
    return *this;
  }
  template<typename T> Equation<T>& operator*=(Term<T>& rhs) {
    auto itl = terms.begin();
    while (itl != terms.end()) {
      (*itl) = (*itl) * rhs;
      ++itl;
    }
    return *this;
  }
  template<typename T> Equation<T>& operator/=(T& rhs) {
    auto it = terms.begin();

    while (it != terms.end()) {
      it->coefficient /= rhs;
      ++it;
    }
    return *this;
  }

  template<typename T> Polynomial<T> getPolynomial(char x) {
    Polynomial<T> poly;
    auto it = terms.begin();

    while (it != terms.end()) {

      auto it2 = it->terms.begin();
      while (it2 != it->terms.end()) {
        if (it2->first != x && it2->second != 0) { //Unknown term
          throw 1;
        }
        else {
          poly += Polynomial<T>(it->coefficient, it2->second); //it->coefficient * x^it2->second
        }
        ++it2;
      }
      
      ++it;
    }
    return poly;
  }
};

template<typename T> Equation<T> operator+(Equation<T>& lhs, Equation<T>& rhs);
template<typename T> Equation<T> operator*(Equation<T>& lhs, Equation<T>& rhs);
template<typename T> Equation<T> operator*(Equation<T>& lhs, Term<T>& rhs);
template<typename T> Term<T> operator*(Term<T>& lhs, Term<T>& rhs);
//template<typename T> Equation<T> operator/(Equation<T>& lhs, T& rhs);
//template<typename T> Equation<T> operator*(Equation<T>& lhs, T& rhs);


template<typename T> Term<T>::Term() {

}
template<typename T> Term<T>::Term(T t, string s) {
  coefficient = t;
  for (size_t i = 0; i < s.length(); i++) {
    terms[s[i]]++;
  }
}

template<typename T> bool operator== (Term<T>& lhs, Term<T>& rhs) {
  auto it = lhs.terms.begin();
  while (it != lhs.terms.end()) {
    if (rhs.terms[it->first] != it->second) {
      return false;
    }
    ++it;
  }
  it = rhs.terms.begin();
  while (it != rhs.terms.end()) {
    if (lhs.terms[it->first] != it->second) {
      return false;
    }
    ++it;
  }
  return true;
}
template<typename T> bool operator!= (Term<T>& lhs, Term<T>& rhs) {
  return !(lhs == rhs);
}

template<typename T> Term<T> operator* (Term<T>& lhs, Term<T>& rhs) {
  Term<T> res;
  res.coefficient = lhs.coefficient * rhs.coefficient;

  auto it = lhs.terms.begin();
  while (it != lhs.terms.end()) {
    res.terms[it->first] += it->second;
    ++it;
  }
  it = rhs.terms.begin();
  while (it != rhs.terms.end()) {
    res.terms[it->first] += it->second;
    ++it;
  }
  return res;
}


template<typename T> Equation<T>::Equation() {

}
template<typename T> Equation<T>::Equation(vector<pair<T, string> > newterms) {
  addTerms(newterms);
}
template<typename T> void Equation<T>::addTerm(Term<T>& t) {
  auto it = terms.begin();
  bool b = true;
  while (b && it != terms.end()) {
    if (*it == t) {
      it->coefficient += t.coefficient;
      b = false;
    }
    ++it;
  }
  if (b) {
    terms.push_back(t);
  }
}
template<typename T> void Equation<T>::addTerm(T coefficient, string sterms) {
  Term<T> t(coefficient, sterms);

  addTerm(t);
}
template<typename T> void Equation<T>::addTerms(vector<pair<T, string> > newterms) {
  for (auto&& it : newterms) {
    addTerm(it.first, it.second);
  }
}
template<typename T> void Equation<T>::addTerms(Equation& newterms) {
  auto it = newterms.terms.begin();
  while (it != newterms.terms.end()) {
    addTerm(*it);
    ++it;
  }
}


template<typename T> int Equation<T>::countTerms(char t) {
  int res = 0;
  auto it = terms.begin();
  while (it != terms.end()) {
    res += it->terms[t];
    ++it;
  }
  return res;
}

template<typename T> typename list<Term<T>>::iterator Equation<T>::getFirstTerm(char t) {
  int res = 0;
  auto it = terms.begin();
  while (it != terms.end()) {
    if (it->terms[t] != 0) {
      return it;
    }
    ++it;
  }
  return it;
}

template<typename T> void Equation<T>::substitute(Equation<T> eq, char t) {
  Equation<T> oldeq = *this;
  terms.clear();
  if (eq.countTerms(t) != 1) {
    throw 1; //Cant substitute non-linear.
  }
  auto it = eq.getFirstTerm(t);
  Term<T> test(1, string(1, t));
  if (*it != test) {
    throw 1; //Cant substitute with other side terms
  }
  eq /= (it->coefficient);
  eq *= Term<T>{-1, ""};
  eq.terms.erase(it);

  auto it2 = oldeq.terms.begin();

  while (it2 != oldeq.terms.end()) {
    Equation<T> eq2({ { 1, "" } });
    for (int i = 0; i < it2->getPower(t); i++) {
      eq2 = eq2 * eq;
    }
    it2->terms[t] = 0;
    eq2 *= (*it2);
    addTerms(eq2);
    ++it2;
  }
}

template<typename T> Polynomial<T> Equation<T>::getPolynomial(char t) {

  Polynomial<T> res(0);

  auto it = terms.begin();

  while (it != terms.end()) {
    res = res + it->toPolynomial(t);
    ++it;
  }

  return res;
}


template<typename T> Equation<T> operator+(Equation<T>& lhs, Equation<T>& rhs) {
  Equation<T> res = lhs;
  res += rhs;
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, Term<T>& rhs) {
  Equation<T> res = lhs;
  res *= rhs;
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, Equation<T>& rhs) {
  Equation<T> res = lhs;
  res *= rhs;
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, T rhs) {
  Equation<T> res = lhs;
  res *= rhs;
  return res;
}
template<typename T> Equation<T> operator/(Equation<T>& lhs, T rhs) {
  Equation<T> res = lhs;
  res /= rhs;
  return res;
}

template<typename T>
class Eqnsys {
public:
  std::map<char, Equation<T> > eqns;
};
