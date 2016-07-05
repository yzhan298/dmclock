// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

/*
 * Copyright (C) 2016 Red Hat Inc.
 */


#pragma once


#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <functional>

#include "assert.h"


namespace crimson {
  using IndIntruVectorData = size_t;

  /* T is the ultimate data that's being stored in the vector, although
   *   through indirection.
   *
   * I is the indirect type that will actually be stored in the vector
   *   and that must allow dereferencing (via operator*) to yield a
   *   T&.
   *
   * C is a functor when given two T&'s will return true if the first
   *   must precede the second.
   *
   * index_info is a data member pointer as to where the vector data in T
   * is stored.
   */
  template<typename I, typename T, IndIntruVectorData T::*index_info, typename C1, typename C2, typename C3>
  class IndIntruVector {

    static_assert(
      std::is_same<T,typename std::pointer_traits<I>::element_type>::value,
      "class I must resolve to class T by indirection (pointer dereference)");

    static_assert(
      std::is_same<bool,
      typename std::result_of<C1(const T&,const T&)>::type>::value,
      "class C1 must define operator() to take two const T& and return a bool");

    static_assert(
      std::is_same<bool,
      typename std::result_of<C2(const T&,const T&)>::type>::value,
      "class C2 must define operator() to take two const T& and return a bool");

    static_assert(
      std::is_same<bool,
      typename std::result_of<C3(const T&,const T&)>::type>::value,
      "class C3 must define operator() to take two const T& and return a bool");


    class Iterator {
      friend IndIntruVector<I, T, index_info, C1, C2, C3>;

      IndIntruVector<I, T, index_info, C1, C2, C3>& vec;
      size_t                                        index;

      Iterator(IndIntruVector<I, T, index_info, C1, C2, C3 >& _vec, size_t _index) :
	vec(_vec),
	index(_index)
      {
	// empty
      }

    public:

      Iterator(Iterator&& other) :
	vec(other.vec),
	index(other.index)
      {
	// empty
      }

      Iterator(const Iterator& other) :
	vec(other.vec),
	index(other.index)
      {
	// empty
      }

      Iterator& operator=(Iterator&& other) {
	std::swap(vec, other.vec);
	std::swap(index, other.index);
	return *this;
      }

      Iterator& operator=(const Iterator& other) {
	vec = other.vec;
	index = other.index;
      }

      Iterator& operator++() {
	if (index <= vec.count) {
	  ++index;
	}
	return *this;
      }

      bool operator==(const Iterator& other) const {
	return index == other.index;
      }

      bool operator!=(const Iterator& other) const {
	return !(*this == other);
      }

      T& operator*() {
	return *vec.data[index];
      }

      T* operator->() {
	return &(*vec.data[index]);
      }
    }; // class Iterator

    class ConstIterator {
      friend IndIntruVector<I, T, index_info, C1, C2, C3>;

      const IndIntruVector<I, T, index_info, C1, C2, C3>& vec;
      size_t                                              index;

      ConstIterator(const IndIntruVector<I, T, index_info, C1, C2, C3>& _vec, size_t _index) :
	vec(_vec),
	index(_index)
      {
	// empty
      }

    public:

      ConstIterator(ConstIterator&& other) :
	vec(other.vec),
	index(other.index)
      {
	// empty
      }

      ConstIterator(const ConstIterator& other) :
	vec(other.vec),
	index(other.index)
      {
	// empty
      }

      ConstIterator& operator=(ConstIterator&& other) {
	std::swap(vec, other.vec);
	std::swap(index, other.index);
	return *this;
      }

      ConstIterator& operator=(const ConstIterator& other) {
	vec = other.vec;
	index = other.index;
      }

      ConstIterator& operator++() {
	if (index <= vec.count) {
	  ++index;
	}
	return *this;
      }

      bool operator==(const ConstIterator& other) const {
	return &vec == &other.vec && index == other.index;
      }

      bool operator!=(const ConstIterator& other) const {
	return !(*this == other);
      }

      const T& operator*() {
	return *vec.data[index];
      }

      const T* operator->() {
	return &(*vec.data[index]);
      }
    }; // class ConstIterator

    
  protected:
    using index_t = IndIntruVectorData;
    std::vector<I>           data;
    index_t                  count;
    const index_t            top_default;

    index_t                  resv;
    index_t                  ready;
    index_t                  limit;

