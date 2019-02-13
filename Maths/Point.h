#pragma once

#include "Types.h"

//#include "../GL_Lib/glm/glm.hpp"
//#include "../GL_Lib/glm/mat4x4.hpp"
//#include "../GL_Lib/glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
//#include "../GL_Lib/glm/gtc/type_ptr.hpp"

template<typename T> class vec3;
//typedef vec2<float>           fVec3;
typedef vec3<int>               iVec3;
typedef vec3<bool>              bVec3;
typedef vec3<distance_type_m>   mVec3;
typedef vec3<vel_type_mpers>    mpsVec3;
typedef vec3<acc_type_mperss>   mpssVec3;
typedef vec3<scalar_type>       sVec3;
typedef vec3<double>            fVec3;

template<typename T> class vec2;
//typedef vec2<float>           fVec2;
typedef vec2<int>               iVec2;
typedef vec2<bool>              bVec2;
typedef vec2<distance_type_m>   mVec2;
typedef vec2<vel_type_mpers>    mpsVec2;
typedef vec2<acc_type_mperss>   mpssVec2;
typedef vec2<scalar_type>       sVec2;
typedef vec2<double>            fVec2;


template<typename T>
class vec3 {
public:
  enum Exception { WRONG_INDEX };

  T x;
  T y;
  T z;

  vec3() : x(0), y(0), z(0) {}
  vec3(T value) : x(value), y(value), z(value) {};
  vec3(T ax, T ay, T az) : x(ax), y(ay), z(az) {};
  template<typename U> vec3(vec3<U> c)
    : x(T(c.x)), y(T(c.y)), z(T(c.z)) {};

  vec3<T> & operator+=(const vec3<T> rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z;
    return *this;
  }
  vec3<T> & operator-=(const vec3<T> rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z;
    return *this;
  }
  vec3<T> & operator*=(const vec3<T> rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z;
    return *this;
  }
  vec3<T> & operator*=(const T rhs) {
    x *= rhs; y *= rhs; z *= rhs;
    return *this;
  }
  vec3<T> & operator/=(const vec3<T> rhs) {
    x /= rhs.x; y /= rhs.y; z /= rhs.z;
    return *this;
  }
  vec3<T> & operator/=(const T rhs) {
    x /= rhs; y /= rhs; z /= rhs;
    return *this;
  }
  vec3<T> & operator%=(const vec3<T> rhs) {
    x %= rhs.x; y %= rhs.y; z %= rhs.z;
    return *this;
  }
  vec3<T> & operator&=(const vec3<T> rhs) {
    x &= rhs.x; y &= rhs.y; z &= rhs.z;
    return *this;
  }
  vec3<T> & operator|=(const vec3<T> rhs) {
    x |= rhs.x; y |= rhs.y; z |= rhs.z;
    return *this;
  }
  vec3<T> & operator^=(const vec3<T> rhs) {
    x ^= rhs.x; y ^= rhs.y; z ^= rhs.z;
    return *this;
  }
  /*vec3<T> & operator<<=(vec3<T> rhs) {
    x ^= rhs.x; y ^= rhs.y; z ^= rhs.z;
    return *this;
  }
  vec3<T> & operator>>=(vec3<T> rhs) {
    x ^= rhs.x; y ^= rhs.y; z ^= rhs.z;
    return *this;
  }*/

  vec3<T> operator+ (const vec3<T> rhs) const { return vec3<T>(*this) += rhs; }
  vec3<T> operator- (const vec3<T> rhs) const { return vec3<T>(*this) -= rhs; }
  vec3<T> operator* (const vec3<T> rhs) const { return vec3<T>(*this) *= rhs; }
  vec3<T> operator* (const T rhs) const { return vec3<T>(*this) *= rhs; }
  vec3<T> operator/ (const vec3<T> rhs) const { return vec3<T>(*this) /= rhs; }
  vec3<T> operator/ (const T rhs) const { return vec3<T>(*this) /= rhs; }
  vec3<T> operator% (const vec3<T> rhs) const { return vec3<T>(*this) %= rhs; }
  vec3<T> operator& (const vec3<T> rhs) const { return vec3<T>(*this) &= rhs; }
  vec3<T> operator| (const vec3<T> rhs) const { return vec3<T>(*this) |= rhs; }
  vec3<T> operator^ (const vec3<T> rhs) const { return vec3<T>(*this) ^= rhs; }
  /*vec3<T> operator<<(vec3<T> rhs) const { return vec3<T>(*this) <<= rhs; }
  vec3<T> operator >> (vec3<T> rhs) const { return vec3<T>(*this) >>= rhs; }*/

