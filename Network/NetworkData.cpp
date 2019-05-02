#include "NetworkData.h"

void saveToFile(DataElement * data, string fileName) {
  int len = data->getLen();
  unsigned char* res = new unsigned char[len];
  int s = 0;

  data->fill(res, s);

  ofstream out(fileName, ios::binary);
  out.write(reinterpret_cast<char*>(res), len);
  out.close();

  delete[] res;
  delete data;
}

int loadFromFile(DataElement * data, string fileName) {
  ifstream in(fileName, ios::binary);
  std::string contents((std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>());
  in.close();
  if (contents.length()) {
    int s = 0;
    data->empty(reinterpret_cast<const unsigned char*>(contents.c_str()), s);
  }
  return contents.length();
}

DataPair::DataPair(int len) {
  _len = len;
  _data = new unsigned char[_len];
}

DataPair::~DataPair() {
  delete[] _data;
}

DataElement::DataElement() {
  _core = new DataPair(0);
}

int DataElement::getLen() {
  int len = 4;
  for (DataElement* d : _children) {
    len += 1 + d->getLen();
  }
  len += 5;
  len += _core->_len;
  return len;
}

bool DataElement::verify() {
  for (auto&& it : _children) {
    if (it == NULL) {
      return false;
    }
    if (!it->verify()) {
      return false;
    }
  }
  if (_core == NULL) {
    return false;
  }
  return true;
}

void DataElement::fill(unsigned char* data, int& start) {
  PACKET_HEADER_TYPE conv;
  conv = getLen() - 4;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    data[start + i] = reinterpret_cast<unsigned char*>(&conv)[i];
  }
  start += 4;

  for (DataElement* d : _children) {
    data[start] = 0;
    start++;
    d->fill(data, start);
  }
  data[start] = 1;
  start++;
  conv = _core->_len;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    data[start + i] = reinterpret_cast<unsigned char*>(&conv)[i];
  }
  start += 4;
  for (int i = 0; i < _core->_len; i++) {
    data[start + i] = _core->_data[i];
  }
  start += _core->_len;
}

void DataElement::empty(const unsigned char* data, int& start) {
  PACKET_HEADER_TYPE conv;

  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    reinterpret_cast<unsigned char*>(&conv)[i] = data[start + i];
  }
  start += 4;

  int end = conv + start;

  while (start < end) {
    int type = data[start];
    start++;
    if (type == 0) {
      DataElement* newElem = new DataElement();
      newElem->empty(data, start);
      _children.push_back(newElem);
    }
    if (type == 1) {
      for (int i = 0; i < PACKET_HEADER_LEN; i++) {
        reinterpret_cast<unsigned char*>(&conv)[i] = data[start + i];
      }
      start += 4;

      int nlen = conv;

      delete _core;

      _core = new DataPair(nlen);

      for (int i = 0; i < _core->_len; i++) {
        _core->_data[i] = data[start + i];
      }
      start += _core->_len;
    }
  }
}

void DataElement::addChild(DataElement* add) {
  _children.push_back(add);
}

DataElement::~DataElement() {
  for (DataElement* d : _children) {
    delete d;
  }
  _children.clear();
  delete _core;
}
