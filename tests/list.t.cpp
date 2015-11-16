#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

#include "sum.hpp"

namespace {
  using namespace pgs;
  
  //type list = Cons of 'a * 'a list | Nil

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

  //It's helpful to have a trait that can "get at" the type `T`
  //contained by a list type `L`
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

#if defined (_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4172)
#endif//defined(_MSC_VER)

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

#if defined (_MSC_VER)
#  pragma warning(pop)
#endif//defined(_MSC_VER)

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

  //rev
  template<class T>
  list<T> rev (list<T> const& l) {
    return fold_left(
        [](auto const& acc, auto const& x) -> auto { 
          return list<T>{constructor<cons_t<T>>{}, x, acc}; }
      , nil<T>()
      , l
   );
  }

  namespace detail {
    list<int> range_aux (list<int> const& acc, int const s, int e) {
      if (s >= e)
        return acc;
      return range_aux (cons (s, acc), s + 1, e);
    };
  }//namespace detail

  list<int> rg (int begin, int end) {
    return rev (detail::range_aux (nil<int> (), begin, end));
  };

  //string_of_list
  template <class T>
  std::string string_of_list (list<T> const& l) {
    std::ostringstream os;
    l.match<std::ostream&> (

      //case cons:
      [&](cons_t<T> const& e) -> auto& { 
        return os << "cons ("<< e.hd << ", " << string_of_list (e.tl) << ")";  
      },
      //case nil:
      [&](nil_t const& e) -> auto& { return os << "nil"; }

    );

    return os.str();
  }

  //ostream inserter
  template <class T>
  std::ostream& operator<< (std::ostream& os, list<T> const& l) {
    return os << string_of_list (l) << std::endl;
  }

}//namespace

TEST (pgs, list) {

  list<int> l = rg (1, 4);
  list<int> const& m = tl (tl (l));

  ASSERT_EQ (rev (l), cons (3, cons (2, cons (1,  nil<int> ()))));
}
