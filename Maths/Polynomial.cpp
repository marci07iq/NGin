#include "Polynomial.h"
/*
template<typename T> Polynomial<T>::Polynomial() {
	this->degree = 0;
	this->Coefficient = vector<Fraction(0);
}

template<typename T> Polynomial<T>::Polynomial(int d) {
	this->Coefficient.resize(d + 1);
	this->degree = d;
	for (int i = 0; i <= d; ++i) {
		this->Coefficient[i] = 0;
	}
}

template<typename T> Polynomial<T>::Polynomial(vector<T> c) {
	this->degree = c.size() - 1;
	this->Coefficient = c;
}

template<typename T> Polynomial<T>::Polynomial(T a, int b) {
	this->Coefficient.resize(b + 1);
	this->degree = b;
	this->Coefficient[this->degree] = a;
}

template<typename T> Polynomial<T> operator+ (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p3(max(lhs.degree, rhs.degree));
	for (int i = 0; i <= p3.degree; i++) {
		p3.Coefficient[i] = ((lhs.Coefficient.size() > i) ? lhs.Coefficient[i] : 0) + ((rhs.Coefficient.size() > i) ? rhs.Coefficient[i] : 0);
	}
	return (p3).Trim();
}
template<typename T> Polynomial<T> operator- (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p3(max(lhs.degree, rhs.degree));
	for (int i = 0; i <= p3.degree; i++) {
		p3.Coefficient[i] = ((lhs.Coefficient.size() > i) ? lhs.Coefficient[i] : 0) - ((rhs.Coefficient.size() > i) ? rhs.Coefficient[i] : 0);
	}
	return (p3).Trim();
}
template<typename T> Polynomial<T> operator* (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p3(lhs.degree + rhs.degree);

	for (int i = 0; i <= lhs.degree; i++) {
		for (int j = 0; j <= rhs.degree; j++) {
			p3.Coefficient[i + j] = p3.Coefficient[i + j] + lhs.Coefficient[i] * rhs.Coefficient[j];
		}
	}
	return (p3).Trim();
}

template<typename T, typename U> Polynomial<T> operator* (Polynomial<T> lhs, U rhs) {
	Polynomial p3(lhs.degree);

	for (int i = 0; i <= lhs.degree; i++) {
		p3.Coefficient[i] = lhs.Coefficient[i] * rhs;
	}

	return (p3).Trim();
}
template<typename T, typename U> Polynomial<T> operator* (U lhs, Polynomial<T> rhs) {
  Polynomial p3(rhs.degree);

  for (int i = 0; i <= rhs.degree; i++) {
    p3.Coefficient[i] = rhs.Coefficient[i] * lhs;
  }

  return (p3).Trim();
}

template<typename T> Polynomial<T> operator/(Polynomial<T> lhs, Polynomial<T> rhs) {
	return PolyDiv(lhs, rhs).first;
}
template<typename T> Polynomial<T> operator%(Polynomial<T> lhs, Polynomial<T> rhs) {
	return PolyDiv(lhs, rhs).second;
}

template<typename T> bool operator> (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p = (lhs - rhs).Trim();
	if (p.degree == 0) {
		return false;
	}
	if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b > 0) {
		return true;
	}
	return false;
}
template<typename T> bool operator< (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p = (lhs - rhs).Trim();
	if (p.degree == 0) {
		return false;
	}
	if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b < 0) {
		return true;
	}
	return false;
}
template<typename T> bool operator>= (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p = (lhs - rhs).Trim();
	if (p.degree == 0) {
		return true;
	}
	if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b > 0) {
		return true;
	}
	return false;
}
template<typename T> bool operator<= (Polynomial<T> lhs, Polynomial<T> rhs) {
	Polynomial p = (lhs - rhs).Trim();
	if (p.degree == 0) {
		return true;
	}
	if (p.Coefficient[p.Coefficient.size() - 1].a*p.Coefficient[p.Coefficient.size() - 1].b < 0) {
		return true;
	}
	return false;
}


template<typename T> istream& operator>>(istream& i, Polynomial<T> &p) {
	stack<Fraction s;
	string h;
	char c = ' ';
	while (c != '\n') {

		i.get(c);
		if (c == ' ' || c == '\n') {
			s.push(Fractionh));
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

template<typename T> pair<Polynomial<T>, Polynomial<T>> PolyDiv(Polynomial<T> lhs, Polynomial<T> rhs) {
	pair<Polynomial, Polynomial> p = make_pair(Polynomial(), Polynomial());

	if (lhs.Coefficient.size()<rhs.Coefficient.size()){
		p = make_pair(Polynomial(), lhs);
	}
	else {
		Polynomial q(Fraction1), lhs.degree - rhs.degree);
		Polynomial nlhs = (lhs - q*(rhs*(lhs.Coefficient[lhs.degree] / rhs.Coefficient[rhs.degree]))).Trim();
		p = PolyDiv(nlhs, rhs);
		p.first = p.first + Polynomial(lhs.Coefficient[lhs.degree] / rhs.Coefficient[rhs.degree], lhs.degree - rhs.degree);
	}
	return p;
}

template<typename T> Polynomial<T> Polynomial<T>::Trim() {
	Polynomial p = *this;
	while (p.Coefficient.size() > 0 && p.Coefficient[p.Coefficient.size() - 1] == Fraction0)) {
		p.degree--;
		p.Coefficient.resize(p.Coefficient.size() - 1);

	}
	return p;
}

template<typename T> T Polynomial<T>::at(T value) {
  float val = 0;
  for (int i = 0; i < this->Coefficient.size(); i++) {
    val += this->Coefficient[i] * pow(value, i);
  }
  return val
}*/
/*
template<typename T> Term<T>::Term() {

}
template<typename T> Term<T>::Term(string s) {
  for (int i = 0; i < s.length(); i++) {
    terms[s[i]]++;
  }
}

template<typename T> bool operator== (Term<T>& lhs, Term<T>& rhs) {
  auto it = lhs.terms.begin();
  while (it != lhs.terms.end()) {
    if (rhs[it->first] != it->second) {
      return false;
    }
  }
  auto it = rhs.terms.begin();
  while (it != rhs.terms.end()) {
    if (lhs[it->first] != it->second) {
      reutrn false;
    }
  }
  return true;
}

template<typename T> Term<T> operator* (Term<T>& lhs, Term<T>& rhs) {
  Term<T> res;
  res.coefficient = lhs.coefficient * rhs.coefficient;

  auto it = lhs.terms.begin();
  while (it != lhs.terms.end()) {
    res[*it->first] += *it->second;
  }
  auto it = rhs.terms.begin();
  while (it != rhs.terms.end()) {
    res[*it->first] += *it->second;
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
      it->coefficient += coefficient;
      b = false;
    }
    ++it;
  }
  if (b) {
    terms.push_back(t);
  }
}
template<typename T> void Equation<T>::addTerm(T coefficient, string sterms) {
  Term<T> t(sterms);

  t.coefficient = coefficient;
  
  addTerm(t);
}
template<typename T> void Equation<T>::addTerms(vector<pair<T, string> > newterms) {
  for (int i = 0; i < newterms.size(); i++) {
    addTerm(newterms[i].first, newterms[i].second);
  }
}
template<typename T> void Equation<T>::addTerms(Equation& newterms) {
  auto it = newterms.begin();
  while (it != newterms.end()) {
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
}

template<typename T> typename list<Term<T>>::iterator Equation<T>::getFirstTerm(char t) {
  int res = 0;
  auto it = terms.begin();
  while (it != terms.end()) {
    if (it->terms[t] != 0) {
      return it;
    }
  }
}

template<typename T> void Equation<T>::substitute(Equation<T> eq, char t) {
  if (countTerms(t) != 1) {
    throw 1; //Cant substitute non-linear.
  }
  auto it = getFirstTerm(t);
  Term test(t + "");
  if (*it != Term) {
    throw 1; //Cant substitute with other side terms
  }
  eq /= it->coefficient;
  eq.terms.erase(it);

  auto it2 = terms.begin();

  while(it2 != terms.end()) {
    Equation<T> eq2({1,""});
    for (int i = 0; i < it2->getPower(t)) {
      eq2 = eq2 * eq;
    }
    it2->coefficients[t] = 0;
    eq2 = eq2 * *it2;
    terms.erase(it2);

    addTerms(eq2);

    ++it;
  }
}

template<typename T> Equation<T> operator+(Equation<T>& lhs, Equation<T>& rhs) {
  Equation<T> res = lhs;
  res.addTerms(rhs);
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, Term<T>& rhs) {
  Equation<T> res;
  for (auto itl : lhs.terms) {
    res.addTerm((*itl) * itr);
  }
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, Equation<T>& rhs) {
  Equation<T> res;
  for (auto it : rhs.terms) {
    res += lhs * *it;
  }
  return res;
}
template<typename T> Equation<T> operator*(Equation<T>& lhs, T& rhs) {
  Equation<T> eq = lhs;
  auto it = eq.terms.begin();

  while (it != eq.terms.end()) {
    it->coefficient *= rhs;
    ++it;
  }
}
template<typename T> Equation<T> operator/(Equation<T>& lhs, T& rhs) {
  Equation<T> eq = lhs;
  auto it = eq.terms.begin();

  while (it != eq.terms.end()) {
    it->coefficient /= rhs;
    ++it;
  }
}
*/