    C1                       comp_resv;
    C2                       comp_limit;
    C3                       comp_ready;

  public:

    IndIntruVector() :
      count(0),
      top_default(0),
      resv(0),
      ready(0),
      limit(0)
    {
      // empty
    }

    IndIntruVector(const IndIntruVector<I,T,index_info,C1, C2, C3>& other) :
      count(other.count),
      top_default(0),
      resv(other.resv),
      ready(other.ready),
      limit(other.limit)
    {
      for (uint i = 0; i < other.count; ++i) {
	data.push_back(other.data[i]);
      }
    }

    bool empty() const { return 0 == count; }

    size_t size() const { return count; }

    // slow call, use specialized version
    T& top(index_t IndIntruVector::*which_top) {
      return *data[this->*which_top];
    }

    // slow call, use specialized version
    const T& top(index_t IndIntruVector::*which_top) const {
      return *data[this->*which_top];
    }

    T& top_resv() {
      return *data[resv];
    }

    // slow call, use specialized version
    const T& top_resv() const {
      return *data[resv];
    }

    T& top_ready() {
      return *data[ready];
    }

    // slow call, use specialized version
    const T& top_ready() const {
      return *data[ready];
    }

    T& top_limit() {
      return *data[limit];
    }

    // specialized tops
    const T& top_limit() const {
      return *data[limit];
    }

    void push(I&& item) {
      index_t i = count++;
      index_of(item) = i;
      data.emplace_back(std::move(item));
      adjust();
    }

    void push(const I& item) {
      I copy(item);
      push(std::move(copy));
    }

    void pop(index_t IndIntruVector::*where) {
      remove(this->*where);
    }

    void pop_resv() {
      remove(resv);
    }

    void pop_ready() {
      remove(ready);
    }

    void pop_limit() {
      remove(limit);
    }

    void pop() {
      remove(top_default);
    }

    void remove(const I& item) {
      index_t i = (*item).*index_info;
      if (i < count) {
	remove(i);
      }
    }


    Iterator find(const I& item) {
      for (index_t i = 0; i < count; ++i) {
	if (data[i] == item) {
	  return Iterator(*this, i);
	}
      }
      return end();
    }


    Iterator find(const T& item) {
      for (index_t i = 0; i < count; ++i) {
	if (data[i].get() == &item) {
	  return Iterator(*this, i);
	}
      }
      return end();
    }

    // reverse find -- start looking from bottom of vec
    Iterator rfind(const I& item) {
      // index_t is unsigned, so we can't allow to go negative; so
      // we'll keep it one more than actual index
      for (index_t i = count; i > 0; --i) {
	if (data[i-1] == item) {
	  return Iterator(*this, i-1);
	}
      }
      return end();
    }

    // reverse find -- start looking from bottom of vec
    Iterator rfind(const T& item) {
      // index_t is unsigned, so we can't allow to go negative; so
      // we'll keep it one more than actual index
      for (index_t i = count; i > 0; --i) {
	if (data[i-1].get() == &item) {
	  return Iterator(*this, i-1);
	}
      }
      return end();
    }


    Iterator begin() {
      return Iterator(*this, 0);
    }

    Iterator end() {
      return Iterator(*this, count);
    }

    ConstIterator cbegin() const {
      return ConstIterator(*this, 0);
    }

    ConstIterator cend() const {
      return ConstIterator(*this, count);
    }

    friend std::ostream& operator<<(std::ostream& out, const IndIntruVector& h) {
      bool first = true;
      for (index_t i = 0 ; i < h.count ; i++){
	if(!first){
	  out << ", ";
	}
	out << h.data[i] << " (" << i << ") ";
	first = false;
      }
      return out;
    }


    // use native loop to update 3 tops in one sweep
    void adjust() {
      resv = ready = limit = 0;

      for (index_t i = 1 ; i < count; i++){
	if (comp_resv(*data[i], *data[resv])){
	  resv = i;
	}

	if (comp_limit(*data[i], *data[limit])){
	  limit = i;
	}

	if (comp_ready(*data[i], *data[ready])){
	  ready = i;
	}
      }
    }


  protected:

    static index_t& index_of(I& item) {
      return (*item).*index_info;
    }


    void remove(index_t i) {
      std::swap(data[i], data[--count]);
      index_of(data[i]) = i;
      data.pop_back();
      adjust();
    }

  }; // class IndIntruVector
} // namespace crimson
