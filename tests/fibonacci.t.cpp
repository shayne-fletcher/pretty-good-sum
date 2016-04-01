#include <pgs/pgs.hpp>

#include <functional>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

namespace {

  using namespace pgs;
  
  struct Nil {};
  template <class T> struct Cons;

  template <class T>
  using stream = sum_type <Nil, recursive_wrapper<Cons<T>>>;

  template <class T>
  struct Cons {
    T h;
    std::function<stream<T>()> t;

    Cons (T h, std::function<stream<T>()> t) : h {h}, t {t}
    {}
  };

  template <class T>
  T hd (stream<T> const& s) {
    return s.template match<T> (
      [](Cons<T> const& l) -> T { return l.h; };
      [](otherwise) -> T { throw std::runtime_error "hd"; }
    );
  }

  template <class T>
  stream<T> const& tl (stream<T> const& l) {
    return l.template match <stream<T>> (
      [] (Cons<T> const& s) -> stream <T> const& { return s.tl (); },
      [] (otherwise&) -> stream<T> const& { throw std::runtime_error{"tl"}; }
    );
  }

  template <class T, class D>
  D take (unsigned int n, stream <T> const& s, D dst) {

    if (n == 0) { 
      return dst;
    }
    
    return s.match<D>(
      [&](Nil const& _) -> D { return  dst; },
      [&]( Cons<T> const& l) -> D {
        return take (n - 1, l.t (), *dst++ = l.h);
      }
    );
  }

}//namespace<anonymous>

TEST (pgs, natural_numbers) {

  std::function<stream<int>(int)> from;
  from = [&](int x) {
    return stream<int>{
      constructor <Cons<int>>{}, x, [=]() { return from (x + 1); }
    };
  };

  stream<int> natural_numbers = from (0);

  std::vector<int> s;
  take (10, natural_numbers, std::back_inserter (s));

  ASSERT_EQ (s, (std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST (pgs, fibonacci_sequence) {

  std::function<stream<int>(int, int)> fib;
  fib = [&](int a, int b) {
    return stream<int>{
      constructor <Cons<int>>{}, a, [=]() { return fib (b, a + b); }
    };
  };

  stream<int> fibonacci_sequence = fib (0, 1);

  std::vector<int> s;
  take (10, fibonacci_sequence, std::back_inserter(s));

  ASSERT_EQ (s, (std::vector<int>{0, 1, 1, 2, 3, 5, 8, 13, 21, 34}));
}
