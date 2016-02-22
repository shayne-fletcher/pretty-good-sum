#if !defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
#  define LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H

#  include <type_traits>

//! \file logical.hpp
//! \brief Conjunctions and disjunctions over predicate packs
//!
//! A definition of `fold_left` is provided that is then used to
//! implement conjunctions and disjunctions over variadic predicate
//! packs.

namespace BloombergLP {

namespace pgs {

//! \brief Fold left, primary template
//! 
//! Left fold of a metafunction through a parameter pack
//!
//! \tparam Acc Accumulator
//! \tparam F Metafunction class
//! \tparam Ts Parameter pack (possibly empty)
//!
//! \returns A terminal state produced by recursive application of the
//! provided binary metafunction `F` to the initial state `Acc` and
//! the elements of the sequence `Ts`.
//!
//! \note \f$fold\_left\;f\;acc\;[b_{1};\cdots;b_{n}]\f$ computes
//! \f$f\;(\cdots\;(f\;(f\;a\;b_{1})\;b_{2})\;\cdots)\;b_{n}\f$

template<class F, class Acc, class... Ts>
struct fold_left : Acc {
};

//! \brief Fold left, partial specialization matching non-empty packs

template <class F, class Acc, class T, class... Ts>
struct fold_left<F, Acc, T, Ts...> : 
  fold_left <F, typename F::template apply<Acc, T>::type, Ts...> {
};

//! \cond

namespace detail {
  struct or_helper {
    template <class Acc, class T>
    struct apply : std::integral_constant<bool, Acc::value || T::value> {
    };
  };

  struct and_helper {
    template <class Acc, class T>
    struct apply : std::integral_constant<bool, Acc::value && T::value> {
    };
};

}//namespace detail

//! \endcond

//! \brief Parameter pack disjunction
//!
//! Fold logical `||`  through a predicate pack
//!
//! \tparam Predicate pack
//!
//! \returns Type equivalent to one of `std::true_type` or
//! `std::false_type`

template <class... Ts>
struct or_ : fold_left <detail::or_helper, std::false_type, Ts...> {
};

//! \brief Parameter pack conjunction
//!
//! Fold logical `&&` through a predicate pack
//!
//! \tparam Ts Predicate pack
//!
//! \returns Type equivalent to one of `std::true_type` or
//! `std::false_type`

template <class... Ts>
struct and_ : fold_left <detail::and_helper, std::true_type, Ts...> {
};

}//namespace pgs

}//namespace BloombergLP

#endif //!defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
