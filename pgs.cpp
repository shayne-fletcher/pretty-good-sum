//"c:/program files (x86)/Microsoft Visual Studio 14.0/vc/vcvarsall.bat" x64
//cl /Fesum.exe /Zi /MDd /EHsc /I d:/boost_1_59_0 sum.cpp

#include "sum.hpp"

#include <iostream>

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
  E_const (int i) : i (i) 
  {} 
};
struct E_add {
  xpr_t l, r;
  E_add (xpr_t const& l, xpr_t const& r) : l (l), r (r) 
  {}
};
struct E_sub {
  xpr_t l, r;
  E_sub (xpr_t const& l, xpr_t const& r) : l (l), r (r) 
  {}
};
struct E_mul {
  xpr_t l, r;
  E_mul (xpr_t const& l, xpr_t const& r) : l (l), r (r) 
  {}
};
struct E_div {
  xpr_t l, r;
  E_div (xpr_t const& l, xpr_t const& r) : l (l), r (r) 
  {}
};

std::ostream& operator << (std::ostream& os, xpr_t const& e) {

  return e.match<std::ostream&> (

    [&](E_const const& e) -> auto& { return os << e.i;  },
    [&](E_mul const& e) -> auto& { return os << e.l << "*" << e.r;  },
    [&](E_div const& e)-> auto&  { return os << e.l << "/" << e.r;  },
    [&](E_add const& e) -> auto& { return os << "(" << e.l << " + " << e.r << ")"; },
    [&](E_sub const& e) -> auto& { return os << "(" << e.l << " - " << e.r << ")"; }
  );

}

int main () {

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

  //print
  std::cout << xpr << std::endl;

  return 0;
}
