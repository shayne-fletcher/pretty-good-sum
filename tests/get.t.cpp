#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

namespace {
  using namespace pgs;
  
  template <class T> struct cons_t;
  template <class T> struct nil_t {};
  
  template <class T>
  using list = sum_type <recursive_wrapper<cons_t<T>>, nil_t<T>>;
  
  template <class T> 
  struct cons_t {
    T hd;
    list<T> tl;
  
    template <class U, class V>
    cons_t (U&& hd, V&& tl) :
      hd {std::forward<U> (hd)}, tl {std::forward<V>(tl)} {
    }

  };

  template <class T>
  inline list<T> nil () {
    return list<T>{constructor<nil_t<T>>{}};
  }

  template <class U, class V>
  inline list<decay_t<U>> cons (U&& hd, V&& tl) {
    using T = decay_t<U>;
    return list<T> {constructor<cons_t<T>>{}, std::forward<U> (hd), std::forward<V> (tl) };
  }

}//namespace

TEST (pgs, get) {

  list<int> l = cons (1, nil<int> ());

  ASSERT_EQ (get<0> (l).hd, 1);

  ASSERT_THROW(get<1> (l), invalid_sum_type_access);

  ASSERT_NO_THROW(get<1>(nil<int> ()));
}
