#pragma once

#include "Polynomial.h"
 
template<typename T>
struct HasGetMethod
{
  template<typename U, void(U::*)(DataElement*)> struct SFINAE {};
  template<typename U> static char Test(SFINAE<U, &U::get>*);
  template<typename U> static int Test(...);
  static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

template<typename T>
struct HasSetMethod
{
  template<typename U, void(U::*)(DataElement*)> struct SFINAE {};
  template<typename U> static char Test(SFINAE<U, &U::set>*);
  template<typename U> static int Test(...);
  static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

template<typename V>
class value {
public:
  V _val;
  value() : _val(V()) {}
  value(V val) : _val(val) {}
  V operator()() {
    return _val;
  }
  
  template<typename T>
  V getAt(T _time) {
    return _val;
  }

  template<typename T> void set(DataElement* data, std::true_type)
  {
    _val.set(data);
  }
  template<typename T> void set(DataElement* data, std::false_type)
  {
    _val = data->_core->toType<V>();
  }
  void set(DataElement* data)
  {
    set<void>(data, std::integral_constant<bool, HasSetMethod<V>::Has>());
  }

  template<typename T>void get(DataElement* data, std::true_type)
  {
    _val.get(data);
  }
  template<typename T> void get(DataElement* data, std::false_type)
  {
    //_val.get(data);
    data->_core->fromType<V>(_val);
  }
  void get(DataElement* data)
  {
    get<void>(data, std::integral_constant<bool, HasGetMethod<V>::Has>());
  }
};


template<typename V>
bool operator ==(const value<V> &a, const value<V> &b) {
  return a._val == b._val;
}
template<typename V>
bool operator !=(const value<V> &a, const value<V> &b) {
  return a._val != b._val;
}
template<typename V>
bool operator <(const value<V> &a, const value<V> &b) {
  return a._val < b._val;
}
template<typename V>
bool operator >(const value<V> &a, const value<V> &b) {
  return a._val > b._val;
}
template<typename V>
bool operator <=(const value<V> &a, const value<V> &b) {
  return a._val <= b._val;
}
template<typename V>
bool operator >=(const value<V> &a, const value<V> &b) {
  return a._val >= b._val;
}

template<typename V>
bool operator ==(const value<V> &a, const V &b) {
  return a._val == b;
}
template<typename V>
bool operator !=(const value<V> &a, const V &b) {
  return a._val != b;
}
template<typename V>
bool operator <(const value<V> &a, const V &b) {
  return a._val < b;
}
template<typename V>
bool operator >(const value<V> &a, const V &b) {
  return a._val > b;
}
template<typename V>
bool operator <=(const value<V> &a, const V &b) {
  return a._val <= b;
}
template<typename V>
bool operator >=(const value<V> &a, const V &b) {
  return a._val >= b;
}

//Value, derivative, time
template<typename V, typename dV = V, typename T = time_type_s>
class linear {
public:
  V _val;
  T _at;
  dV _change;

  linear() : _val(V()) {}
  linear(V val, T at) : _val(val), _at(at), _change(dV(0)) {}
  linear(V val, T at, dV change) : _val(val), _at(at), _change(change) {}
 
  V getAt(T _time) {
    return _val + to_doubleT<V, T>((_time - _at) * to_doubleT<T,dV>(_change));
  }

  template<typename X> void set(DataElement* data, X& val, std::true_type)
  {
    val.set(data);
  }
  template<typename X> void set(DataElement* data, X& val, std::false_type)
  {
   val = data->_core->toType<X>();
  }
  void set(DataElement* data)
  {
    set<V>(data->_children[0], _val, std::integral_constant<bool, HasSetMethod<V>::Has>());
    set<T>(data->_children[1], _at, std::integral_constant<bool, HasSetMethod<T>::Has>());
    set<dV>(data->_children[2], _change, std::integral_constant<bool, HasSetMethod<dV>::Has>());
  }

  template<typename X>void get(DataElement* data, X val, std::true_type)
  {
    val.get(data);
  }
  template<typename X> void get(DataElement* data, X val, std::false_type)
  {
    data->_core->fromType<X>(val);
  }
  void get(DataElement* data)
  {
    DataElement* basee = new DataElement();
    get<V>(basee, _val, std::integral_constant<bool, HasGetMethod<V>::Has>());
    data->addChild(basee);

    DataElement* timee = new DataElement();
    get<T>(timee, _at, std::integral_constant<bool, HasGetMethod<T>::Has>());
    data->addChild(timee);

    DataElement* chge = new DataElement();
    get<dV>(chge, _change, std::integral_constant<bool, HasGetMethod<dV>::Has>());
    data->addChild(chge);
  }
};

//Frametype, value, time
template<typename V, typename U = V, typename T = time_type_s>
class keyframe {
public:
  map<T, V> _frames;
  void addFrame(T _time, V _val) {
    _frames[_time] = _val;
  }
  typename std::map<T, V>::iterator search(T _time) {
    typename std::map<T, V>::iterator lb(_frames.upper_bound(_time));
    return ((lb == _frames.begin()) ? (_frames.end()) : (--lb));
  }
  T getFirst() {
    return _frames.begin()->first;
  }
  T getLast() {
    return (--_frames.end())->first;
  }
  U getAt(T _time) {
    typename std::map<T, V>::iterator it = search(_time);
    if(it != _frames.end()) {
      return it->second.getAt(_time);
    }
    else {
      if(_frames.size()) {
        LOG WARN MATH "Asked " << _time << ", first " << getFirst() END;
      } else {
        LOG WARN MATH "Asked " << _time << ", no frames" END;
      }
      _CrtDbgBreak();
      return U();
    }
  }
  double getDoubleAt(double _time) {
    return to_double(getAt(_time));
  }
  void get(DataElement* data) {
    for (auto it : _frames) {
      DataElement* paire = new DataElement();

      DataElement* timee = new DataElement();
      timee->_core->fromType<T>(it.first);
      paire->_children.push_back(timee);

      DataElement* vale = new DataElement();
      it.second.get(vale);
      paire->_children.push_back(vale);

      data->_children.push_back(paire);
    }
  }
  void set(DataElement* data) {
    _frames.clear();
    for (DataElement* it : data->_children) {
      T nt = it->_children[0]->_core->toType<T>();
      V nv;
      nv.set(it->_children[1]);
      addFrame(nt, nv);
    }
  }
  size_t size() {
    return _frames.size();
  }
};

