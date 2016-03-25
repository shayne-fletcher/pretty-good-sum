#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

namespace {
struct E_const;
struct E_add;
struct E_sub;
struct E_sub;
struct E_mul;
struct E_div;

using xpr_t = pgs::sum_type<
  E_const
  , pgs::recursive_wrapper<E_add>
  , pgs::recursive_wrapper<E_sub>
  , pgs::recursive_wrapper<E_mul>
  , pgs::recursive_wrapper<E_div>
  >;

struct E_const { 
  int i; 
  E_const (int i) : i {i} 
  {} 
};
struct E_add {
  xpr_t l, r;
  E_add (xpr_t const& l, xpr_t const& r) : l {l}, r{r} 
  {}
};
struct E_sub {
  xpr_t l, r;
  E_sub (xpr_t const& l, xpr_t const& r) : l {l}, r {r} 
  {}
};
struct E_mul {
  xpr_t l, r;
  E_mul (xpr_t const& l, xpr_t const& r) : l {l}, r {r} 
  {}
};
struct E_div {
  xpr_t l, r;
  E_div (xpr_t const& l, xpr_t const& r) : l {l}, r {r} 
  {}
};

int eval (xpr_t const& e) {

  return e.match<int> (
    [&](E_const const& e) -> int { return e.i;  },
    [&](E_mul const& e) -> int { return eval (e.l) * eval (e.r); },
    [&](E_div const& e)-> int  { return eval (e.l) / eval (e.r); },
    [&](E_add const& e) -> int { return eval (e.l) + eval (e.r); },
    [&](E_sub const& e) -> int { return eval (e.l) - eval (e.r); }
   );

}

}//namespace<anonymous>

TEST (pgs, match) {

  //n=2 + 3
  xpr_t n{
    pgs::constructor<E_add>{}
    , xpr_t {pgs::constructor<E_const>(), 2}
    , xpr_t {pgs::constructor<E_const>(), 3}
  };
  //d=5
  xpr_t d{pgs::constructor<E_const>{}, 5};
  //xpr = (2 + 3)/5
  xpr_t xpr = xpr_t{pgs::constructor<E_div>{}, n, d};

  //eval!
  ASSERT_EQ (eval (xpr), 1);
}
