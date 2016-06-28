// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

/*
 * Copyright (C) 2016 Red Hat Inc.
 */


#include <memory>
#include <string>
#include <iostream>

#include "indirect_intrusive_vector.h"


class TestCompare1;
class TestCompare2;
class TestCompare3;

class Test1 {
    friend TestCompare1;
    friend TestCompare2;
    friend TestCompare3;

public:
    int data;
    crimson::IndIntruVectorData index_data;

    Test1(int _data) : data(_data) {}

    friend std::ostream& operator<<(std::ostream& out, const Test1& d) {
        out << d.data << " (" << d.index_data << ")";
        return out;
    }

    int& the_data() { return data; }
};

// sorted low to high
struct TestCompare1 {
    bool operator()(const Test1& d1, const Test1& d2) {
      return d1.data < d2.data;
    }
};

// sorted high to low
struct TestCompare2 {
    bool operator()(const Test1& d1, const Test1& d2) {
      return d1.data > d2.data;
    }
};


// first all even precede odd, then they're sorted high to low
struct TestCompare3 {
    bool operator()(const Test1& d1, const Test1& d2) {
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

template<typename I, typename T, crimson::IndIntruVectorData T::*index_info, typename C1, typename C2, typename C3>
void feed(crimson::IndIntruVector<I, T, index_info, C1, C2, C3> &my_vect){
    const std::shared_ptr<Test1> d99 = std::make_shared<Test1>(99);

    my_vect.push(std::make_shared<Test1>(2));
    my_vect.push(d99);
    my_vect.push(std::make_shared<Test1>(1));
    my_vect.push(std::make_shared<Test1>(-5));
    my_vect.push(std::make_shared<Test1>(12));
    my_vect.push(std::make_shared<Test1>(-12));
    my_vect.push(std::make_shared<Test1>(-7));

    std::cout << my_vect << std::endl;

}


int main(int argc, char** argv) {
    Test1 d1(2);
    Test1 d2(3);
    Test1 d3(1);
    Test1 d4(-5);

    crimson::IndIntruVector<std::shared_ptr<Test1>, Test1, &Test1::index_data, TestCompare1, TestCompare2, TestCompare3> my_vect;

    feed(my_vect);
    my_vect.display_sorted(std::cout, "reservation");

    while(!my_vect.empty()){
	my_vect.pop_reservation();
    }
//
    //  [](const Test1& t){ return t.data % 3 == 0;}
    feed(my_vect);
    my_vect.display_sorted(std::cout, "ready");

    while(!my_vect.empty()){
	my_vect.pop();
    }
    feed(my_vect);
    my_vect.display_sorted(std::cout, "limit");

    while(!my_vect.empty()){
	my_vect.pop();
    }
    feed(my_vect);
    my_vect.display_sorted(std::cout);

//    auto& t = my_vect.top();
//    t.the_data() = 17;
//    my_vect.adjust();
//    std::cout << my_vect << std::endl;



//    while (!my_vect.empty()) {
//        auto& top = my_vect.top();
//        std::cout << top << std::endl;
//        my_vect.pop();
//        std::cout << my_vect << std::endl;
//    }

    return 0;
}
