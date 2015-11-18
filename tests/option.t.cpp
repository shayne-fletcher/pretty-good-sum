#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <iostream>

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
  return u.match<bool> (
   [](some_t<T> const&) -> T const& { return true; },
   [](none_t const&) -> T const& { return false; }
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
      [=](some_t<T> const& o) { return unit (f (o.data)); }
  );
}

}//namespace<anonymous>

TEST (pgs, option) {
  ASSERT_EQ (get(some (1)), 1);
  ASSERT_THROW (get (none<int>()), std::runtime_error);
  auto f = [](int i) { //avoid use of lambda in unevaluated context
    return some (i * i);   };
  ASSERT_EQ (get (some (3) * f), 9);
  option<int> x = map ([](int x) { return x * x; }, some (3));
  ASSERT_EQ (get (x), 9);
}
