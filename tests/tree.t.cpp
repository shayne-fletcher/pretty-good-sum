#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <string>
#include <utility>
#include <numeric>
#include <iterator>
#include <iostream>

namespace {

  using namespace pgs;

  namespace tree_detail {
    //('a, 'b) tree = 
    //  | Empty 
    //  | Node of ('a, 'b) * ('a, 'b) tree * ('a,'b) tree
    struct empty_t {}; //Case 1
    template <class T, class V> struct node_t; //Case 2

    //Since `node_t<K, V>` values contain `tree<K, V>` values,
    //`node_<K, V>` is a recursive type
    template <class K, class V>
    using tree = sum_type <empty_t, recursive_wrapper<node_t<K, V>>>;

    //A definition for `tree<K, V>` being available, now we can "fill
    //in" the definition of `node_t<K, V>`
    template <class K, class V>
    struct node_t {
      using tree_type = tree<K, V>;
      using value_type = std::pair<K, V>; 

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

    template <class K, class V>
    bool empty (tree<K, V> const& t) {
      return t.match<bool>(
       [](empty_t) { return true; },
       [](node_t<K, V> const&) { return false; }
       );
    }

    template <class K, class V, class AccT, class F>
    AccT fold (tree<K, V> const& t, AccT const& z, F f) {
      return t.match<AccT>(
       [&z](empty_t const&) -> AccT { return z; },
       [=, &z](node_t<K, V> const& n) -> AccT {
         auto const& b = n.data;
         auto const& l = n.left_child;
         auto const& r = n.right_child;
         return fold(r, f (fold (l, z, f), b), f);
       }
     );
    }

    template <class K, class V, class P>
    tree<K, V> insert (tree<K, V> const& t, P&& p) {
      return t.match<tree<K, V>> (
         [&p](empty_t) { 
         return tree<K, V>{
            constructor<node_t<K, V>>{}
          , std::forward<P>(p)
          , tree<K, V> {constructor<empty_t>{}}
          , tree<K, V> {constructor<empty_t>{}}
          };
        },
       [&p](node_t<K, V> const& m) {
          K const& k = p.first;
          V const& v = p.second;
          K const& a = m.data.first;
          tree<K, V> const& l = m.left_child;
          tree<K, V> const& r = m.right_child;
          if (k == a)
            return tree<K, V>{ 
                constructor<node_t<K, V>>{}
                , std::forward<P> (p), l, r};
          if (k < a) {
            return tree<K, V>{ 
                constructor<node_t<K, V>>{}
                , m.data, insert (l, p), r};
          }
          return tree<K, V>{ 
              constructor<node_t<K, V>>{}
              , m.data, l, insert (r, p)};
        }
      );
    }

    template <class K, class V>
    bool contains (tree<K, V> const& t, K const& k) {
      return t.match<bool>(
        [](empty_t) { return false; },
        [&k](node_t<K, V> const& n) {
          auto const& a = n.data.first;
          auto const& l = n.left_child;
          auto const& r = n.right_child;
          if (k == a) return true;
          if (k < a) return contains (l, k);
          return contains (r, k);
        }
      );
    }

    template <class K, class V>
    V const& lookup (tree<K, V> const& t, K const& k) {
      return t.match<V const&>(
        [](empty_t const&) -> V const& { 
          throw std::runtime_error{"lookup"}; 
        },
        [&k](node_t<K, V> const& n) -> V const& { 
          auto const& a = n.data.first;
          auto const& l = n.left_child;
          auto const& r = n.right_child;
          if (k == a) return n.data.second;
          if (k < a) return lookup (l, k);
          return lookup (r, k); 
        }
      );
    }

    template <class K, class V, class P>
    std::pair<tree<K, V>, tree<K, V>> partition (tree<K, V> const& t, P const& p) {
      auto f = [&p](auto const& acc, std::pair<K, V> const& b) {
        auto const& l = acc.first;
        auto const& r = acc.second;
        if (p (b))
          return std::make_pair (insert (l, b), r);
        return std::make_pair (l, insert (r, b));
      };
      tree<K, V> const empty{constructor<empty_t>{}};
      return fold (t, std::make_pair (empty, empty), f);
    }

    template <class K, class V>
    std::pair<K, V> const& min_binding (tree<K, V> const& t) {
      return t.match<std::pair<K, V> const&>(
        [](empty_t) -> std::pair<K, V> const& {  
         throw std::runtime_error{"min_binding"};
       },
       [](node_t<K, V> const& n) -> std::pair<K, V> const& {  
         auto const& l = n.left_child;
         if (empty (l)) return n.data;
         return min_binding (l);
       }
     );
    }

