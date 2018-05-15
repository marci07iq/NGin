#pragma once

#include "NetworkData.h"

/*struct bits {
  vector<uint8_t> _data;
  int _len;
  bool getBit(int _bit) {
    return _data[_bit/8] & (1 << (_bit %8));
  }

  void setBit(int _bit, bool _val) {
    if (_len <= _bit) {
      _data.resize(_bit/8 + 1);
      _len = _bit;
    }
    _data[_bit / 8] &= (char)~((!_val) << (_bit % 8));
  }
};

bits bit_zero = { { 0 },1 };
bits bit_one = { { 1 },1 };

bits concat(bits& lhs, bits& rhs) {
  bits res;
  res._len = lhs._len;
  res._data = lhs._data;
  res._data.resize((lhs._len + rhs._len)/8);
  int offset = lhs._len;
  for (int i = 0; i < rhs._len; i++) {
    res.setBit(i + offset, rhs.getBit(i));
  }
  return res;
}

class HuffmanTree {
public:
  HuffmanTree* _0;
  HuffmanTree* _1;
  HuffmanTree* _up;
  char _val;
  ~HuffmanTree() {
    if (_0 != NULL) {
      delete _0;
    }
    if (_1 != NULL) {
      delete _1;
    }
  }
  list<char> me(char d, bits prefix, vector<bits>& code) {
    if (_0 == NULL) {
      code[_val] = prefix;
      return {_val};
    }
    list<char> res;
    res.splice(res.end(), _0->me(d, concat(prefix, bit_zero), code));
    res.splice(res.end(), _1->me(d, concat(prefix, bit_one), code));

  }
};

pair<HuffmanTree*, int> nextHuffman(HuffmanTree* _val) {
  int res = 0;
  while (_val->_1 == NULL) {
    if (_val->_up == NULL) {
      return {NULL,0};
    }
    _val = _val->_up;
  }
  _val = _val->_1;
  while (_val->_0 != NULL) {
    _val = _val->_0;
  }
  return {_val, res};
}

void HuffmanEncode(char* data, int len, char** out, int& outlen) {
  vector<

  priority_queue<pair<int, HuffmanTree*>> freqs;
  int freq[256];
  for (int i = 0; i < len; i++) {
    freq[data[i]]++;
  }
  for (int i = 0; i < 256; i++) {
    HuffmanTree* nt = new HuffmanTree;
    nt->_val = (char)i;
    freqs.push(make_pair(freq[i],nt));
  }
  while (freqs.size() > 1) {
    pair<int, HuffmanTree*> a = freqs.top();
    freqs.pop();
    pair<int, HuffmanTree*> b = freqs.top();
    freqs.pop();

    HuffmanTree* nt = new HuffmanTree;
    nt->_0 = a.second;
    nt->_1 = b.second;
    a.second->_up = nt;
    b.second->_up = nt;
    freqs.push(make_pair(a.first + b.first, nt));
  }
  vector<char> vals;
  
  HuffmanTree* tree = freqs.top().second;

  HuffmanTree* it = tree;


}*/

void DataCompress(DataPair* raw, DataPair** compact);

void DataExtract(DataPair* compact, DataPair** raw);
