#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace {

  using namespace BloombergLP;
  using namespace pgs;

  struct atom_unit 
  {};

  struct atom_bool { 
    bool val; 
    explicit atom_bool (bool val) : val (val) 
    {}
  };

  struct atom_int { 
    int val; 
    explicit atom_int (int val) : val (val) 
    {}
  };

  struct atom_float { 
    double val; 
    explicit atom_float (double val) : val (val) 
    {}
  };

  struct atom_string { 
    std::string val; 
    explicit atom_string (std::string const& val) : val (val) 
    {}
    explicit atom_string (std::string&& val) : val (std::move (val)) 
    {}
  };

  /*
    type atom =
    | Atom_unit
    | Atom_bool of bool
    | Atom_int of int
    | Atom_float of float
    | Atom_string of string
  */
  using atom = sum_type<atom_unit, atom_bool, atom_int, atom_float, atom_string>;

  std::string string_of_atom (atom const& a) {
    return a.match<std::string>(
      [](atom_unit const&) -> std::string { return "#u"; }
    , [](atom_bool const& b) -> std::string { return b.val ? "#t" : "#f"; }
    , [](atom_int const& i) -> std::string { return std::to_string (i.val); }
    , [](atom_float const& f) -> std::string { return std::to_string (f.val); }
    , [](atom_string const& s) -> std::string { return s.val; }
    );
  }

  struct expr_atom { 
    atom val; 
    expr_atom (atom const& val) : val (val)
    {}
    expr_atom (atom&& val) : val (std::move (val))
    {}
  };

  struct expr_list;

  /*
    type expr =
    | Expr_atom of atom
    | Expr_list of expr list
  */
  using expr =  sum_type<expr_atom, recursive_wrapper<expr_list>>;

  struct expr_list {
    std::list<expr> val;
    explicit expr_list(std::list<expr>const& l) : val (l)
    {}
    explicit expr_list(std::list<expr>&& l): val (std::move (l))
    {}
  };

  std::string string_of_expr (expr const& e) {
    return e.match<std::string>(
      [](expr_atom const& a) -> std::string { return string_of_atom (a.val); }
    , [](expr_list const& l) -> std::string {
        std::list<expr> const& exprs = l.val;
        std::list<expr>::const_iterator begin=exprs.begin (), end = exprs.end ();
        std::ostringstream os;
        os << "(";
        while (begin != end) {
          if (begin != l.val.begin ())
            os << " ";
          os << string_of_expr (*begin);
          ++begin;
        }
        os << ")";
        return os.str ();
      }
    );
  }

  struct sexpr {
  private:
    expr impl_;//implementation

  public:

    //Ctors

    //Default constructor
    sexpr () 
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_unit>{}}}
    {}
    //Construct from `bool`
    explicit sexpr (bool b)
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_bool>{}, b}}
    {}
    //Construct from `int`
    explicit sexpr (int i)
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_int>{}, i}}
    {}
    //Construct from `double`
    explicit sexpr (double d)
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_float>{}, d}}
    {}
    //Construct from `char const*`
    explicit sexpr (char const* s)
      : sexpr{std::string{s}}
    {}
    //Construct from `std::string` (r-value ref)
    explicit sexpr (std::string const& s)
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_string>{}, s}}
    {}
    //Construct from `std::string` (r-value ref)
    explicit sexpr (std::string&& s)
      : impl_{constructor<expr_atom>{}, atom{constructor<atom_string>{}, std::move (s)}}
    {}
    //Construct from a `std::list<sexpr>`
    explicit sexpr (std::list<sexpr> const& l)  
      : impl_{constructor<expr_list>{}, expr_list{std::list<expr>{}}}
    {
      std::list<expr> exprs;
      std::transform (
         l.begin (), l.end (), std::back_inserter (exprs)
       , [](sexpr const& s) -> expr const& { return s.impl_; });
      impl_ = expr{constructor<expr_list>{}, expr_list{std::move (exprs)}};
    }

    //Predicates

    //Test for unit
    bool is_unit () const {
      return impl_.match<bool>(
        [](expr_atom const& e) {
          return e.val.match<bool>(
           [](atom_unit const&) -> bool { return true; },
           [](otherwise) -> bool { return false; }
         ); },
        [](otherwise) -> bool { return false; }
      );
    }
    //Test for bool
    bool is_bool () const {
      return impl_.match<bool>(
        [](expr_atom const& e) {
          return e.val.match<bool>(
           [](atom_bool const&) -> bool { return true; },
           [](otherwise) -> bool { return false; }
         ); },
        [](otherwise) -> bool { return false; }
      );
    }
    //Test for int
    bool is_int () const {
      return impl_.match<bool>(
        [](expr_atom const& e) {
          return e.val.match<bool>(
           [](atom_int const& b) -> bool { return true; },
           [](otherwise) -> bool { return false; }
         ); },
        [](otherwise) -> bool { return false; }
      );
    }
    //Test for float
    bool is_float () const {
      return impl_.match<bool>(
        [](expr_atom const& e) {
          return e.val.match<bool>(
           [](atom_float const& b) -> bool { return true; },
           [](otherwise) -> bool { return false; }
         ); },
        [](otherwise) -> bool { return false; }
      );
    }
    //Test for string
    bool is_string () const {
      return impl_.match<bool>(
        [](expr_atom const& e) {
          return e.val.match<bool>(
           [](atom_string const& b) -> bool { return true; },
           [](otherwise) -> bool { return false; }
         ); },
        [](otherwise) -> bool { return false; }
      );
    }
    //Test for list
    bool is_list () const {
      return impl_.match<bool>(
        [](expr_list const& e) { return true; },
        [](otherwise) -> bool { return false; }
      );
    }
  
    //Accessors

    //This can't fail
    expr const& get_expr () const {
      return impl_;
    }

    //Attempt to get a `const` reference to the atom this s-expression
    //represents. Throw `std::runtime_error` if the expression is not
    //an atom
    expr_atom const& get_atom () const {
      return impl_.match<expr_atom const&>(
       [](expr_atom const& e) -> expr_atom const& { return e; },
       [](otherwise) -> expr_atom const& { throw std::runtime_error{"get_atom"}; }
     );
    }

    //Attempt to get a `const` reference to the list of expressions
    //that this s-expression represents. Throw `std::runtime_error` if
    //it's not a list
    expr_list const& get_list () const {
      return impl_.match<expr_list const&>(
       [](expr_list const& e) -> expr_list const& { return e; },
       [](otherwise) -> expr_list const& { throw std::runtime_error{"get_list"}; }
     );
    }

    //Conversions

    //Obtain a string representation
    std::string to_string () const { return string_of_expr (impl_); }
  };

}//namespace<anonymous>

TEST (pgs, sexpr) {

  using sexpr_list=std::list<sexpr>;

  sexpr s_unit;
  sexpr s_bool{true};
  sexpr s_int{1};
  sexpr s_float{1.0};
  sexpr s_string{std::string{"McCarthy"}};
  sexpr s_list{sexpr_list{s_unit, s_int, s_float, s_string}};

  ASSERT_TRUE (s_bool.is_bool ());
  ASSERT_FALSE (s_int.is_bool ());

  sexpr xpr{sexpr_list{
      sexpr{"+"}, sexpr{5}, sexpr{sexpr_list{sexpr{"+"}, sexpr{3}, sexpr{5}}}}
  };

  ASSERT_EQ (xpr.to_string (), std::string{"(+ 5 (+ 3 5))"});
}
