#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

namespace {

using namespace pgs;

//The various cases of an expression

//`E_const` expressions store their constant value
struct E_const { 
  int i; 
  E_const (int i) : i (i) {} 
};
struct E_add;   //addition expression
struct E_sub;   //difference expression
struct E_mul;   //multiplication expression
struct E_div;   //divistion expression

//An `xpr_t` is one of a constant expression, an addition expression,
//a subtraction expression, ...
using xpr_t = sum_type<
    E_const
  , recursive_wrapper<E_add>
  , recursive_wrapper<E_sub>
  , recursive_wrapper<E_mul>
  , recursive_wrapper<E_div>
  >;

//Base class for expressions that contain a left operand and a right
//operand
struct op {
  xpr_t l, r;

  template <class U, class V>
  op (U&& l, V&& r) : l {std::forward<U> (l)}, r {std::forward<V> (r)} 
  {}
};

struct E_add : op {
  template <class U, class V>
  E_add (U&& l, V&& r) : op {std::forward<U> (l), std::forward<V> (r)}
  {}
};

struct E_sub : op{
  template <class U, class V>
  E_sub (U&& l, V&& r) : op {std::forward<U> (l), std::forward<V> (r)}
  {}
};

struct E_mul : op {
  template <class U, class V>
  E_mul (U&& l, V&& r) : op {std::forward<U> (l), std::forward<V> (r)}
  {}
};

struct E_div : op {
  template <class U, class V>
  E_div (U&& l, V&& r) : op {std::forward<U> (l), std::forward<V> (r)}
  {}
};

//Factory functions
inline xpr_t cst (int i) {
  return xpr_t{constructor<E_const>{}, i};
}
template <class L, class R>
inline xpr_t add (L&& l, R&& r) {
  return xpr_t {constructor<E_add>{}, std::forward<L>(l), std::forward<R>(r) };
}
template <class L, class R>
inline xpr_t sub (L&& l, R&& r) {
  return xpr_t{constructor<E_sub>{}, std::forward<L>(l), std::forward<R>(r) };
}
template <class L, class R>
inline xpr_t mul (L&& l, R&& r) {
  return xpr_t{constructor<E_mul>{}, std::forward<L>(l), std::forward<R>(r) };
}
template <class L, class R>
inline xpr_t div (L&& l, R&& r) {
  return xpr_t{constructor<E_div>{}, std::forward<L>(l), std::forward<R>(r) };
}

//An `ostream` "inserter" for expressions
std::ostream& operator << (std::ostream& os, xpr_t const& e) {

  return e.match<std::ostream&> (
    [&](E_const const& e) -> std::ostream& { 
      return os << e.i;  
    },
    [&](E_mul const& e) -> std::ostream& { 
      return os << e.l << "*" << e.r;  }
    ,
    [&](E_div const& e)-> std::ostream&  { 
      return os << e.l << "/" << e.r;  
    },
    [&](E_add const& e) -> std::ostream& { 
      return os << "(" << e.l << " + " << e.r << ")"; 
    },
    [&](E_sub const& e) -> std::ostream& { 
      return os << "(" << e.l << " - " << e.r << ")"; 
    }
   );
}

}//namespace<anonymous>

TEST (pgs, expression) {

  //n=2 + 3
  xpr_t n = add (cst (2), cst (3));
  //d=5
  xpr_t d = cst (5);
  //xpr = n/d = (2 + 3)/5
  xpr_t xpr = div (n, d);

  //print
  std::ostringstream os;
  os << xpr;
  ASSERT_EQ (os.str (), std::string {"(2 + 3)/5"});
}
