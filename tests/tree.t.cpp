#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <string>
#include <utility>

namespace {

  using namespace pgs;

  //Dead simple binary search tree - no attempt at balancing
  template <class K, class V>
  class binary_search_tree {
  private:
    //('a, 'b) tree = 
    //  | Empty 
    //  | Node of ('a, 'b) * ('a, 'b) tree * ('a,'b) tree
    struct empty {}; //Case 1
    template <class T, class V> struct node; //Case 2

    //Since `node_<K, V>` values contain `tree<K, V>` values, `node<K,
    //V>` is a recursive type
    template <class K, class V>
    using tree = sum_type <empty, recursive_wrapper<node<K, V>>>;

    //A definition for `tree<K, V>` being available, now we can "fill
    //in" the definition of `node<K, V>`
    template <class K, class V>
    struct node {
      std::pair <K, V> data;
      tree<K, V> left_child;
      tree<K, V> right_child;
    };

  private:
    tree<K, V> impl_; //the root of the tree

  public:
    binary_search_tree() : impl_{constructor<empty>{}} 
    {} //default construction results in the empty tree

    //empty
    bool empty () {
      return impl_.match<bool>(
       [](empty) -> bool { return true },
       [](node<K, V> const&) -> { return false};
       );
    }
  };

}//namespace<anonymous>

TEST (pgs, tree) {

  ASSERT_TRUE(t.empty (), binary_search_tree<int, std::string>{});

}
