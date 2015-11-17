#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

#include "sum.hpp"

namespace {
  using namespace pgs;
  
  template <class T> struct cons_t; // Case 1
  struct nil_t {};  // Case 2

  //Equaility of nil_t values
  bool operator==(nil_t const&, nil_t const&) {
    return true;
  }
  
  //Since `cons_t<T>` values contain `list_t<values>`, `cons_t<T>`
  //(and so `list_t<T>`) is a recursive type
  template <class T>
  using list = sum_type <recursive_wrapper<cons_t<T>>, nil_t>;

  //A trait that can "get at" the type `T` contained by a list type
  //`L`
  template <class L>
  struct list_value_type;
  template <class T>
  struct list_value_type<list<T>> { typedef T type; };
  template <class L>
  using list_value_type_t = typename list_value_type<L>::type;

  //A definition for `list<T>` now being available, we can "fill in"
  //the definition of `cons_<T>`
  template <class T> 
  struct cons_t {
    T hd;
    list<T> tl;
  
    template <class U, class V>
    cons_t (U&& hd, V&& tl) :
      hd (std::forward<U> (hd)), tl (std::forward<V>(tl)) {
    }
  };

 //Equality of `cons_t<T>` values
 template <class T>
  bool operator== (cons_t<T> const& l, cons_t<T> const& r) {
    return l.hd == r.hd && l.tl == r.tl;
  }

 //Non-equality of `cons_t<T>` values
  template <class T>
  bool operator!= (cons_t<T> const& l, cons_t<T> const& r) {
    return !(l == r);
  }

  //A factory function for a `nil_t` (a constant)
  template <class T>
  constexpr list<T> nil () {
    return list<T>{constructor<nil_t>{}};
  }

  //Factory function for a `cons_t<T>`
  template <class U, class V>
  inline std::remove_cv_t<std::remove_reference_t<V>> cons (U&& hd, V&& tl) {
    using t = list_value_type_t<std::remove_cv_t<std::remove_reference_t<V>>>;
    return list<t>{constructor<cons_t<t>>{}, std::forward<U>(hd), std::forward<V>(tl) };
  }

  //hd
  template <class T>
  inline T const& hd (list<T> const& l) {
    return l.match <T const&> (
      //case : nil_t
      [] (nil_t const&) -> T const& { 
        throw std::runtime_error{"hd"}; },
      //case : cons_t<T>                         
      [] (cons_t<T> const& n) -> T const& { 
        return n.hd; } 
    );
  }

  //tail
  template <class T>
  inline list<T> const& tl (list<T> const& l) {
    return l.match <list<T> const&> (
      //case : nil_t
      [] (nil_t const&) -> list<T> const& { 
        throw std::runtime_error{"tl"}; },
      //case : cons_t<T>                         
      [] (cons_t<T> const& n) -> list<T> const& { 
        return n.tl; } 
    );
  }

  //fold_left
  template <class F, class AccT, class T>
  AccT fold_left (F f, AccT const& acc, list<T> const& l) {
    return l.match<AccT>(
      //case : nil_t
      [=](nil_t){ return acc; },
      //case : cons_t<T>
      [=](cons_t<T> const& x) { 
        return fold_left (f, f(acc, x.hd), x.tl); }
      );
  }

  //fold_right
  template <class F, class T, class AccT>
  AccT fold_right (F f, list<T> const& l, AccT const& acc) {
    return l.match<AccT>(
      //case : nil_t
      [=](nil_t){ return acc; },
      //case : cons_t<T>
      [=](cons_t<T> const& x) { 
        return f (x.hd, (fold_right (f, x.tl, acc))); }
      );
  }

  //rev
  template<class T>
  inline auto rev (list<T> const& l) {
    return fold_left(
        [](auto const& acc, auto const& x) { return cons (x, acc); }
      , nil<T>()
      , l
   );
  }

