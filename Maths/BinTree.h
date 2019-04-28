#pragma once

#include "helperFunctions.h"

template<typename _VAL>
class BinTreeNode {
public:
  BinTreeNode* _path[2];
  BinTreeNode* _up;
  _VAL _val;
  void addOptions(BinTreeNode* lhs, BinTreeNode* rhs) {
    lhs->_up = this;
    rhs->_up = this;
    _path[0] = lhs;
    _path[1] = rhs;
  }
};

template<typename _VAL>
class BinTree : public BinTreeNode<_VAL> {
  public:
  class iterator {
    public:
      BinTreeNode<_VAL>* _ptr;
      iterator(BinTreeNode<_VAL>* ptr = NULL) {
        _ptr = ptr;
      }
      _VAL& operator->() const {
        return _ptr->_val;
      }
      bool good() {
        return _ptr != NULL;
      }
      bool next_good() {
        return _ptr->_path[0] != NULL;
      }
      void next(bool b) {
        if(!next_good()) {
          _ptr->addOptions(new BinTreeNode<_VAL>(), new BinTreeNode<_VAL>());
        }
        _ptr = _ptr->_path[b];
      }
      BinTreeNode<_VAL>* base() {
        return _ptr;
      }
      BinTreeNode<_VAL>* operator*() {
        return _ptr;
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
      _lookup >>= 1;
    }
    return it;
  }
  template<typename I, typename S>
  iterator operator[](pair<I, S> _what) {
    BinTree<_VAL>::iterator it = begin();
    for (size_t i = 0; i < _what.second; i++) {
      it.next(_what.first % 2);
      _what.first >>= 1;
    }
    return it;
  }
  template<typename I>
  iterator operator[](I _what) {
    return lookup(_what);
  }
};