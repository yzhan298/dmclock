// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

/*
 * Copyright (C) 2016 Red Hat Inc.
 */

#include <iostream>
#include <memory>
#include <set>

#include "gtest/gtest.h"

#include "indirect_intrusive_linear_heap.h"


struct Elem {
  int data;

  crimson::IndIntruHeapData heap_data;
  crimson::IndIntruHeapData heap_data_alt;

  Elem(int _data) : data(_data) { }

  friend std::ostream& operator<<(std::ostream& out, const Elem& d) {
    out << d.data;
    return out;
  }
};


// sorted low to high
struct ElemCompareL2H {
  bool operator()(const Elem& d1, const Elem& d2) {
    return d1.data < d2.data;
  }
};

// sorted high to low
struct ElemCompareH2L {
  bool operator()(const Elem& d1, const Elem& d2) {
    return d1.data > d2.data;
  }
};

// first all evens precede all odds, then they're sorted high to low
struct ElemCompareAlt {
  bool operator()(const Elem& d1, const Elem& d2) {
    if (0 == d1.data % 2) {
      if (0 == d2.data % 2) {
	return d1.data > d2.data;
      } else {
	return true;
      }
    } else if (0 == d2.data % 2) {
      return false;
    } else {
      return d1.data > d2.data;
    }
  }
};


class HeapFixture2: public ::testing::Test {

public:

  crimson::IndIntruLinHeap<std::shared_ptr<Elem>,
			Elem,
			&Elem::heap_data,
			ElemCompareL2H, ElemCompareH2L, ElemCompareAlt> heap;

  std::shared_ptr<Elem> data1, data2, data3, data4, data5, data6, data7;

  void SetUp() {
    data1 = std::make_shared<Elem>(2);
    data2 = std::make_shared<Elem>(99);
    data3 = std::make_shared<Elem>(1);
    data4 = std::make_shared<Elem>(-5);
    data5 = std::make_shared<Elem>(12);
    data6 = std::make_shared<Elem>(-12);
    data7 = std::make_shared<Elem>(-7);

    heap.push(data1);
    heap.push(data2);
    heap.push(data3);
    heap.push(data4);
    heap.push(data5);
    heap.push(data6);
    heap.push(data7);
  }

  void TearDown() { 
    // nothing to do
  }
}; // class HeapFixture2


