#if !defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
#  define LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H

/*!
 * \file logical.hpp
 * \brief Conjunctions and disjunctions over predicate packs
 *
 * A definition of fold_left is provided that is then used to
 * implement conjunctions and disjunctions over variadic predicate
 * packs.
 *
 */

#  include <type_traits>

namespace pgs {

/*!
 * \brief Primary template
 * 
 * @tparam Acc Accumulator
 * @tparam F Metafunction class
 * @tparam Ts Parameter pack (possibly empty)
 */
template<class F, class Acc, class... Ts>
struct fold_left : Acc {
};

/*!
 * \brief Specialization for a non-empty parameter pack
 *
 * @tparam Acc Accumulator
 * @tparam F Metafunction class
 * @tparam T Head of the parameter pack
 * @tparam Ts Tail of the parameter pack
 */
template <class F, class Acc, class T, class... Ts>
struct fold_left<F, Acc, T, Ts...> : 
    fold_left <F, typename F::template apply<Acc, T>::type, Ts...> {
};

namespace detail {

/*!
 * \brief Metaclass for implementation of 'or_'
 */
struct or_helper {
  /*!
   * \brief Metafunction used by 'or_'
   *
   * @tparam Acc The result of the compuation "so far"
   * @tparam T The current predicate under consideration
   */
  template <class Acc, class T>
  struct apply : std::integral_constant<bool, Acc::value || T::value> {
  };
};

/*!
 * \brief Metaclass for implementation of 'and_'
 */
struct and_helper {
  /*!
   * \brief Metafunction used by 'and_'
   *
   * @tparam Acc The result of the compuation "so far"
   * @tparam T The current predicate under consideration
   */
  template <class Acc, class T>
  struct apply : std::integral_constant<bool, Acc::value && T::value> {
  };
};

}//namespace detail

/*!
 * \brief Parameter pack disjunction
 * 
 * @Ts Predicate pack
 */
template <class... Ts>
struct or_ : fold_left <detail::or_helper, std::false_type, Ts...> {
};

/*!
 * \brief Parameter pack conjunction
 *
 * @Ts Predicate pack
 */
template <class... Ts>
struct and_ : fold_left <detail::and_helper, std::true_type, Ts...> {
};

}//namespace pgs

#endif //!defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
