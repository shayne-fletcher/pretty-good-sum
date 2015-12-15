#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <string>
#include <utility>
#include <numeric>
#include <iterator>
#include <iostream>

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

    //Apply 'f' to each binding in the tree
    template <class AccT, class F>
    AccT fold (AccT const& z, F f) const {
      return impl_.match<AccT>(
       [&z](empty_type const&) -> AccT { return z; },
       [=, &z](node_type const& n) -> AccT {
         auto const& b = n.data;
         auto const& l = n.left_child;
         auto const& r = n.right_child;
         return self_type{r}.fold(f (self_type{l}.fold (z, f), b), f);
       }
     );
    }

    //Compute a new tree with the same bindings as self but also
    //containing the binding `(k, v)`. If `k` was already bound in
    //self, that binding is replaced by `(k, v)`
    template <class P>
    self_type insert (P&& p) const {
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

    //Compute a new tree with same bindings as self but without a
    //binding `k`
    self_type remove (K const& k) const {
      return fold (self_type {},
        [&k](self_type const& acc, value_type const& p) {
          return k == p.first ? acc : acc.insert (p);
      });
    }

    //Compute the number of bindings in the tree
    std::size_t size () const {
      return fold (std::size_t{0}
          , [](std::size_t acc, auto const&) { return ++acc; });
    }

    //True if `k` is bound
    bool contains (K const& k) const {
      return impl_.match<bool>(
        [](empty_type) { return false; },
        [&k](node_type const& n) {
          auto const& a = n.data.first;
          auto const& l = n.left_child;
          auto const& r = n.right_child;
          if (k == a) return true;
          if (k < a) return self_type{l}.contains (k);
          return self_type{r}.contains (k);
        }
      );
    }

    //Get the value bound to `k` in the tree. Raise a
    //`std::runtime_error` if there is no binding for `k`
    V const& lookup (K const& k) const {
      return impl_.match<V const&>(
        [](empty_type const&) -> V const& { throw std::runtime_error{"lookup"}; },
        [&k](node_type const& n) -> V const& { 
          auto const& a = n.data.first;
          auto const& l = n.left_child;
          auto const& r = n.right_child;
          if (k == a) return n.data.second;
          if (k < a) return self_type{l}.lookup (k);
          return self_type{r}.lookup (k); }
      );
    }

    //Extract the bindings in the tree. They'll come back sorted
    //lexicographicaly on keys
    template <class ItT>
    ItT bindings (ItT dst) const {
      return fold (dst, [](auto dst, auto const& n) { return *dst++ = n; });
    }

    //Check if all the bindings in the tree satisfy the predicate `p`
    template <class P>
    bool for_all (P const& p) const {
      return fold (true, [&p](auto acc, auto const& n) { return acc && p (n); });
    }

    //True if at least one binding satisfies the predicate `p`
    template <class P>
    bool exists (P const& p) const {
      return fold (false, [&p](auto acc, auto const& n) { return acc || p (n); });
    }

    //Compute a tree all the bindings that satisfy the predicate 'p'
    template <class P>
    self_type filter (P const& p) const {
      auto f = [&p](auto const& acc, value_type const& b) {
        if (p (b))
          return acc.insert (b);
        return acc;
      };
      return fold (self_type{}, f);
    }

    //Compute a pair of maps `(l, r)` where `l` contains all the
    //bindings that satisfy the predicate `p` and `r` is all the
    //bindings that dont'
    template <class P>
    std::pair<self_type, self_type> partition (P const& p) const {
      auto f = [&p](auto const& acc, value_type const& b) {
        auto const& l = acc.first, r = acc.second;
        if (p (b))
          return std::make_pair (self_type{l}.insert (b), self_type{r});
        return std::make_pair (self_type{l}, self_type{r}.insert (b));
      };
      return fold (std::make_pair (self_type{}, self_type{}), f);
    }

    //Computes a tree with the same keys but where `f` has been
    //applied to all values in the bindings
    template <class F>
    auto map (F f) -> 
      binary_search_tree<K, decltype (f (std::declval<V>()))> {
      auto l = [=](auto const& acc, value_type const& b) {
        return acc.insert (std::make_pair (b.first, f (b.second)));
      };
      return fold (self_type{}, l);
    }

  };

  //Factory function for creating a tree from a list
  template <class ItT>
  auto mk_tree (ItT begin, ItT end) {
    using node_t = typename std::iterator_traits<ItT>::value_type;
    using key_t = typename node_t::first_type;
    using val_t = typename node_t::second_type;
    using tree_t = binary_search_tree<key_t, val_t>;

    tree_t l;
    for (; begin != end; l = l.insert(*begin++));

    return l;
  }

}//namespace<anonymous>

