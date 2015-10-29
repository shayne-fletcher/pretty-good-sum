#if !defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
#  define LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H

#  include <type_traits>

namespace pgs {

//fold_left

template<class F, class Acc, class... Ts>
struct fold_left : Acc {
};

template <class F, class Acc, class T, class... Ts>
struct fold_left<F, Acc, T, Ts...> : 
    fold_left <F, typename F::template apply<Acc, T>::type, Ts...> {
};

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

//or

template <class... Ts>
struct or_ : fold_left <detail::or_helper, std::false_type, Ts...> {
};

//and

template <class... Ts>
struct and_ : fold_left <detail::and_helper, std::true_type, Ts...> {
};

}//namespace pgs

#endif //!defined (LOGICAL_70B10597_2771_496C_9203_4CD3986547F1_H)
