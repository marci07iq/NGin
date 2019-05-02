#pragma once

#include "../Maths/helperFunctions.h"
#include "Compress.h"

#define PACKET_HEADER_LEN 4 //Used to store packet ID and data length;
#define PACKET_HEADER_TYPE uint32_t


class DataPair {
public:
  unsigned char* _data;
  int _len;
  DataPair(int len = 0);
  template<typename T> void fromTypeV(T val) {
    delete[] _data;
    serialize<T>(val, &_data, _len);
  }
  template<typename T> void fromType(T& val) {
    delete[] _data;
    serialize<T>(val, &_data, _len);
  }
  template<typename T> T toType() {
    return deserializeT<T>(_data, _len);
  }

  ~DataPair();
};

class DataElement {
public:
  DataPair* _core;
  vector<DataElement*> _children;

  DataElement();
  int getLen();
  bool verify();
  void fill(unsigned char* data, int& start);
  void empty(const unsigned char* data, int& start);
  void addChild(DataElement* add);
  ~DataElement();
};


template<typename T>
void vSFunc(T& val, const DataElement* elem) {
  val = elem->_core->toType<T>();
}

template<typename T>
void vGFunc(T& val, DataElement* elem) {
  elem->_core->fromType<T>(val);
}

template<typename T>
void oSFunc(T& val, const DataElement* elem) {
  val.set(elem);
}

template<typename T>
void oGFunc(T& val, DataElement* elem) {
  val.get(elem);
}

/*template<typename T>
void setVecO(vector<T>& vec, DataElement* data, void(sFunc)(void(sFunc)(T&, DataElement*))) {
  for (auto&& it : vec) {
    DataElement* ne;
    sFunc<T>(it, ne);
    data->addChild(ne);
  }
}

template<typename T>
void getVecO(vector<T>& vec, DataElement* data, void(gFunc)(T&, DataElement*)) {
  for (auto&& it : data->_childer) {
    T nval;
    nval.get(it->_core);
    vec.push_back(nval);
  }
}
*/

void saveToFile(DataElement* data, string fileName);

int loadFromFile(DataElement* data, string fileName);