TEST(IndIntruLinearHeap, shared_ptr) {
  crimson::IndIntruLinHeap<std::shared_ptr<Elem>,
			Elem,
			&Elem::heap_data,
			ElemCompareL2H, ElemCompareH2L, ElemCompareAlt> heap;

  EXPECT_TRUE(heap.empty());

  heap.push(std::make_shared<Elem>(2));

  EXPECT_FALSE(heap.empty());

  heap.push(std::make_shared<Elem>(99));
  heap.push(std::make_shared<Elem>(1));
  heap.push(std::make_shared<Elem>(-5));
  heap.push(std::make_shared<Elem>(12));
  heap.push(std::make_shared<Elem>(-12));
  heap.push(std::make_shared<Elem>(-7));

  // std::cout << heap << std::endl;

  EXPECT_FALSE(heap.empty());

  EXPECT_EQ(-12, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(-7, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(-5, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(1, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(2, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(12, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(99, heap.top_resv().data);

  EXPECT_FALSE(heap.empty());
  heap.pop_resv();
  EXPECT_TRUE(heap.empty());
}


TEST(IndIntruLinearHeap, remove_data) {
  crimson::IndIntruLinHeap<std::shared_ptr<Elem>,
			Elem,
			&Elem::heap_data,
			ElemCompareL2H, ElemCompareH2L, ElemCompareAlt> heap;

  EXPECT_TRUE(heap.empty());

  std::shared_ptr<Elem> data1, data2;
  data1 = std::make_shared<Elem>(90);
  data2 = std::make_shared<Elem>(99);

  heap.push(data1);
  heap.push(data2);

  heap.push(std::make_shared<Elem>(1));
  heap.push(std::make_shared<Elem>(-5));
  heap.push(std::make_shared<Elem>(12));
  heap.push(std::make_shared<Elem>(-12));
  heap.push(std::make_shared<Elem>(-7));

  // ready is 99 == data2
  EXPECT_EQ(99, heap.top_ready().data);
  heap.remove(data2);
  EXPECT_EQ(6 , heap.size());

  EXPECT_EQ(90, heap.top_ready().data);
  heap.pop(); // remove data1 == 0
  EXPECT_EQ(5 , heap.size());

  EXPECT_EQ(-12, heap.top_resv().data);
  EXPECT_EQ(12, heap.top_ready().data);
  EXPECT_EQ(12, heap.top_limit().data);

}

TEST(IndIntruLinearHeap, regular_ptr) {
  crimson::IndIntruLinHeap<Elem*,
			  Elem,
			  &Elem::heap_data,
			  ElemCompareL2H, ElemCompareH2L, ElemCompareAlt> heap;

  EXPECT_TRUE(heap.empty());

  heap.push(new Elem(2));

  EXPECT_FALSE(heap.empty());

  heap.push(new Elem(99));
  heap.push(new Elem(1));
  heap.push(new Elem(-5));
  heap.push(new Elem(12));
  heap.push(new Elem(-12));
  heap.push(new Elem(-7));

  EXPECT_FALSE(heap.empty());

  EXPECT_EQ(-12, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(-7, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(-5, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(1, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(2, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(12, heap.top_resv().data);
  delete &heap.top_resv();
  heap.pop_resv();
  EXPECT_EQ(99, heap.top_resv().data);

  delete &heap.top_resv();

  EXPECT_FALSE(heap.empty());
  heap.pop_resv();
  EXPECT_TRUE(heap.empty());
}




TEST_F(HeapFixture2, adjust) {
  // alter a value
  data3->data = 32;  // was 1
  heap.adjust();

  // check R-tops
  EXPECT_EQ(-12, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(-7, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(-5, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(2, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(12, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(32, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_EQ(99, heap.top_resv().data);
  heap.pop_resv();
  EXPECT_TRUE(heap.empty());


    // check P-tops
  heap.push(data1);
  heap.push(data2);
  heap.push(data3);
  heap.push(data4);
  heap.push(data5);
  heap.push(data6);
  heap.push(data7);

  EXPECT_EQ(99, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(32, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(12, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(2, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(-5, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(-7, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_EQ(-12, heap.top_ready().data);
  heap.pop_ready();
  EXPECT_TRUE(heap.empty());

  // check L-tops
  heap.push(data1);
  heap.push(data2);
  heap.push(data3);
  heap.push(data4);
  heap.push(data5);
  heap.push(data6);
  heap.push(data7);

  EXPECT_EQ(32, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(12, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(2, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(-12, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(99, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(-5, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_EQ(-7, heap.top_limit().data);
  heap.pop_limit();
  EXPECT_TRUE(heap.empty());

  // now check adjust
  heap.push(data1);
  heap.push(data2);
  heap.push(data3);
  heap.push(data4);
  heap.push(data5);
  heap.push(data6);
  heap.push(data7);

  // change one data
  data1->data = 64; // was 2
  heap.adjust();
  EXPECT_EQ(-12, heap.top_resv().data);
  EXPECT_EQ(99, heap.top_ready().data);
  EXPECT_EQ(64, heap.top_limit().data);

  // check adjust_resv
  data6->data = -2; // was -12
  heap.adjust_resv();
  EXPECT_EQ(-7, heap.top_resv().data);
  EXPECT_EQ(99, heap.top_ready().data);
  EXPECT_EQ(64, heap.top_limit().data);

  // check adjust_ready_limit
  data6->data = 100;
  heap.adjust_ready_limit();
  EXPECT_EQ(-7, heap.top_resv().data);
  EXPECT_EQ(100, heap.top_ready().data);
  EXPECT_EQ(100, heap.top_limit().data);

  // revert changes
  data1->data = 2;
  data6->data = -12;
  heap.adjust();
}

//
//TEST_F(HeapFixture1, iterator_basics) {
//  {
//    uint count = 0;
//    for(auto i = heap.begin(); i != heap.end(); ++i) {
//      ++count;
//    }
//
//    EXPECT_EQ(7, count) << "count should be 7";
//  }
//
//  auto i1 = heap.begin();
//
//  EXPECT_EQ(-12, i1->data) <<
//    "first member with * operator must be smallest";
//
//  EXPECT_EQ(-12, (*i1).data) <<
//    "first member with -> operator must be smallest";
//
//  Elem& e1 = *i1;
//  EXPECT_EQ(-12, e1.data) <<
//    "first member with -> operator must be smallest";
//
//  {
//    std::set<int> values;
//    values.insert(2);
//    values.insert(99);
//    values.insert(1);
//    values.insert(-5);
//    values.insert(12);
//    values.insert(-12);
//    values.insert(-7);
//
//    for(auto i = heap.begin(); i != heap.end(); ++i) {
//      auto v = *i;
//      EXPECT_NE(values.end(), values.find(v.data)) <<
//	"value in heap must be part of original set";
//      values.erase(v.data);
//    }
//    EXPECT_EQ(0, values.size()) << "all values must have been seen";
//  }
//}
//
//
TEST_F(HeapFixture2, const_iterator_basics) {
  const auto& cheap = heap;

  {
    uint count = 0;
    for(auto i = cheap.cbegin(); i != cheap.cend(); ++i) {
      ++count;
    }

    EXPECT_EQ(7, count) << "count should be 7";
  }

  auto i1 = heap.cbegin();

  EXPECT_EQ(2, i1->data) <<
    "first member with * operator must be smallest";

  EXPECT_EQ(2, (*i1).data) <<
    "first member with -> operator must be smallest";

  const Elem& e1 = *i1;
  EXPECT_EQ(2, e1.data) <<
    "first member with -> operator must be smallest";

  {
    std::set<int> values;
    values.insert(2);
    values.insert(99);
    values.insert(1);
    values.insert(-5);
    values.insert(12);
    values.insert(-12);
    values.insert(-7);

    for(auto i = heap.cbegin(); i != heap.cend(); ++i) {
      auto v = *i;
      EXPECT_NE(values.end(), values.find(v.data)) <<
	"value in heap must be part of original set";
      values.erase(v.data);
    }
    EXPECT_EQ(0, values.size()) << "all values must have been seen";
  }
}
//
//
//TEST_F(HeapFixture1, iterator_find_rfind) {
//  {
//    auto it1 = heap.find(data7);
//    EXPECT_NE(heap.end(), it1) << "find for included element should succeed";
//    EXPECT_EQ(-7, it1->data) <<
//      "find for included element should result in right value";
//
//    auto fake_data = std::make_shared<Elem>(-7);
//    auto it2 = heap.find(fake_data);
//    EXPECT_EQ(heap.end(), it2) << "find for not included element should fail";
//  }
//
//  {
//    auto it1 = heap.rfind(data7);
//    EXPECT_NE(heap.end(), it1) <<
//      "reverse find for included element should succeed";
//    EXPECT_EQ(-7, it1->data) <<
//      "reverse find for included element should result in right value";
//
//    auto fake_data = std::make_shared<Elem>(-7);
//    auto it2 = heap.rfind(fake_data);
//    EXPECT_EQ(heap.end(), it2) <<
//      "reverse find for not included element should fail";
//  }
//}
//
//
//TEST_F(HeapFixture1, iterator_remove) {
//  auto it1 = heap.find(data7);
//  EXPECT_NE(heap.end(), it1) << "find for included element should succeed";
//
//  heap.remove(it1);
//
//  auto it2 = heap.find(data7);
//  EXPECT_EQ(heap.end(), it2) << "find for removed element should fail";
//
//  for (auto it3 = heap.begin(); it3 != heap.end(); ++it3) {
//    EXPECT_NE(-7, it3->data) <<
//      "iterating through heap should not find removed value";
//  }
//
//  // move through heap without -7
//  EXPECT_EQ(-12, heap.top().data);
//  heap.pop();
//  EXPECT_EQ(-5, heap.top().data);
//  heap.pop();
//  EXPECT_EQ(1, heap.top().data);
//  heap.pop();
//  EXPECT_EQ(2, heap.top().data);
//  heap.pop();
//  EXPECT_EQ(12, heap.top().data);
//  heap.pop();
//  EXPECT_EQ(99, heap.top().data);
//  heap.pop();
//}