TEST (pgs, tree_basic) {

  using tree_t = binary_search_tree<int, std::string>;

  tree_t t;

  ASSERT_TRUE(t.empty ());

  tree_t tt = t.insert (
   std::make_pair (1, std::string ("foo"))).
    insert (std::make_pair (2, std::string ("bar")));

  ASSERT_EQ(tt.size (), 2);

  tree_t ttt = tt.remove (2);

  ASSERT_EQ(ttt.size (), 1);
}

namespace std {//Hack!

  template <class K, class V>
  std::ostream& operator << (std::ostream& os, std::pair<K, V> const& p) {
    return os << "(" << p.first << ", " << p.second << ")";
  }

}//namspace std


TEST (pgs, tree_more) {

  using node_t = std::pair<std::string, int>;
  using tree_t = binary_search_tree<std::string, int>;

  node_t data[] = {
      std::make_pair (std::string {"mike"}, 45),
      std::make_pair (std::string {"sally"}, 25),
      std::make_pair (std::string {"sebastien"}, 33),
      std::make_pair (std::string {"jack"}, 12),
      std::make_pair (std::string {"henry"}, 66)
  };

  tree_t ages = mk_tree (data, data + 5);

  EXPECT_EQ (ages.size (), 5);
  EXPECT_TRUE (ages.contains (std::string {"sebastien"}));
  EXPECT_EQ (ages.lookup (std::string {"sally"}), 25);
  EXPECT_THROW (ages.lookup (std::string {"gru"}), std::runtime_error);

  std::cout << "Bindings : ";
  ages.bindings (
    std::ostream_iterator<std::pair<std::string, int>>(std::cout, " ")
  );
  std::cout << '\n';

  auto everyone_of_age = [](auto const& b) { return b.second > 21; };
  EXPECT_FALSE (ages.for_all (everyone_of_age));
  auto no_senior_citizen = [](auto const& b) { return b.second < 70; };
  EXPECT_TRUE (ages.for_all (no_senior_citizen));
  auto sebastien = [](auto const& b) { return b.first == "sebastien"; };
  EXPECT_TRUE (ages.exists (sebastien));
  auto gru = [](auto const& b) { return b.first == "gru"; };
  EXPECT_FALSE (ages.exists (gru));
  auto logans_run = ages.filter ([](node_t const& b) { return b.second >= 35; });
  EXPECT_EQ (logans_run.size (), 2);

  auto pp = ages.partition ([](node_t const& b) { return b.second < 30; });
  std::cout << "Those under 30: ";
  pp.first.bindings (
    std::ostream_iterator<std::pair<std::string, int>>(std::cout, " ")
  );
  std::cout << '\n';
  std::cout << "Those 30 and over: ";
  pp.second.bindings (
    std::ostream_iterator<std::pair<std::string, int>>(std::cout, " ")
  );
  std::cout << '\n';

  std::cout << "Increment everyone's age : ";
  ages = ages.map ([](auto age) { return ++age; });
  ages.bindings(
    std::ostream_iterator<std::pair<std::string, int>>(std::cout, " ")
  );
  std::cout << '\n';
  EXPECT_EQ(ages.lookup (std::string{"henry"}), 67);
}
