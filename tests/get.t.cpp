#include <gtest/gtest.h>

#include "sum.hpp"

namespace {
  using namespace pgs;
  
  template <class T> struct cons;
  template <class T> struct nil {};
  
  template <class T>
  using list = sum_type <recursive_wrapper<cons<T>>, nil<T>>;
  
  template <class T> 
  struct cons {
    T hd;
    list<T> tl;
  
    template <
      class U, class V
      , class = std::enable_if_t<
          and_<std::is_same<list<T>, std::remove_reference_t<V>>
               , std::is_same<T, std::remove_reference_t<U>>>::value>
      >
    cons (U&& hd, V&& tl) :
      hd (std::forward<U> (hd)), tl (std::forward<V>(tl)) {
    }

  };

}//namespace

TEST (pgs, get) {
  // 1 :: []
  list<int> l{
   constructor<cons<int>>{}
     , 1
     , list<int>{constructor<nil<int>>{}}
  };

  ASSERT_EQ (get<0> (l).hd, 1);

  ASSERT_THROW(get<1> (l), invalid_sum_type_access);

  ASSERT_NO_THROW(get<1>(list<int>{constructor<nil<int>>{}}));
}
