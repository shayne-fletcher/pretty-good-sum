#if !defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
#  define SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H

//! \file sum.hpp
//!
//! \copyright Copyright Björn Aili, 2013
//! \copyright Copyright Shayne Fletcher, 2015

/*
 * Copyright (c) 2013 Björn Aili
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */

//! \mainpage
//!
//! \section abstract Abstract
//!
//! A library implementing a "pretty good sum type".
//!
//! \image html prettygood.png
//!
//! \file sum.hpp
//!
//! \brief Contains the definition of the (pretty good) `sum_type`
//!
//! \details A type for modeling "sums with constructors" as used in the
//! (algebraic datatype concepts of the) functional approach to
//! programming.

//! \example list.t.cpp
//! Implementation of a functional list
//!
//! \example tree.t.cpp
//! Dead simple, purely functional binary search tree
//!
//! \example array.t.cpp
//! An implementation of persistent arrays
//!
//! \example sexpr.t.cpp
//! Rough draft of a type for S-expressions
//!
//! \example expression.t.cpp
//! Abstract syntax trees
//!
//! \example option.t.cpp
//! Options are a type that can be either 'None' (undefined) or 'Some
//! x' for any value 'x'

#  include <pgs/recursive_union.hpp>

#  include <cstddef>
#  include <iostream>

