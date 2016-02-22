#include <pgs/pgs.hpp>

#include <gtest/gtest.h>

namespace {
  using namespace BloombergLP;
  using namespace pgs;
  
  template <class T> struct cons;
  template <class T> struct nil {};
  
  template <class T>
  using list = sum_type <recursive_wrapper<cons<T>>, nil<T>>;
  
  template <class T> 
  struct cons {
    T hd;
    list<T> tl;
  
    template <class U, class V>
    cons (U&& hd, V&& tl) :
      hd (std::forward<U> (hd)), tl (std::forward<V>(tl)) {
    }
  };
  
  bool foo_copy_detected=false;
  bool foo_move_detected=false;
  
  struct foo {
    explicit foo (int i){ }
    foo (foo const& bar) { foo_copy_detected=true; }
    foo (foo&& bar) { foo_move_detected=true; }
    foo& operator = (foo const& f) =delete;
    foo& operator = (foo&& f) = delete;
  };
  
  list<foo> singleton () {
    return list<foo>{
      constructor<cons<foo>>{}
      , foo{1}
      , list<foo>{constructor<nil<foo>>{}}
    };
  }
  
}//namespace<anonymous>

TEST (pgs, move) {
  list<foo> l = singleton ();
  ASSERT_TRUE(foo_move_detected && !foo_copy_detected);
}

TEST (pgs, move_assign) {
  list<foo> l = singleton ();
  list<foo> m{constructor<nil<foo>>{}};

  ASSERT_TRUE(m.is_type_at<1>());

  m = std::move (l); //step into the debugger to observe the right overload is chosen

  ASSERT_TRUE(m.is_type_at<0>());

  ASSERT_TRUE(true);
}
