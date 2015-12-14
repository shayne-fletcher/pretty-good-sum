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
    struct empty_t {}; //Case 1
    template <class T, class V> struct node_t; //Case 2

    //Since `node_t<K, V>` values contain `tree<K, V>` values,
    //`node_<K, V>` is a recursive type
    template <class K, class V>
    using tree = sum_type <empty_t, recursive_wrapper<node_t<K, V>>>;

    //These abbreviations will become essential in your efforts to
    //retain some sanity in what follows
    using empty_type = empty_t;
    using node_type = node_t<K, V>;
    using value_type = std::pair<K, V>; 
    using tree_type = tree<K, V>;
    using self_type = binary_search_tree<K, V>;

    //A definition for `tree<K, V>` being available, now we can "fill
    //in" the definition of `node_t<K, V>`
    template <class K, class V>
    struct node_t {
      //Construct a `node_t` from `data`, a `left` and a `right`
      //sub-tree
      template <class P, class U, class V>
      node_t (P&& data, U&& left_child, V&& right_child) 
        : data (std::forward<P>(data))
        , left_child (std::forward<U> (left_child))
        , right_child (std::forward<V> (right_child))
      {}
      //A node consists of:
      value_type data; //data
      tree_type left_child; //a left sub-tree and
      tree_type right_child;//a right sub-tree
    };

  private:
    tree_type impl_; //the root of the tree

    //Extension constructor for internal use
    template <class T>
    binary_search_tree (T&& n) : impl_ (std::forward<T> (n))
    {}

  public:
    //Default construction results in the empty tree
    binary_search_tree() : impl_{constructor<empty_type>{}} 
    {}

    //Test for emptiness
    bool empty () {
      return impl_.match<bool>(
       [](empty_type) { return true; },
       [](node_type const&) { return false; }
       );
    }

    //Compute a new tree with the same bindings as self but also
    //containing the binding `(k, v)`. If `k` was already bound in
    //self, that binding is replaced by `(k, v)`
    template <class P>
    binary_search_tree<K, V> insert (P&& p) const {
      return impl_.match<self_type> (
        [&p](empty_type) { 
          tree_type n{
            constructor<node_type>{}
          , std::forward<P>(p)
          , tree_type {constructor<empty_t>{}}
          , tree_type {constructor<empty_t>{}}
          };
          return self_type{n}; },
        [&p](node_type const& m) {
          K const& k = p.first;
          V const& v = p.second;
          K const& a = m.data.first;
          tree_type const& l = m.left_child;
          tree_type const& r = m.right_child;
          if (k == a)
            return self_type { tree_type{ 
                constructor<node_type>{}
                , std::forward<P> (p), l, r}};
          if (k < a) {
            return self_type { tree_type{ 
                constructor<node_type>{}
                , m.data, self_type{l}.insert (p).impl_, r} };
          }
          return self_type { tree_type{ 
              constructor<node_type>{}
              , m.data, l, self_type{r}.insert (p).impl_} };
        }
      );
    }
  };

}//namespace<anonymous>

TEST (pgs, tree) {

  binary_search_tree<int, std::string> t;

  ASSERT_TRUE(t.empty ());

  binary_search_tree<int, std::string> tt = 
    t.insert (std::make_pair (1, std::string ("foo")));

}
