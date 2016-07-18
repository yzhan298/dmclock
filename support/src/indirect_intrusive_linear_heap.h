// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

/*
 * Copyright (C) 2016 Red Hat Inc.
 */


#pragma once

#include "indirect_intrusive_base.h"


namespace crimson {


  template<typename I, typename T, IndIntruHeapData T::*heap_info, typename CmpR, typename CmpP, typename CmpL>
  class IndIntruLinHeap : public IndIntruBase <I, T, heap_info, CmpR, CmpP, CmpL>  {

    static_assert(
      std::is_same<T,typename std::pointer_traits<I>::element_type>::value,
      "class I must resolve to class T by indirection (pointer dereference)");

    static_assert(
      std::is_same< bool,
      typename std::result_of<CmpR(const T&, const T&)>::type >::value,
      "class CmpR must define operator() to take two const T& and return a bool");

    static_assert(
      std::is_same< bool,
      typename std::result_of<CmpP(const T&, const T&)>::type >::value,
      "class CmpP must define operator() to take two const T& and return a bool");

    static_assert(
      std::is_same< bool,
      typename std::result_of<CmpL(const T&, const T&)>::type >::value,
      "class CmpL must define operator() to take two const T& and return a bool");


    class ConstIterator {
      friend IndIntruLinHeap < I, T, heap_info, CmpR, CmpP, CmpL>;

      const IndIntruLinHeap < I, T, heap_info, CmpR, CmpP, CmpL>& heap;
      size_t                                                      index;

      ConstIterator(const IndIntruLinHeap<I, T, heap_info, CmpR, CmpP, CmpL>& _heap, size_t _index) :
	heap(_heap),
	index(_index)
      {
	// empty
      }

    public:

      ConstIterator(ConstIterator&& other) :
	heap(other.heap),
	index(other.index)
      {
	// empty
      }

      ConstIterator(const ConstIterator& other) :
	heap(other.heap),
	index(other.index)
      {
	// empty
      }

      ConstIterator& operator=(ConstIterator&& other) {
	std::swap(heap, other.heap);
	std::swap(index, other.index);
	return *this;
      }

      ConstIterator& operator=(const ConstIterator& other) {
	heap = other.heap;
	index = other.index;
      }

      ConstIterator& operator++() {
	if (index <= heap.count) {
	  ++index;
	}
	return *this;
      }

      bool operator==(const ConstIterator& other) const {
	return &heap == &other.heap && index == other.index;
      }

      bool operator!=(const ConstIterator& other) const {
	return !(*this == other);
      }

      const T& operator*() {
	return *heap.data[index];
      }

      const T* operator->() {
	return &(*heap.data[index]);
      }
    }; // class ConstIterator



    CmpR                     cmp_resv;
    CmpP                     cmp_ready;
    CmpL                     cmp_limit;

  protected:
    using index_t = IndIntruHeapData;

    std::vector<I>           data;
    index_t                  count;
    T                        *resv;
    T                        *ready;
    T                        *limit;


  public:

    IndIntruLinHeap() :
      count(0),
      resv(0),
      ready(0),
      limit(0)
    {
      // empty
    }

    IndIntruLinHeap(const IndIntruLinHeap < I, T, heap_info, CmpR, CmpP, CmpL > &other) :
      count(other.count),
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

    inline T& top_resv() {
      //return *data[resv];
      return *resv;
    }

    // slow call, use specialized version
    inline const T& top_resv() const {
      //return *data[resv];
      return *resv;
    }

    inline T& top_ready() {
      //return *data[ready];
      return *ready;
    }

    // slow call, use specialized version
    inline const T& top_ready() const {
      //return *data[ready];
      return *ready;
    }

    inline T& top_limit() {
      //return *data[limit];
      return *limit;
    }

    // specialized tops
    inline const T& top_limit() const {
      //return *data[limit];
      return *limit;
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

    void pop_resv() {
      remove(resv->*heap_info);
    }

    void pop_ready() {
      remove(ready->*heap_info);
    }

    void pop_limit() {
      remove(limit->*heap_info);
    }

    void pop() {
      remove(0);
    }

    void adjust_resv() {
      resv = &(*data[0]);
      for (index_t i = 1 ; i < count; i++){
	if (cmp_resv(*data[i], *resv)){
	  resv = &(*data[i]);
	}
      }
    }

    void adjust_ready_limit() {
      ready = limit = &(*data[0]);
      T* elem;
      for (index_t i = 1 ; i < count; i++){
	elem = &(*data[i]);
	if (cmp_ready(*elem, *ready)){
	  ready = elem;
	}
	if (cmp_limit(*elem, *limit)){
	  limit = elem;
	}
      }
    }

    // use native loop to update 3 tops in one sweep
    void adjust() {
      resv = ready = limit = &(*data[0]);
      T *elem;
      for (index_t i = 1 ; i < count; i++){
	elem = &(*data[i]);
	if (cmp_resv(*elem, *resv)){
	  resv = elem;
	}

	if (cmp_ready(*elem, *ready)){
	  ready = elem;
	}

	if (cmp_limit(*elem, *limit)){
	  limit = elem;
	}
      }
    }

    void remove(const I& item) {
      index_t i = (*item).*heap_info;
      if (i < count) {
	remove(i);
      }
    }

    ConstIterator cbegin() const {
      return ConstIterator(*this, 0);
    }

    ConstIterator cend() const {
      return ConstIterator(*this, count);
    }

    friend std::ostream& operator<<(std::ostream& out, const IndIntruLinHeap& h) {
      bool first = true;
      for (auto i = h.data.cbegin(); i != h.data.cend() ; ++i){
	if(!first){
	  out << ", ";
	}
	out << **i;
	first = false;
      }
      return out;
    }


  protected:

    static index_t& index_of(I& item) {
      return (*item).*heap_info;
    }

    inline void remove(index_t i) {
      std::swap(data[i], data[--count]);
      index_of(data[i]) = i;
      data.pop_back();

      if (count > 0) {
	adjust();
      } else {
	resv = ready = limit = NULL;
      }
    }

  }; // class IntruIndirect

} // namespace crimson
