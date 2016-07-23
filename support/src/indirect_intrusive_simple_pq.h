// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

/*
 * Copyright (C) 2016 Red Hat Inc.
 */


#pragma once

#include "indirect_intrusive_base.h"


namespace crimson {

  template<typename I, typename T, IndIntruHeapData T::*heap_info, typename C>
  class IndIntruSimplePQ : public IndIntruBase <I, T, heap_info, C>  {
    using super = IndIntruBase <I, T, heap_info, C>;

  protected:
    using index_t = IndIntruHeapData;

    C               comparator;

  public:

    IndIntruSimplePQ() :
      super()
    {
      // empty
    }

    IndIntruSimplePQ(const IndIntruSimplePQ < I, T, heap_info, C > &other) :
      super(other)
    {
      // empty
    }

    void pop() {
      remove((index_t)0);
    }

    void push(I&& item) {
      super::push(std::move(item));
      go_backward(super::count - 1);
    }

    void push(const I& item) {
      I copy(item);
      push(std::move(copy));
    }

    void promote(T& item) {
      go_backward(item.*heap_info);
    }

    void demote(T& item) {
      go_forward(item.*heap_info);
    }

    void adjust(T& item) {
      index_t i = item.*heap_info;
      if (0 == i){
	go_forward(i);
      } else if ((super::count - 1) == i){
	go_backward(i);
      } else {
	if (comparator(*super::data[i] , *super::data[i-1])){
	  go_backward(i);
	} else {
	  go_forward(i);
	}
      }
    }

    void remove(typename super::Iterator& i) {
      remove(i.cur_index());
      i = super::end();
    }

    void remove(const I& item) {
      remove((*item).*heap_info);
    }

  protected:

    void remove(index_t i) {
      while (super::count > 0
	  && i < (super::count - 1)){
	std::swap(super::data[i], super::data[i + 1]);
	super::intru_data_of(super::data[i]) = i;
	super::intru_data_of(super::data[i + 1]) = i + 1;
	++i;
      }
      super::data.pop_back();
      super::count--;
    }

    // [i+1..count] is already sorted.
    void go_forward (index_t i) {
      while ((super::count > 0 &&
	  i < (super::count - 1) &&
	  ! comparator(*super::data[i], *super::data[i+1]))) {
	std::swap(super::data[i], super::data[i+1]);
	super::intru_data_of(super::data[i]) = i;
	super::intru_data_of(super::data[i+1]) = i + 1;
	++i;;
      }
    }

    // [0..i-1] is already sorted.
    void go_backward (index_t i) {
      while ((i > 0) &&
	  comparator(*super::data[i], *super::data[i-1])) {
	std::swap(super::data[i], super::data[i-1]);
	super::intru_data_of(super::data[i]) = i;
	super::intru_data_of(super::data[i - 1]) = i - 1;
	--i;
      }
    }

  }; // class IndIntruSimplePQ

} // namespace crimson