  vec3<T> operator+() const { return vec3<T>(+x, +y, +z); }
  vec3<T> operator-() const { return vec3<T>(-x, -y, -z); }
  vec3<T> operator~() const { return vec3<T>(~x, ~y, ~z); }
  vec3<T> operator!() const { return vec3<T>(!x, !y, !z); }

  vec3<T> & operator++() { ++x, ++y, ++z; return *this; }
  vec3<T> & operator--() { --x, --y, --z; return *this; }
  vec3<T> operator++(int) { vec3<T> cp(*this); ++x, ++y, ++z; return cp; }
  vec3<T> operator--(int) { vec3<T> cp(*this); --x, --y, --z; return cp; }

  T& operator[](int n)
  {
    switch (n)
    {
    case 0:
      return x;
      break;

    case 1:
      return y;
      break;

    case 2:
      return z;
      break;

    default:
      throw WRONG_INDEX;
    }
  }
  /*const T & operator[](int ix) const {
    return ix == 0 ? x : ix == 1 ? y : ix == 2 ? z : 0;
  }*/
  T sqrlen() const {
    return x*x + y*y + z*z;
  }
  longDouble length() const {
    return sqrt(this->sqrlen());
  }
  vec3<T> norm() {
    *this /= length();
    return *this;
  }

  bVec3 operator==(vec3<T> rhs) const {
    return bVec3(x == rhs.x, y == rhs.y, z == rhs.z);
  }
  bVec3 operator!=(vec3<T> rhs) const {
    return bVec3(x != rhs.x, y != rhs.y, z != rhs.z);
  }
  bVec3 operator<(vec3<T> rhs) const {
    return bVec3(x < rhs.x, y < rhs.y, z < rhs.z);
  }
  bVec3 operator<=(vec3<T> rhs) const {
    return bVec3(x <= rhs.x, y <= rhs.y, z <= rhs.z);
  }
  bVec3 operator>=(vec3<T> rhs) const {
    return bVec3(x >= rhs.x, y >= rhs.y, z >= rhs.z);
  }
  bVec3 operator>(vec3<T> rhs) const {
    return bVec3(x > rhs.x, y > rhs.y, z > rhs.z);
  }

  fVec3 at(float t) { //Only for polynomial vectors
    return {to_double(x.at(t)), to_double(y.at(t)), to_double(z.at(t))};
  }

  vec3<T> randomize(T r)
  {
    //I am not sure if this is random
    /*int distFromO = ((float) rand() / RAND_MAX  - 0.5) * MAP_SIZE;
    this->x = (float) rand() / RAND_MAX - 0.5;
    float angle = (float) rand() / (RAND_MAX/(2*PI));
    this->y = cos(angle);
    this->z = sin(angle);
    (*this) = (*this)*distFromO;*/

    //this certainly is random, but slow
    bool notEnd;
    do
    {
      for (int i = 0; i <= 2; i++)
      {
        (*this)[i] = ((float)rand() / RAND_MAX - 0.5) * r;
      }
      notEnd = this->length() > r / 2;
    } while (notEnd);
    return *this;
  }

  void get(DataElement* data) {
    DataElement* xx = new DataElement();
    xx->_core->fromType<T>(x);
    data->addChild(xx);

    DataElement* yy = new DataElement();
    yy->_core->fromType<T>(y);
    data->addChild(yy);

    DataElement* zz = new DataElement();
    zz->_core->fromType<T>(z);
    data->addChild(zz);
  }
  void set(const DataElement* data) {
    x = data->_children[0]->_core->toType<T>();
    y = data->_children[1]->_core->toType<T>();
    z = data->_children[2]->_core->toType<T>();
  }

  void load(xml_node<> *me) {
    x = strTo<T>(me->first_node("x")->value());
    y = strTo<T>(me->first_node("y")->value());
    z = strTo<T>(me->first_node("z")->value());
  }

