#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <cstdlib>
#include <climits>
#include <functional>

//type 'a t = Some of 'a | None

namespace {

using namespace BloombergLP;
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

//is_none : `true` if a `some_t<>`, `false` otherwise
template<class T>
bool is_none (option<T> const& o) {
  return o.template match<bool> (
   [](some_t<T> const&) -> bool { return false; },
   [](none_t const&) -> bool { return true; }
  );
}

//A trait that can "get at" the type `T` contained by an option
template <class>
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

//is_some : `false` if a `none_t`, `true` otherwise
template<class T>
inline bool is_some (option<T> const& o) {
  return !is_none (o);
}

//Attempt to get a `const` reference to the value contained by an
//option
template <class T>
T const& get (option<T> const & u) {
  return u.template match<T const&> (
   [](some_t<T> const& o) -> T const& { return o.data; },
   [](none_t const&) -> T const& { throw std::runtime_error {"get"}; }
  );
}

//Attempt to get a non-`const` reference to the value contained by an
//option
template <class T>
T& get (option<T>& u) {
  return u.template match<T&> (
   [](some_t<T>& o) -> T& { return o.data; },
   [](none_t&) -> T& { throw std::runtime_error {"get"}; }
   );
}

//`default x (Some v)` returns `v` and `default x None` returns `x`
template <class T>
T default_ (T x, option<T> const& u) {
  return u.template match<T> (
    [](some_t<T> const& o) -> T { return o.data; },
    [=](none_t const&) -> T { return x; }
  );
}

//`map_default f x (Some v)` returns `f v` and `map_default f x None`
//returns `x`
template<class F, class U, class T>
auto map_default (F f, U const& x, option<T> const& u) -> U {
  return u.template match <U> (
    [=](some_t<T> const& o) -> U { return f (o.data); },
    [=](none_t const&) -> U { return x; }
  );
}

//Option monad 'bind'
template<class T, class F>
auto operator * (option<T> const& o, F k) -> decltype (k (get (o))) {
  using result_t = decltype (k ( get (o)));
  using t = option_value_type_t<result_t>;
  return o.template match<result_t>  (
      [](none_t const&) -> result_t { return none<t>(); }, 
      [=](some_t<T> const& o) -> result_t { return k (o.data); }
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
  return m.template match<option<t>> (
      [](none_t const&) -> option<t> { return none<t>(); }, 
      [=](some_t<T> const& o) -> option<t> { return some (f (o.data)); }
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
  ASSERT_EQ (get (map (g, some (3))), 9);
  ASSERT_TRUE (is_none (map (g, none<int>())));

  ASSERT_EQ (default_(1, none<int>()), 1);
  ASSERT_EQ (default_(1, some(3)), 3);
  auto h = [](int y){ return float (y * y); };
  ASSERT_EQ (map_default (h, 0.0, none<int>()), 0.0);
  ASSERT_EQ (map_default (h, 0.0, some (3)), 9.0);
}

namespace {

//safe "arithmetic"

std::function<option<int>(int)> add (int x) {
  return [=](int y) -> option<int> {
    if ((x > 0) && (y > INT_MAX - x) ||
        (x < 0) && (y < INT_MIN - x)) {
        return none<int>(); //overflow
      }
    return some (y + x);
  };
}

std::function<option<int>(int)> sub (int x) {
  return [=](int y) -> option<int> {
    if ((x > 0) && (y < (INT_MIN + x)) ||
        (x < 0) && (y > (INT_MAX + x))) {
      return none<int>(); //overflow
    }
    return some (y - x);
  };
}

std::function<option<int>(int)> mul (int x) {
  return [=](int y) -> option<int> {
    if (y > 0) { //y positive
      if (x > 0) {  //x positive
        if (y > (INT_MAX / x)) {
          return none<int>(); //overflow
        }
      }
      else { //y positive, x nonpositive
        if (x < (INT_MIN / y)) {
          return none<int>(); //overflow
        }
      }
    }
    else { //y is nonpositive
      if (x > 0) { // y is nonpositive, x is positive
        if (y < (INT_MIN / x)) {
          return none<int>();
        }
      }
      else { //y, x nonpositive 
        if ((y != 0) && (x < (INT_MAX / y))) {
          return none<int>(); //overflow
        }
      }
    }

    return some (y * x);
  };
}

std::function<option<int>(int)> div (int x) {
  return [=](int y) {
    if (x == 0) {
      return none<int>();//division by 0
    }

    if (y == INT_MIN && x == -1)
      return none<int>(); //overflow

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

  //INT_MIN/(-1)
  ASSERT_TRUE (is_none (unit (INT_MIN) * div (-1)));
}

