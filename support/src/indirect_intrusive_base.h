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
  using IndIntruHeapData = size_t;

  template<typename I, typename T, IndIntruHeapData T::*heap_info,  typename ... Cs>
  class IndIntruBase {
    // empty
  }; // class IndIntruBase
} // namespace crimson