  //length
  template <class T>
  inline auto length (list<T> const& l) {
    return fold_left ([](auto acc, auto const&) { return ++acc; }, 0, l);
  }

  //nth
  template <class T>
  T const& nth (list<T> const& l, std::size_t i) {
    return l.match<T const&>(
      //case : nil_t
      [](nil_t) -> T const& { 
        throw std::runtime_error {"nth"}; return *(T*)0; },
      //case : cons_t<T>
      [=](cons_t<T> const& x) -> T const& { return i == 0 ? x.hd : nth (x.tl, i - 1); }
     );
  }

  namespace detail {
    list<int> range_aux (list<int> const& acc, int const s, int e) {
      if (s >= e) return acc;
      return range_aux (cons (s, acc), s + 1, e);
    };
  }//namespace detail

  //rg - like the Python function 'range'
  list<int> rg (int begin, int end) {
    return rev (detail::range_aux (nil<int> (), begin, end));
  };

  //append (catenate two lists - not tail recursive)
  template <class T>
  list<T> append (list<T> const& l, list<T> const& r) {
    return fold_right (
      [](T const& x, list<T> const& acc) -> list<T> { return cons (x, acc); }
      , l, r);
  }

  //List monad 'unit'
  template <class T>
  inline list<T> unit (T&& a) { 
    return cons (std::forward<T>(a), nil<T>()); 
  }

  //List monad 'bind'
  /*
    let rec ( * ) : 'a t -> ('a -> 'b t) -> 'b t =
      fun l -> fun k ->
        match l with | [] -> [] | (h :: tl) -> k h @ tl * k
  */
  template <class T, class F>
  auto operator * (list<T> const& a, F k) -> decltype (k (hd (a))) {
    using result_t = decltype (k (hd (a)));
    using t = list_value_type_t<result_t>;
    return a.match<result_t>(
        [](nil_t const&) { return nil<t>(); }, 
        [=](cons_t<T> const& x) { return append (k (x.hd), x.tl * k); }
    );
  }

  //join - 'z * \m.m'
  // Concatenates a list of lists
  template <class T>
  list<T> join (list<list<T>> const& z) {
    return z * [](auto const& m) { return m; };
  }

  //map - 'map f m = m * \a.unit (f a)'
  //  The equivalent of `std::transform ()`
  template <class T, class F>
  list<T> map (F f, list<T> const& m) {
    return m * [=](auto const& a) { return unit (f (a)); };
  }

}//namespace

TEST (pgs, list) {

  //basics

  //check rev [1; 2; 3] = [3; 2; 1]
  ASSERT_EQ (rev (rg (1, 4)), cons (3, cons (2, cons (1,  nil<int> ()))));
  //check append ([1; 2; 3], [4; 5; 6; 7]) = [1; 2; 3; 4; 5; 6; 7]
  ASSERT_EQ (append (rg (1, 4), rg (4, 8)), rg (1, 8));
  //check join ([1; 2], [3; 4]) = [1; 2; 3; 4]
  ASSERT_EQ (join (cons (rg (1, 3), cons (rg (3, 5), nil<list<int>>()))) , rg (1, 5));

  //list comprehensions

  //check [1; 2]^2 = [1; 4]
  list<int> l = rg (1, 3);
  list<int> m = //avoid 'lambda in unevaluated ctx' error
     map ([](auto m) { return m * m; }, rg (1, 3));
  ASSERT_EQ (m, cons (1, cons (4, nil<int>())));
  //cartesian product
  auto n = 
     l * [&m](auto x) { 
        return m * [=](auto y) { 
           return unit (std::make_pair (x, y)); }; 
  };
  ASSERT_EQ (n, cons (std::make_pair (1, 1)
              , cons (std::make_pair (1, 4)
              , cons (std::make_pair (2, 1)
              , cons (std::make_pair (2, 4)
              , nil<std::pair<int, int>>())))));
}
