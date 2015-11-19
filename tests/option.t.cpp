#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <iostream>
#include <cstdlib>

//type 'a t = Some of 'a | None

namespace {

using namespace pgs;

template <class T>
struct some_t { //Case 1
  T data;  
  template <class U>
  explicit some_t (U&& data) : data (std::forward<U> (data))
  {}
};

struct none_t //Case 2
{};

//Options are a type that can either hold a value of type `none_t`
//(undefined) or `some_t<T>`
template<class T>
using option = sum_type<some_t<T>, none_t>;

//A trait that can "get at" the type `T` contained by an option
template <class L>
struct option_value_type;
template <class T>
struct option_value_type<option<T>> { typedef T type; };
template <class T>
using option_value_type_t = typename option_value_type<T>::type;

//Factory function for case `none_t`
template <class T>
option<T> none () {
  return option<T>{constructor<none_t>{}};
}

//Factory function for case `some_t<>`
template <class T>
option<T> some (T&& val) {
  return option<T>{constructor<some_t<T>>{}, std::forward<T> (val)};
}

//is_none : `true` if a `some_<>`, `false` otherwise
template<class T>
bool is_none (option<T> const& o) {
  return o.match<bool> (
   [](some_t<T> const&) { return false; },
   [](none_t const&) { return true; }
  );
}

//Attempt to get a `const` reference to the value contained by an
//option
template <class T>
T const& get (option<T> const & u) {
  return u.match<T const&> (
   [](some_t<T> const& o) -> T const& { return o.data; },
   [](none_t const&) -> T const& { throw std::runtime_error {"get"}; }
  );
}

//Attempt to get a non-`const` reference to the value contained by an
//option
template <class T>
T& get (option<T>& u) {
  return u.match<T&> (
   [](some_t<T>& o) -> T& { return o.data; },
   [](none_t&) -> T& { throw std::runtime_error {"get"}; }
   );
}

//Option monad 'bind'
template<class T, class F>
auto operator * (option<T> const& o, F k) -> decltype (k (get (o))) {
  using result_t = decltype (k ( get (o)));
  using t = option_value_type_t<result_t>;
  return o.match<result_t>(
      [](none_t const&) { return none<t>(); }, 
      [=](some_t<T> const& o) { return k (o.data); }
  );
}

//Option monad 'unit'
template<class T>
option<T> unit (T&& a) {
  return some (std::forward<T> (a));
}

//map
template <class T, class F>
auto map (F f, option<T> const& m) -> option<decltype (f (get (m)))>{
  using t = decltype (f ( get (m)));
  return m.match<option<t>>(
      [](none_t const&) { return none<t>(); }, 
      [=](some_t<T> const& o) { return some (f (o.data)); }
  );
}

}//namespace<anonymous>

TEST (pgs, option) {
  ASSERT_EQ (get(some (1)), 1);
  ASSERT_THROW (get (none<int>()), std::runtime_error);
  auto f = [](int i) { //avoid use of lambda in unevaluated context
    return some (i * i);   };
  ASSERT_EQ (get (some (3) * f), 9);
  auto g = [](int x) { return x * x; };
  option<int> x = map (g, some (3));
  option<int> y = map (g, none<int>());
  ASSERT_EQ (get (x), 9);
  ASSERT_TRUE (is_none (y));
}

namespace {

//safe "arithmetic"

auto add (int x) {
  return [=](int y) {
    if ((x > 0) && (y > INT_MAX - x)) {
        return none<int>(); //overflow
      }
    return some (y + x);
  };
}

auto sub (int x) {
  return [=](int y) {
    if ((x > 0) && (y < (INT_MIN +x))) {
      return none<int>(); //underflow
    }
    return some (y - x);
  };
}

auto mul (int x) {
  return [=](int y) {
    if (x != 0 && y > (INT_MAX)/x) {
      return none<int>(); //overflow
    }
    return some (y * x);
  };
}

auto div (int x) {
  return [=](int y) {
    if (x == 0) {
      return none<int>();//division by 0
    }
    return some (y / x);
  };
}

}//namespace<anonymous>

TEST(pgs, safe_arithmetic) {

  //2 * (INT_MAX/2) + 1 (won't overflow since `INT_MAX` is odd and
  //division will truncate)
  ASSERT_EQ (get (unit (INT_MAX) * div (2) * mul (2) * add (1)), INT_MAX);

  //2 * (INT_MAX/2 + 1) (overflow)
  ASSERT_TRUE (is_none (unit (INT_MAX) * div (2) * add (1) * mul (2)));
}
