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

  /* T is the ultimate data that's being stored in the heap, although
   *   through indirection.
   *
   * I is the indirect type that will actually be stored in the heap
   *   and that must allow dereferencing (via operator*) to yield a
   *   T&.
   *
   * C{1, 2, 3} are functors when given two T&'s will return true if the first
   *   must precede the second.
   *
   * index_info is a data member pointer as to where the heap data in T
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

      IndIntruVector<I, T, index_info, C1, C2, C3>& vect;
      size_t                            index;

      Iterator(IndIntruVector<I, T, index_info, C1, C2, C3>& _vect, size_t _index) :
	vect(_vect),
	index(_index)
      {
	// empty
      }

    public:

      Iterator(Iterator&& other) :
	vect(other.vect),
	index(other.index)
      {
	// empty
      }

      Iterator(const Iterator& other) :
	vect(other.vect),
	index(other.index)
      {
	// empty
      }

      Iterator& operator=(Iterator&& other) {
	std::swap(vect, other.vect);
	std::swap(index, other.index);
	return *this;
      }

      Iterator& operator=(const Iterator& other) {
	vect = other.vect;
	index = other.index;
      }

      Iterator& operator++() {
	if (index <= vect.count) {
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
	return *vect.data[index];
      }

      T* operator->() {
	return &(*vect.data[index]);
      }

#if 0
      // the item this iterator refers to
      void increase() {
	vect.siftUp(index);
      }
#endif
    }; // class Iterator


    class ConstIterator {
      friend IndIntruVector<I, T, index_info, C1, C2, C3>;

      const IndIntruVector<I, T, index_info, C1, C2, C3>& vect;
      size_t                                  index;

      ConstIterator(const IndIntruVector<I, T, index_info, C1, C2, C3>& _vect, size_t _index) :
	vect(_vect),
	index(_index)
      {
	// empty
      }

    public:

      ConstIterator(ConstIterator&& other) :
	vect(other.vect),
	index(other.index)
      {
	// empty
      }

      ConstIterator(const ConstIterator& other) :
	vect(other.vect),
	index(other.index)
      {
	// empty
      }

      ConstIterator& operator=(ConstIterator&& other) {
	std::swap(vect, other.vect);
	std::swap(index, other.index);
	return *this;
      }

      ConstIterator& operator=(const ConstIterator& other) {
	vect = other.vect;
	index = other.index;
      }

      ConstIterator& operator++() {
	if (index <= vect.count) {
	  ++index;
	}
	return *this;
      }

      bool operator==(const ConstIterator& other) const {
	return &vect == &other.vect && index == other.index;
      }

      bool operator!=(const ConstIterator& other) const {
	return !(*this == other);
      }

      const T& operator*() {
	return *vect.data[index];
      }

      const T* operator->() {
	return &(*vect.data[index]);
      }
    }; // class ConstIterator


  protected:
    using index_t = IndIntruVectorData;

    std::vector<I> data;
    index_t        count;
    index_t        fake_top_resrv;
    index_t        fake_top_ready;
    index_t        fake_top_limit;

    C1             comparator_resrv;
    C2             comparator_ready;
    C3             comparator_limit;


  public:
    IndIntruVector() :
      count(0),
      fake_top_resrv(0),
      fake_top_ready(0),
      fake_top_limit(0)
    {
      // empty
    }

    IndIntruVector(const IndIntruVector<I,T,index_info,C1, C2, C3>& other) :
      count(other.count),
      fake_top_resrv(other.fake_top_resrv),
      fake_top_ready(other.fake_top_ready),
      fake_top_limit(other.fake_top_limit)
    {
      for (uint i = 0; i < other.count; ++i) {
	data.push_back(other.data[i]);
      }
    }

    bool empty() const { return 0 == count; }

    size_t size() const { return count; }

    // tops
    T& top() { return *data[0];}
    const T& top() const { return *data[0];}
    I& top_ind() { return data[0]; }
    const I& top_ind() const { return data[0]; }

    T& top_reservation() { return *data[fake_top_resrv];}
    const T& top_reservation() const { return *data[fake_top_resrv];}
    I& top_ind_reservation() { return data[fake_top_resrv]; }
    const I& top_ind_reservation() const { return data[fake_top_resrv]; }

    T& top_ready() { return *data[fake_top_ready];}
    const T& top_ready() const { return *data[fake_top_ready];}
    I& top_ind_ready() { return data[fake_top_ready]; }
    const I& top_ind_ready() const { return data[fake_top_ready]; }

    T& top_limit() { return *data[fake_top_limit];}
    const T& top_limit() const { return *data[fake_top_limit];}
    I& top_ind_limit() { return data[fake_top_limit]; }
    const I& top_ind_limit() const { return data[fake_top_limit]; }


    void push(I&& item) {
      index_t i = count++;
      intru_data_of(item) = i;
      data.emplace_back(std::move(item));
      adjust();
    }

    void push(const I& item) {
      I copy(item);
      push(std::move(copy));
    }

    // pops
    void pop() { remove(0); }
    void pop_reservation() { remove(fake_top_resrv); }
    void pop_ready() { remove(fake_top_ready); }
    void pop_limit() { remove(fake_top_limit); }

    void remove(Iterator& i) {
      remove(i.index);
      i = end();
    }

    Iterator find(const I& item) {
      for (index_t i = 0; i < count; ++i) {
	if (data[i] == item) {
	  return Iterator(*this, i);
	}
      }
      return end();
    }

    // NB: should we be using operator== instead of address check?
    Iterator find(const T& item) {
      for (index_t i = 0; i < count; ++i) {
	if (data[i].get() == &item) {
	  return Iterator(*this, i);
	}
      }
      return end();
    }

    // reverse find -- start looking from bottom of heap
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

    // reverse find -- start looking from bottom of heap
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
      auto i = h.data.cbegin();
      if (i != h.data.cend()) {
	out << **i;
	++i;
	while (i != h.data.cend()) {
	  out << ", " << **i;
	  ++i;
	}
      }
      return out;
    }

    // can only be called if I is copyable
    std::ostream&
    display_sorted(std::ostream& out, std::string which = "",
		   std::function<bool(const T&)> filter = all_filter) const {
      static_assert(std::is_copy_constructible<I>::value,
		    "cannot call display_sorted when class I is not copy"
		    " constructible");

      IndIntruVector<I,T,index_info,C1, C2, C3> copy = *this;
      T *top;
      bool first = true;
      while(!copy.empty()) {
	if (which == "reservation"){
	  top = copy.top_ind_reservation().get();
	} else if (which == "ready"){
	  top = copy.top_ind_ready().get();
	} else if (which == "limit"){
	  top = copy.top_ind_limit().get();
	} else{
	  top = copy.top_ind().get();
	}

	if (filter(*top)) {
	  if (!first) {
	    out << ", ";
	  }
	  if (which == "reservation"){
	    out << copy.top_reservation();
	  } else if (which == "ready"){
	    out << copy.top_ready();
	  } else if (which == "limit"){
	    out << copy.top_limit();
	  } else{
	    out << copy.top();
	  }

	  first = false;
	}

	if (which == "reservation"){
	  copy.pop_reservation();
	} else if (which == "ready"){
	  copy.pop_ready();
	} else if (which == "limit"){
	  copy.pop_limit();
	} else{
	  copy.pop();
	}
      }
      out << std::endl;

      return out;
    }


  protected:

    static index_t& intru_data_of(I& item) {
      return (*item).*index_info;
    }

    void remove(index_t i) {
      std::swap(data[i], data[--count]);
      intru_data_of(data[i]) = i;
      data.pop_back();
      adjust();
    }

    // default value of filter parameter to display_sorted
    static bool all_filter(const T& data) { return true; }


    // use native loop over std::min_element to update 3 mins
    // in one sweep
    void adjust() {
      fake_top_resrv = fake_top_ready = fake_top_limit = 0;

      for (index_t i = 1 ; i < count; i++){
	if (comparator_resrv(*data[i], *data[fake_top_resrv])){
	  fake_top_resrv = i;
	}

	if (comparator_ready(*data[i], *data[fake_top_ready])){
	  fake_top_ready = i;
	}

	if (comparator_limit(*data[i], *data[fake_top_limit])){
	  fake_top_limit = i;
	}
      }
    }

  }; // class LinearLookUp
} // namespace crimson
