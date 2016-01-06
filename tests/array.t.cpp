#include <gtest/gtest.h>

#include "sum_type.hpp"

#include <memory>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <sstream>

//A persistent array is a data structure providing the same operations
//as a usual array, namely the manipulation of elements indexed from
//`0` to `n - 1`, with low cost access and update operations, but
//where the update operation returns a new persistent array and leaves
//the previous one unchanged.

//The approach taken here due to Baker[1] follows the recitation in
//Conchon/Filliatre[2]. For the sake of this being tutorial in nature,
//we stop short of providing the version that employs "rerooting" (not
//that it's at all hard to implement).

//What's interesting about this data structure is that from the point
//of view of the client, it is purely applicative. However, the
//implementation is imperative in nature and the persistence illusory!

//[1] Henry G. Baker. Shallow binding in Lisp 1.5. Commun. ACM,
//21(7):565-569, 1978.
//
//[2]"A Persistent Union-Find Data Structure Sylvain Conchon,
//Jean-Chrisophe Filliatre
//"http://www.lri.fr/~filliatr/ftp/publis/puf-wml07.ps

namespace {

  using namespace pgs;

  //The goal here is an efficient solution to the problem of
  //persistent arrays. The basic idea is to use a usual array for
  //the last version of the persistent array and indirections for
  //pervious versions.

  template <class T> struct array_data; //fwd. decl.

  //`peristent_array<T>` is the type of persistent arrays. It is a
  //reference on a value of type `array_data<T>` which inidicates
  //its nature (being a sum of two cases)
  template <class T> using persistent_array = std::shared_ptr<array_data<T>>;

  //Case 1 : An immediate value of type `array_t<T>` containing a
  //member `data` of type `std::vector<T>`
  template <class T>
  struct array_t {
    std::vector <T> data;//array data
    array_t (int n, T const& val) : data (n, val)
    {}
    array_t (std::vector<T>&& data) : data (data)
    {}
  };

  //Case 2 : An indirection of type `diff_t<T>` value effectively a
  //product `(i, v, t)` standing for a persistent array which is
  //identical to the persistent array `t` everywhere except at index
  //`i` where it contains `v`
  template <class T>
  struct diff_t {
    int i; //index
    T v;  //value at that index
    persistent_array<T> t;  //the base array
    diff_t (int i, T const& v, persistent_array<T> const& t) 
      : i (i), v(v), t (t) 
    {}
  };

  //The definition of the sum `array_data<T>` is achieved via
  //inheritance and a forwarding construtor
  template <class T>
  struct array_data : sum_type<array_t<T>, diff_t<T>> {
    using base_type = sum_type<array_t<T>, diff_t<T>>;
    template <class U, class... Args>
      array_data (constructor<U> t, Args&&... args)
      : base_type (t, std::forward<Args>(args)...)
    {}
  };

  //`make (n, val)` returns a persistent array containing `n` copies
  //of `val`
  template <class T>
  persistent_array<T> make (int n, T const& val) {
    return persistent_array<T>{
      new array_data<T>{constructor<array_t<T>>{}, n, val}};
  }

  //To implement `get` we consider the two cases. Either the
  //persistent array is an immediate array, or we need to consider
  //the indirection and possibly recursively access another
  //persistent array
  template <class T>
  T const& get (persistent_array<T> const& t, int i) {
    return t->match<T const&>(
      [=](array_t<T> const& a) -> T const& { 
        return a.data[i]; 
      }
    , [=](diff_t<T> const& d)  -> T const& { 
        return (i == d.i) ? d.v : get (d.t, i); });
  }

  //`set` is where the subtlety is concentrated. The idea is to keep
  //the efficiency of a usual array on the very last version of the
  //persistent array, while possibly decreasing the efficiency of
  //previous versions
  template <class T>
  persistent_array<T> set (
    persistent_array<T>& t, int i, T const& v) {

    return t->match<persistent_array<T>>(
      [&t, &v, i](array_t<T>& a) -> persistent_array<T> {
        T old = a.data[i];
        a.data[i] = v;
        //`res` will be the newest version
        persistent_array<T> res{
          new array_data<T>{constructor<array_t<T>>{}, std::move (a.data)}};
        //Now, replace the contents of `t` with an indirection
        *t = array_data<T>{constructor<diff_t<T>>{}, i, old, res};

        return res;
      }

    , [t, v, i](diff_t<T>& d) -> persistent_array<T> {
        return persistent_array<T>{
          new array_data<T>{constructor<diff_t<T>>{}, i, v, t}};
      });
  }

  //`ostream` inserter for persistent arrays
  template <class T>
  std::ostream& operator << (
     std::ostream& os, persistent_array<T> const& t) {
    return t->match<std::ostream&>(
      [&](array_t<T> const& a) -> std::ostream& {
        os << "[| ";
        std::copy (a.data.begin (), a.data.end ()
                   , std::ostream_iterator<T>(os, "; "));
        os << "|]";
        return os;
      }
      , [&](diff_t<T> const& d) -> std::ostream& {
        os << "Diff (";
        os << d.i << ", " << d.v << ", " << d.t;
        os << ")";
        return os;
      });
  }

  //Return a string representation of the given persistent array
  template <class T>
  std::string string_of_persistent_array (persistent_array<T> const& t) {
    std::ostringstream os;
    os << t;
    return os.str ();
  }

}//namespace<anonymous>

TEST (pgs, array) {

  persistent_array<int> a0 = make (7, 0);
  persistent_array<int> a1 = set (a0, 1, 7);
  persistent_array<int> a2 = set (a1, 2, 8);
  persistent_array<int> a3 = set (a1, 2, 9);

  ASSERT_EQ (
    string_of_persistent_array<int> (a0)
  , std::string ("Diff (1, 0, Diff (2, 0, [| 0; 7; 8; 0; 0; 0; 0; |]))"));
  ASSERT_EQ (
    string_of_persistent_array<int> (a1)
  , std::string ("Diff (2, 0, [| 0; 7; 8; 0; 0; 0; 0; |])"));
  ASSERT_EQ (
    string_of_persistent_array<int> (a2)
  , std::string ("[| 0; 7; 8; 0; 0; 0; 0; |]"));
  ASSERT_EQ (
    string_of_persistent_array<int> (a3)
  , std::string ("Diff (2, 9, Diff (2, 0, [| 0; 7; 8; 0; 0; 0; 0; |]))"));

}
