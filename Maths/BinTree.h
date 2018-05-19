#pragma once

#include "constants.h"

template<typename _VAL>
class BinTreeNode {
  BinTreeNode* _path[2];
  BinTreeNode* _up;
  _VAL _val;
public:
  void addOptions(BinTreeNode* lhs, BinTreeNode* rhs) {
    lhs->_up = this;
    rhs->_up = this;
    _path = {lhs, rhs};
  }
};

template<typename _VAL>
class BinTree : public BinTreeNode<_VAL> {
  public:
  class iterator {
    BinTreeNode<_VAL>* _ptr;
    public:
      iterator(BinTreeNode<_VAL>* ptr = NULL) {
        _ptr = ptr
      }
      _VAL operator->() {
        return _ptr->_val;
      }
      void next(bool b) {
        if(next_good()) {
          _ptr = _ptr->_path[b];
        } else {
          _ptr->addOptions(new BinTreeNode<_VAL>(), new BinTreeNode<_VAL>())
        }
      }
      bool good() {
        return _ptr != NULL;
      }
      bool next_good() {
        return _ptr->_path[0] != NULL;
      }
      BinTreeNode<_VAL>* base() {
        return _ptr;
      }
      _VAL& operator*() {
        return _ptr->_val;
      }
      void up() {
        _ptr = _ptr->_up; 
      }
  };
  iterator begin() {
    return iterator(this);
  }
  iterator lookup(uint64_t& _lookup) {
    BinTree<_VAL>::iterator it = begin();
    while (it.next_good()) {
      it.next(_lookup % 2);
      _lookup >> 1;
    }
    return it;
  }
  iterator operator[](pair<uint64_t, size_t>& to) {
    BinTree<_VAL>::iterator it = begin();
    for (size_t i = 0; i < to; i++) {
      it.next(_lookup % 2);
      _lookup >> 1;
    }
    return it;
  }
};

