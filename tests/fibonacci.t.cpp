#include <pgs/pgs.hpp>

#include <functional>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

namespace {

  using namespace pgs;
  
  struct Nil {};
  template <class T> class Cons;
  template <class T>
    using stream = sum_type <
        Nil
      , recursive_wrapper<Cons<T>>
      >;
  template <class T>
    using stream_thunk = std::function<stream<T>()>;
  template <class T> inline 
    stream<T> force (stream_thunk<T> const& s) { 
      return s (); 
  }

  template <class T>
  class Cons {
  public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = value_type const&;
    using stream_type = stream<value_type>;

  private:
    using stream_thunk_type = stream_thunk<value_type>;

  public:
    template <class U, class V>
    Cons (U&& h, V&& t) : 
      h {std::forward<U> (h)}, t {std::forward<V> (t)}
    {}

    const_reference hd () const { return h; }
    stream_type tl () const { return force (t); }

  private:
    value_type h;
    stream_thunk_type t;
  };

  template <class T>
  T const hd (stream<T> const& s) {
    return s.template match<T const&> (
        [](Cons<T> const& l) -> T const& { return l.hd (); }
      , [](otherwise) -> T const & { throw std::runtime_error { "hd" }; }
    );
  }

  template <class T>
  stream<T> tl (stream<T> const& l) {
    return l.template match <stream<T>> (
      [] (Cons<T> const& s) -> stream <T> { return s.tl (); }
    , [] (otherwise&) -> stream<T> { throw std::runtime_error{"tl"}; }
    );
  }

  template <class T, class D>
  D take (unsigned int n, stream <T> const& s, D dst) {
    return (n == 0) ? dst :
      s.template match<D>(
         [&](Nil const& _) -> D { return  dst; },
         [&](Cons<T> const& l) -> D { 
           return take (n - 1, l.tl (), *dst++ = l.hd ()); }
      );
  }

  //Natural numbers generator

  class natural_numbers_gen {
  private:
    using int_stream = stream<int>;
    
  private:
    int start;
    std::function<int_stream(int)> from;
  
  public:
    explicit natural_numbers_gen (int start) : start (start) {
      from = [this](int x) {
        return int_stream{
          constructor <Cons<int>>{}, x, [=]() {
            return this->from (x + 1); }
        };
      };
    }

   explicit operator int_stream() const { return from (start); }
  };

  //Fibonacci numbers generator

  class fibonacci_numbers_gen {
  private:
    using int_stream = stream<int>;
    
  private:
    int start;
    std::function<int_stream(int, int)> fib;
  
  public:
    explicit fibonacci_numbers_gen () {
      fib = [&](int a, int b) {
        return int_stream{
          constructor <Cons<int>>{}, a, [a, b, this]() { 
            return this->fib (b, a + b); }
        };
      };
    }

    explicit operator int_stream() const { return fib (0, 1); }
  };
  
}//namespace<anonymous>

TEST (pgs, natural_numbers) {

  std::vector<int> s;
  take (10, stream<int> (natural_numbers_gen{0}), std::back_inserter (s));

  ASSERT_EQ (s, (std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST (pgs, fibonacci_sequence) {

  std::vector<int> s;
  take (10, stream<int> (fibonacci_numbers_gen {}), std::back_inserter(s));

  ASSERT_EQ (s, (std::vector<int>{0, 1, 1, 2, 3, 5, 8, 13, 21, 34}));
}