  T maxV() {
    T res = x;
    res = (res < y) ? y : res;
    res = (res < z) ? z : res;
    return res;
  }
  T minV() {
    T res = x;
    res = (res > y) ? y : res;
    res = (res > z) ? z : res;
    return res;
  }
};

template<typename T>
vec3<T> sqrtVec(vec3<T> what) {
  return{ sqrt(what.x), sqrt(what.y), sqrt(what.z) };
}

template<typename T>
vec3<T> rotateVecZ(vec3<T> what, float with) {
  return {cos(with) * what.x - sin(with) * what.y, sin(with) * what.x + cos(with) * what.y, what.z };
}

struct polar_vec3 {
  double r;
  double phi;
  double theta;
  void reNormalize() {
    phi = modulo(phi, 2* CONS_PI);
    theta = min(max(theta, 0.01), CONS_PI - 0.01);
  }
  fVec3 toCartesian() {
    return {r * cos(phi) * sin(theta), r * sin(phi) * sin(theta), r * cos(theta)};
  }
  void fromCartesian(fVec3 val) {
    r = val.length();
    phi = 0;
    double tr = fVec3({val.x, val.y, 0}).length();
    if (tr > 0) {
      phi = atan2(val.y, val.x);
    }
    theta = 0;
    if (r > 0) {
      theta = atan2(tr, val.z);
    }
  }
};

template<typename T>
class vec2 {
public:
  enum Exception { WRONG_INDEX };

  T x;
  T y;

  vec2() : x(0), y(0) {}
  vec2(T value) : x(value), y(value) {};
  vec2(T ax, T ay) : x(ax), y(ay) {};
  template<typename U> vec2(vec2<U> c)
    : x(T(c.x)), y(T(c.y)) {
  };

  vec2<T> & operator+=(vec2<T> rhs) {
    x += rhs.x; y += rhs.y;
    return *this;
  }
  vec2<T> & operator-=(vec2<T> rhs) {
    x -= rhs.x; y -= rhs.y;
    return *this;
  }
  vec2<T> & operator*=(vec2<T> rhs) {
    x *= rhs.x; y *= rhs.y;
    return *this;
  }
  vec2<T> & operator*=(T rhs) {
    x *= rhs; y *= rhs;
    return *this;
  }
  vec2<T> & operator/=(vec2<T> rhs) {
    x /= rhs.x; y /= rhs.y;
    return *this;
  }
  vec2<T> & operator/=(T rhs) {
    x /= rhs; y /= rhs;
    return *this;
  }
  vec2<T> & operator%=(vec2<T> rhs) {
    x %= rhs.x; y %= rhs.y;
    return *this;
  }
  vec2<T> & operator&=(vec2<T> rhs) {
    x &= rhs.x; y &= rhs.y;
    return *this;
  }
  vec2<T> & operator|=(vec2<T> rhs) {
    x |= rhs.x; y |= rhs.y;
    return *this;
  }
  vec2<T> & operator^=(vec2<T> rhs) {
    x ^= rhs.x; y ^= rhs.y;
    return *this;
  }
  /*vec2<T> & operator<<=(vec2<T> rhs) {
  x ^= rhs.x; y ^= rhs.y; z ^= rhs.z;
  return *this;
  }
  vec2<T> & operator>>=(vec2<T> rhs) {
  x ^= rhs.x; y ^= rhs.y; z ^= rhs.z;
  return *this;
  }*/

  vec2<T> operator+ (vec2<T> rhs) const { return vec2<T>(*this) += rhs; }
  vec2<T> operator- (vec2<T> rhs) const { return vec2<T>(*this) -= rhs; }
  vec2<T> operator* (vec2<T> rhs) const { return vec2<T>(*this) *= rhs; }
  vec2<T> operator* (T rhs) const { return vec2<T>(*this) *= rhs; }
  vec2<T> operator/ (vec2<T> rhs) const { return vec2<T>(*this) /= rhs; }
  vec2<T> operator/ (T rhs) const { return vec2<T>(*this) /= rhs; }
  vec2<T> operator% (vec2<T> rhs) const { return vec2<T>(*this) %= rhs; }
  vec2<T> operator& (vec2<T> rhs) const { return vec2<T>(*this) &= rhs; }
  vec2<T> operator| (vec2<T> rhs) const { return vec2<T>(*this) |= rhs; }
  vec2<T> operator^ (vec2<T> rhs) const { return vec2<T>(*this) ^= rhs; }
  /*vec2<T> operator<<(vec2<T> rhs) const { return vec2<T>(*this) <<= rhs; }
  vec2<T> operator >> (vec2<T> rhs) const { return vec2<T>(*this) >>= rhs; }*/