    template <class K, class V>
    std::pair<K, V> const& max_binding (tree<K, V> const& t) {
      return t.match<std::pair<K, V> const&>(
        [](empty_t) -> std::pair<K, V> const& {  
         throw std::runtime_error{"max_binding"};
       },
       [](node_t<K, V> const& n) -> std::pair<K, V> const& {  
         auto const& r = n.right_child;
         if (empty (r)) return n.data;
         return max_binding (r);
       }
     );
    }

    using empty_type = empty_t;
    template<class K, class V>
      using node_type =node_t<K, V>;
    template<class K, class V>
      using value_type = typename node_type<K, V>::value_type;
    template <class K, class V>
      using tree_type = typename node_type<K, V>::tree_type;

  }//namespace tree_detail

  //Dead simple binary search tree - no attempt at balancing
  template <class K, class V>
  class binary_search_tree {
  private:

    //These abbreviations will become essential in your efforts to
    //retain some sanity in what follows
    using empty_type = tree_detail::empty_t;
    using node_type = tree_detail::node_type<K, V>;
    using value_type = typename node_type::value_type;
    using tree_type = typename node_type::tree_type;
    using self_type = binary_search_tree<K, V>;

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
      return tree_detail::empty (impl_);
    }

    //Apply 'f' to each binding in the tree
    template <class AccT, class F>
    AccT fold (AccT const& z, F f) const {
      return tree_detail::fold (impl_, z, f);
    }

    //Compute a new tree with the same bindings as self but also
    //containing the binding `(k, v)`. If `k` was already bound in
    //self, that binding is replaced by `(k, v)`
    template <class P>
    self_type insert (P&& p) const {
      return self_type { tree_detail::insert (impl_, std::forward<P>(p))};
    }

    //Compute a new tree with same bindings as self but without a
    //binding `k`
    self_type remove (K const& k) const {
      return this->fold (self_type {},
        [&k](self_type const& acc, value_type const& p) {
          return k == p.first ? acc : acc.insert (p);
      });
    }

    //Compute the number of bindings in the tree
    std::size_t size () const {
      return this->fold (std::size_t{0}
          , [](std::size_t acc, auto const&) { return ++acc; });
    }

    //True if `k` is bound
    bool contains (K const& k) const {
      return tree_detail::contains (impl_, k);
    }

    //Get the value bound to `k` in the tree. Raise a
    //`std::runtime_error` if there is no binding for `k`
    V const& lookup (K const& k) const {
      return tree_detail::lookup (impl_, k);
    }

    //Extract the bindings in the tree. They'll come back sorted
    //lexicographicaly on keys
    template <class ItT>
    ItT bindings (ItT dst) const {
      return this->fold (dst, [](auto dst, auto const& n) { 
          return *dst++ = n; });
    }

    //Check if all the bindings in the tree satisfy the predicate `p`
    template <class P>
    bool for_all (P const& p) const {
      return this->fold (true, [&p](auto acc, auto const& n) { 
          return acc && p (n); });
    }

    //True if at least one binding satisfies the predicate `p`
    template <class P>
    bool exists (P const& p) const {
      return fold (false, [&p](auto acc, auto const& n) {
          return acc || p (n); });
    }

    //Compute a tree all the bindings that satisfy the predicate 'p'
    template <class P>
    self_type filter (P const& p) const {
      auto f = [&p](auto const& acc, value_type const& b) {
        if (p (b))
          return acc.insert (b);
        return acc;
      };
      return this->fold (self_type{}, f);
    }

    //Compute a pair of maps `(l, r)` where `l` contains all the
    //bindings that satisfy the predicate `p` and `r` is all the
    //bindings that dont'
    template <class P>
    std::pair<self_type, self_type> partition (P const& p) const {
      std::pair<tree_type, tree_type> res{tree_detail::partition(impl_, p)};
      return std::make_pair (
       self_type{std::move(res.first)}, self_type{std::move (res.second)});
    }

    //Computes a tree with the same keys but where `f` has been
    //applied to all values in the bindings
    template <class F>
    auto map (F f) -> 
      binary_search_tree<K, decltype (f (std::declval<V>()))> {
      auto l = [=](auto const& acc, value_type const& b) {
        return acc.insert (std::make_pair (b.first, f (b.second)));
      };
      return this->fold (self_type{}, l);
    }

    //Return the smallest binding (may throw `std::runtime_error` if
    //the tree is empty)
    value_type min_binding () const {
      return tree_detail::min_binding (impl_);
    }

    //Return the largest binding (may throw `std::runtime_error` if
    //the tree is empty)
    value_type max_binding () const {
      return tree_detail::max_binding (impl_);
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

TEST (pgs, tree2_basic) {

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

TEST (pgs, tree2_more) {

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

  ages = ages.map ([](auto age) { return ++age; });
  EXPECT_EQ(ages.lookup (std::string{"henry"}), 67);

  EXPECT_EQ (std::make_pair (std::string{"henry"}, 67), ages.min_binding ());
  EXPECT_EQ (std::make_pair (std::string{"sebastien"}, 34), ages.max_binding ());
}
