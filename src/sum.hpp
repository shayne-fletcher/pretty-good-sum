#if !defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
#  define SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H
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

//! \file sum_type.hpp
//! \copyright Copyright (c) 2013 Bj�rn Aili
//! \copyright Copyright Shayne Fletcher, 2015
/*
 * Copyright (c) 2013 Bj�rn Aili
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
#  include "recursive_union.hpp"

#  include <cstddef>
#  include <iostream>

namespace pgs {

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

}//namespace detail
//! \endcond

//! \brief A metafunction to compute the index `I` of a type `T` in a
//! sequence `Ts`
//!
//! \tparam T The type for which we desire its index in `Ts`
//! \tparam Ts A parameter pack containing `T`

template <class T, class... Ts>
struct index_of {
  static auto const value = detail::index_of_impl<0u, T, Ts...>::value;
};

//! \class sum_type
//!
//! \brief A type modeling "sums with constructors" as used in
//! functional programming

template <class... Ts>
class sum_type {
private:
  std::size_t cons;
  recursive_union<Ts...> data;
  
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

  //! The currently active type is `i`?
  template<std::size_t I>
  constexpr bool is_type_at () const;

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
  return union_visitor<R, indicies, Ts...>::visit (
               data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class R, class... Fs>
R sum_type<Ts...>::match(Fs&&... fs) {
  using indicies = range_t<0, sizeof... (Ts) - 1>;

  return union_visitor<R, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class... Fs>
void sum_type<Ts...>::match(Fs&&... fs) const {
  using indicies = range_t<0, sizeof... (Ts) - 1>;

  union_visitor<void, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template<class... Ts>
  template <class... Fs>
void sum_type<Ts...>::match(Fs&&... fs) {
  using indicies = range_t<0, sizeof... (Ts) - 1>;
   
  union_visitor<void, indicies, Ts...>::visit (
                data, cons, std::forward<Fs>(fs)...);
}

template <class... Ts>
  template <std::size_t I>
constexpr bool sum_type<Ts...>::is_type_at () const {
  return cons == I;
}

//! \endcond

}//namespace pgs

#endif //!defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
