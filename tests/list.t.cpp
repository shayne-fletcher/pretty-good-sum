#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

#include "sum.hpp"

namespace {
  using namespace pgs;
  
  //type list = Cons of 'a * 'a list | Nil
  template <class T> struct cons_t; // Case 2
  struct nil_t {};  // Case 2

  //equaility of nil_t values
  bool operator==(nil_t const&, nil_t const&) {
    return true;
  }
  
  //Since cons_t<T> values contain list_t<values>, cons_t<T> (and so
  //list_t<T>) is a recursive type
  template <class T>
  using list = sum_type <recursive_wrapper<cons_t<T>>, nil_t>;

  //list<T> is now defined and so we may go ahead and "fill in" the
  //definition of cons_<T>
  template <class T> 
  struct cons_t {
    T hd;
    list<T> tl;
  
    template <class U, class V>
    cons_t (U&& hd, V&& tl) :
      hd (std::forward<U> (hd)), tl (std::forward<V>(tl)) {
    }

  };

 //equality of cons_t<T> values
 template <class T>
  bool operator== (cons_t<T> const& l, cons_t<T> const& r) {
    return l.hd == r.hd && l.tl == r.tl;
  }

 //non-equality of cons_t<T> values
  template <class T>
  bool operator!= (cons_t<T> const& l, cons_t<T> const& r) {
    return !(l == r);
  }

  //A factory function for a nil_t (a constant)
  template <class T>
  inline list<T> nil () {
    return list<T>{constructor<nil_t>{}};
  }

  //Factory function for a cons_t<T>
  template <class T>
  inline list<T> cons (T&& t, list<T>&& l) {
    return list<T>{
      constructor<cons_t<T>>{}, std::forward<T> (t), std::forward<list<T>>(l)};
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
  AccT fold_left (F f, AccT acc, list<T> const& l) {
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

  /*
  TODO:'range'

  let range s e =
    let rec loop acc s e =
      if s >= e then acc
      else loop (s :: acc) (s + 1) e 
    in List.rev (loop [] s e)
   */

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

  list<int> l = cons (2, cons (1, nil<int> ()));
  list<int> const& m = tl (tl (l));

  ASSERT_EQ (rev (l), cons (1, cons (2, nil<int> ())));
  ASSERT_EQ (hd (l), 2);
  ASSERT_EQ (m, nil<int>());
}