namesapce BloombergLP {

namespace pgs {

//! \brief A utility suitable for use as a wildcard in a pattern
//! match.
struct otherwise {
  template <class T>
  otherwise (T&&) {} //< Construct from anything
};

//! \cond
template <class... Ts> class sum_type;//fwd. decl
//! \endcond

//! \cond
namespace detail {

  template <std::size_t I, class T, class... Ts> 
  struct index_of_impl;

  template <std::size_t I, class T, class... Ts>
  struct index_of_impl<I, T, T, Ts...> {
    static auto const value = I;
  };
  
  template <std::size_t I, class T, class... Ts>
  struct index_of_impl<I, T, recursive_wrapper<T>, Ts...> {
    static auto const value = I;
  };

  template <std::size_t I, class X, class T, class... Ts> 
  struct index_of_impl<I, X, T, Ts...>{
    static auto const value = index_of_impl<I + 1, X, Ts...>::value;
  };

  template <std::size_t I, class T, class... Ts>
  struct type_at_impl : type_at_impl<I - 1, Ts...>
  {};

  template <class T, class... Ts>
  struct type_at_impl<0, T, Ts...> {
    using type = recursive_wrapper_unwrap_t<T>;
  };

  struct sum_type_accessor {

    template <class... Ts>
    static constexpr std::size_t active_index (sum_type<Ts...> const& s) 
      noexcept {
      return s.cons;
    }

    template <class... Ts>
    static constexpr bool compare_at (
         std::size_t i, sum_type<Ts...> const& u, sum_type<Ts...> const& v) 
      noexcept {
      return u.data.compare (i, v.data);
    }

  };

  template <std::size_t I, class... Ts>
  struct get_sum_type_element {

    static auto& get (sum_type<Ts...>& u) {
      if (u.cons != I){
        std::string message;
        message += "Indexing with ";
        message += std::to_string (I);
        message += ", but the active index is ";
        message += std::to_string (u.cons);

        throw invalid_sum_type_access{message};
      }
      return recursive_union_indexer<I, Ts...>::ref (u.data);
    }

    static auto const& get (sum_type<Ts...> const& u) {
      if (u.cons != I){
        std::string message;
        message += "Indexing with ";
        message += std::to_string (I);
        message += ", but the active index is ";
        message += std::to_string (u.cons);

        throw invalid_sum_type_access{message};
      }
      return recursive_union_indexer<I, Ts...>::ref (u.data);
    }
  };

}//namespace detail
//! \endcond

//! \brief A metafunction to compute the index `I` of a type `T` in a
//! sequence `Ts`
//!
//! \tparam T The type for which we desire its index in `Ts`
//! \tparam Ts A parameter pack containing `T`
template <class T, class... Ts>
struct index_of {
  //!\brief The index of `T` in `Ts...`
  static constexpr auto const value = 
    detail::index_of_impl<0u, T, Ts...>::value;
};

//! \brief Get the type at a given index in a variadic type list
template <std::size_t I,  class... Ts>
using type_at = typename detail::type_at_impl<I, Ts...>::type;

//! \class sum_type
//!
//! \brief A type modeling "sums with constructors" as used in
//! functional programming
template <class... Ts>
class sum_type {
private:
  std::size_t cons;
  recursive_union<Ts...> data;

private:
  friend struct detail::sum_type_accessor;
  template <std::size_t I, class... Us>
    friend struct detail::get_sum_type_element;

public:

  //! Ctor
  template <class T, class... Args>
  explicit sum_type (constructor<T> t, Args&&... args);

  sum_type () = delete;
  sum_type (sum_type const& other); //!< Copy ctor
  sum_type (sum_type&& other); //!< Move ctor

  ~sum_type(); //!< Dtor

  //! Copy-assign operator
  sum_type& operator= (sum_type const& other);

  //! Move-assign operator
  sum_type& operator= (sum_type&& other);

  //! `match` function, `const` overoad
  template <class R, class... Fs> R match(Fs&&... fs) const;
  //! `match` function, non-`const` overoad
  template <class R, class... Fs> R match(Fs&&... fs);
  //! `match` procedure, `const` overoad
  template <class... Fs> void match(Fs&&... fs) const;
  //! `match` procedure, non-`const` overoad
  template <class... Fs> void match(Fs&&... fs);

  //! The currently active `v` is at position `I`?
  template<std::size_t I>
  constexpr bool is_type_at () const noexcept;
};

//! \cond
template <class... Ts>
sum_type<Ts...>::sum_type (sum_type const& other) : cons (other.cons) {
  //std::cout << "sum_type::sum_type (sum_type const &)\n";
  data.copy (cons, other.data);
}

template <class... Ts>
sum_type<Ts...>::sum_type (sum_type&& other) : cons (other.cons) {
  //std::cout << "sum_type::sum_type (sum_type&&)\n";
  data.move (cons, std::move (other.data));
}

template <class... Ts>
  template <class T, class... Args>
  sum_type<Ts...>::sum_type (constructor<T> t, Args&&... args)
  : data (t, std::forward<Args>(args)...), cons (index_of<T, Ts...>::value){
  //std::cout << "sum_type<Ts...>::sum_type (constructor<T> t, Args&&... args)\n";
}

template <class... Ts>
sum_type<Ts...>::~sum_type() {
  data.destruct (cons);
}

template <class... Ts>
sum_type<Ts...>& sum_type<Ts...>::operator= (sum_type const& other) {
  if (std::addressof (other) == this)
    return *this;

  data.destruct (cons);
  cons = other.cons;
  data.copy (cons, other.data);

  return *this;
}

template <class... Ts>
sum_type<Ts...>& sum_type<Ts...>::operator= (sum_type&& other) {
  if (std::addressof (other) == this)
    return *this;

  data.destruct (cons);
  cons = other.cons;
  data.move (cons, std::move (other.data));

  return *this;
}

template<class... Ts>
  template <class R, class... Fs>
R sum_type<Ts...>::match(Fs&&... fs) const {
  using indicies = range_t<0, sizeof... (Ts) - 1>;
  return recursive_union_visitor<R, indicies, Ts...>::visit (
               data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class R, class... Fs>
R sum_type<Ts...>::match(Fs&&... fs) {
  using indicies = range_t<0, sizeof... (Ts) - 1>;

  return recursive_union_visitor<R, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class... Fs>
void sum_type<Ts...>::match(Fs&&... fs) const {
  using indicies = range_t<0, sizeof... (Ts) - 1>;

  recursive_union_visitor<void, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class... Fs>
void sum_type<Ts...>::match(Fs&&... fs) {
  using indicies = range_t<0, sizeof... (Ts) - 1>;
   
  recursive_union_visitor<void, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template <class... Ts>
  template <std::size_t I>
constexpr bool sum_type<Ts...>::is_type_at () const noexcept {
  return cons == I;
}
//! \endcond

//! \brief Attempt to get at the value contained in a sum

template <std::size_t I, class... Ts>
constexpr type_at<I, Ts...>& get (sum_type<Ts...>& s);

//! \brief Attempt to get at the value contained in a sum

template <std::size_t I, class... Ts>
constexpr type_at<I, Ts...> const& get (sum_type<Ts...> const& s);

//! \cond
template <std::size_t I, class... Ts>
inline constexpr type_at<I, Ts...>& get (sum_type<Ts...>& s) {
  return detail::get_sum_type_element<I, Ts...>::get (s);
}

template <std::size_t I, class... Ts>
inline constexpr type_at<I, Ts...> const& get (sum_type<Ts...> const& s) {
  return detail::get_sum_type_element<I, Ts...>::get (s);
}
//! \endcond

template <class... Ts>
bool operator == (sum_type<Ts...> const& u, sum_type<Ts...> const& v) {
  std::size_t m = detail::sum_type_accessor::active_index (u);
  std::size_t n = detail::sum_type_accessor::active_index (v);

  return m == n && detail::sum_type_accessor::compare_at (m, u, v);
}

template <class... Ts>
bool operator != (sum_type<Ts...> const& u, sum_type<Ts...> const& v) {
  return ! (u == v);
}

}//namespace pgs

}//namespace BloombergLP

#endif //!defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
