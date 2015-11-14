#include <gtest/gtest.h>

#include "sum.hpp"

namespace {
  using namespace pgs;
  
  template <class T> struct cons_t;
  struct nil_t {};

  bool operator==(nil_t const&, nil_t const&) {
    return true;
  }
  
  template <class T>
  using list = sum_type <recursive_wrapper<cons_t<T>>, nil_t>;

  template <class T> 
  struct cons_t {
    T hd;
    list<T> tl;
  
    template <class U, class V>
    cons_t (U&& hd, V&& tl) :
      hd (std::forward<U> (hd)), tl (std::forward<V>(tl)) {
    }

  };

  template <class T>
  bool operator== (cons_t<T> const& l, cons_t<T> const& r) {
    return l.hd == r.hd && l.tl == r.tl;
  }

  template <class T>
  bool operator!= (cons_t<T> const& l, cons_t<T> const& r) {
    return !(l == r);
  }

  template <class T>
  inline list<T> nil () {
    return list<T>{constructor<nil_t>{}};
  }

  template <class T>
  inline list<T> cons (T&& t, list<T>&& l) {
    return list<T>{
      constructor<cons_t<T>>{}, std::forward<T> (t), std::forward<list<T>>(l)};
  }

#if defined (_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4172)
#endif//defined(_MSC_VER)

  template <class T>
  inline T const& hd (list<T> const& l) {
    return l.match <T const&> (
      [] (cons_t<T> const& n) -> T const& { return n.hd; }, 
      [] (nil_t const&) -> T const& { 
        throw std::runtime_error{"hd"}; return 0;} 
    );
  }

  template <class T>
  inline list<T> const& tl (list<T> const& l) {
    return l.match <list<T> const&> (
      [] (cons_t<T> const& n) -> list<T> const& { return n.tl; }, 
      [] (nil_t const&) -> list<T> const& { 
        throw std::runtime_error{"tl"}; return *(list<T>*)0; } 
    );
  }


#if defined (_MSC_VER)
#  pragma warning(pop)
#endif//defined(_MSC_VER)

  /*
    let rec fold_left f acc xs =
      match xs with
      | [] -> acc
      | (hd :: tl) -> fold_left f (f acc hd) tl

  template <class F, class AccT, class T>
  decltype(auto) fold_left (F f, AccT acc, list<T> const& l) {

  }
   */
  

}//namespace

TEST (pgs, list) {

  list<int> l = cons (1, nil<int> ());
  list<int> const& m = tl (l);

  ASSERT_EQ (hd (l), 1);
  ASSERT_EQ (m, nil<int>());
}