  vec2<T> operator+() const { return vec2<T>(+x, +y); }
  vec2<T> operator-() const { return vec2<T>(-x, -y); }
  vec2<T> operator~() const { return vec2<T>(~x, ~y); }
  vec2<T> operator!() const { return vec2<T>(!x, !y); }

  vec2<T> & operator++() { ++x, ++y; return *this; }
  vec2<T> & operator--() { --x, --y; return *this; }
  vec2<T> operator++(int) { vec2<T> cp(*this); ++x, ++y; return cp; }
  vec2<T> operator--(int) { vec2<T> cp(*this); --x, --y; return cp; }

  T& operator[](int n) {
    switch (n) {
      case 0:
        return x;
        break;

      case 1:
        return y;
        break;

      default:
        throw WRONG_INDEX;
    }
  }
  /*const T & operator[](int ix) const {
  return ix == 0 ? x : ix == 1 ? y : ix == 2 ? z : 0;
  }*/
  T sqrlen() const {
    return x*x + y*y;
  }
  longDouble length() const {
    return sqrt(this->sqrlen());
  }
  vec2<T> norm() {
    *this /= length();
    return *this;
  }

  bVec2 operator==(vec2<T> rhs) const {
    return bVec2(x == rhs.x, y == rhs.y);
  }
  bVec2 operator!=(vec2<T> rhs) const {
    return bVec2(x != rhs.x, y != rhs.y);
  }
  bVec2 operator<(vec2<T> rhs) const {
    return bVec2(x < rhs.x, y < rhs.y);
  }
  bVec2 operator<=(vec2<T> rhs) const {
    return bVec2(x <= rhs.x, y <= rhs.y);
  }
  bVec2 operator>=(vec2<T> rhs) const {
    return bVec2(x >= rhs.x, y >= rhs.y);
  }
  bVec2 operator>(vec2<T> rhs) const {
    return bVec2(x > rhs.x, y > rhs.y);
  }

  fVec2 at(float t) { //Only for polynomial vectors
    return{ to_double(x.at(t)), to_double(y.at(t)), to_double(z.at(t)) };
  }

  vec2<T> randomize(T r) {
    //I am not sure if this is random
    /*int distFromO = ((float) rand() / RAND_MAX  - 0.5) * MAP_SIZE;
    this->x = (float) rand() / RAND_MAX - 0.5;
    float angle = (float) rand() / (RAND_MAX/(2*PI));
    this->y = cos(angle);
    this->z = sin(angle);
    (*this) = (*this)*distFromO;*/

    //this certainly is random, but slow
    bool notEnd;
    do {
      for (int i = 0; i <= 2; i++) {
        (*this)[i] = ((float)rand() / RAND_MAX - 0.5) * r;
      }
      notEnd = this->length() > r / 2;
    } while (notEnd);
    return *this;
  }

  void get(DataElement* data) {
    DataElement* xx = new DataElement();
    xx->_core->fromType<T>(x);
    data->addChild(xx);

    DataElement* yy = new DataElement();
    yy->_core->fromType<T>(y);
    data->addChild(yy);
  }
  void set(DataElement* data) {
    x = data->_children[0]->_core->toType<T>();
    y = data->_children[1]->_core->toType<T>();
  }

  void load(xml_node<> *me) {
    x = strTo<T>(me->first_node("x")->value());
    y = strTo<T>(me->first_node("y")->value());
  }
};


inline bool band(bVec3 v) { return v.x && v.y && v.z; }
inline bool bor(bVec3 v) { return v.x || v.y || v.z; }
inline bool bxor(bVec3 v) { return v.x != v.y != v.z; }

inline bool band(bVec2 v) { return v.x && v.y; }
inline bool bor(bVec2 v) { return v.x || v.y; }
inline bool bxor(bVec2 v) { return v.x != v.y; }


template<typename T> std::ostream& operator<<(std::ostream& os, const vec3<T>& v)
{
  //throw 1;
  os << v.x << " " << v.y << " " << v.z;
  return os;
}
template<typename T> std::istream& operator>> <>(std::istream& is, vec3<T>& v)
{
  //throw 1;
  is >> v.x >> v.y >> v.z;
  return is;
}

template<typename T> std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
  os << v.x << " " << v.y;
  return os;
}
template<typename T> std::istream& operator>> <>(std::istream& is, vec2<T>& v) {
  is >> v.x >> v.y;
  return is;
}


template<typename T> inline T dot(vec2<T> lhs, vec2<T> rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T> inline T dot(vec3<T> lhs, vec3<T> rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
template<typename T> inline vec3<T> crs(vec3<T> lhs, vec3<T> rhs) {
  return vec3<T>(lhs.y*rhs.z - lhs.z*rhs.y,
    lhs.z*rhs.x - lhs.x*rhs.z,
    lhs.x*rhs.y - lhs.y*rhs.x);
}


template<typename T> inline float distance(vec2<T> lhs, vec2<T> rhs) {
  return (lhs - rhs).length();
}

template<typename T> inline float distance(vec3<T> lhs, vec3<T> rhs) {
  return (lhs-rhs).length();
}

template<typename T> vector<vec3<T>> randstartpos(T radius)
{
  T modrad = radius*(0.25*ran1() + 0.5);
  T modx1 = radius*ran1() / 8;
  T modx2 = radius*ran1() / 8;
  T mody1 = radius*ran1() / 8;
  T mody2 = radius*ran1() / 8;
  T modz1 = radius*ran1() / 8;
  T modz2 = radius*ran1() / 8;

  vec3<T> firstship(modrad + modx1, modrad + mody1, modrad + modz1);
  vec3<T> secondship(-modrad - modx2, -modrad - mody2, -modrad - modz2);
  return {firstship, secondship};
}

template<typename T> vec3<T> vecSwitch(bVec3& s, vec3<T> lhs, vec3<T> rhs) {
  return {
  s.x ? lhs.x : rhs.x,
  s.y ? lhs.y : rhs.y,
  s.z ? lhs.z : rhs.z
  };
}

template<typename T>
class Matrix4 {
public:
  T _vals[16];
  //glm::mat<4, 4, T, glm::defaultp> _base;

  inline T& at(int i, int j) {
    return _vals[4*i + j];
  }

  void setNull() {
    for (int i = 0; i < 16; i++) {
      _vals[i] = 0;
    }
    //_base = glm::zero<decltype(_base)>();
  }

  void setIdentity() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        at(i, j) = (i == j) ? 1: 0;
      }
    }
    //_base = glm::identity<decltype(_base)>();
  }

  Matrix4 & operator*(Matrix4 rhs) {
    Matrix4f res;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        res.at(i, j) = 0;
        for (int k = 0; k < 4; k++) {
          res.at(i, j) += at(i, k) * rhs.at(k, j);
        }
      }
    }
    return res;
    //return _base * rhs._base;
  }

  void set(float from[16]) {
    for (int i = 0; i < 16; i++) {
      _vals[i] = from[i];
    }
  }
  void transpose() {
    for (int i = 0; i < 4; i++) {
      for (int j = i + 1; j < 4; j++) {
        swap(at(i, j), at(j, i));
      }
    }
    //_base = glm::transpose(_base);
  }

  Matrix4() {
    setNull();
  }
  const Matrix4<T> invert() {
    Eigen::Matrix<T, 4, 4, Eigen::RowMajor> inv = Eigen::Map<Eigen::Matrix<T, 4, 4, Eigen::RowMajor>>(_vals).inverse();
    T* inv_T = inv.data();
    Matrix4<T> res;
    for (int i = 0; i < 16; i++) {
      res._vals[i] = inv_T[i];
    }
    return res;
  }
};

typedef Matrix4<int> Matrix4i;
typedef Matrix4<float> Matrix4f;
typedef Matrix4<double> Matrix4d;