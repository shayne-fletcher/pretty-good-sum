#if !defined(TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H)
#  define TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H

//! \file type_traits.hpp
//!
//! \brief At this time contains only a metafunction for determining
//! if a given type `F` is "callable" on a parameter pack of arguments
//! `Args`

#  include <type_traits>

#  if defined (__GNUC__)
#    if __cplusplus <= 201103L
namespace std {

  template <bool B, class T = void>
  using enable_if_t = typename enable_if<B, T>::type; //C++14

  template <class B>
  struct negation : std::integral_constant <bool, !B::value>
  {}; // C++17
  
}//namespace std
#  endif //__cplusplus <= 201103L
#endif//defined (__GNU_C)

namespace BloombergLP {

namespace pgs { 

  //! \brief Convenience metafunction
  template <class T, class U>
  using not_is_same = std::negation<std::is_same<T, U>>;

  //! \cond
  namespace detail {
    struct no {};
  }//namespace detail
  //! \endcond

  //! \brief Metafunction to determine if an instance of a type `F`
  //! can be applied to a tuple of parameters with types determined by
  //! the template parameter pack `Args`

  template<typename F, typename...Args>
  struct is_callable {

    //!\cond
    template<class G, class...Qs>
    static auto check(G g, Qs...qs) -> decltype(g(qs...));

    static detail::no check(...);

    static constexpr bool value = 
     !std::is_same<
        detail::no
      , decltype(check (std::declval<F>(), std::declval<Args>()...))>::value;

    //!\endcond

  };

}//namespace pgs

}//namespace BloombergLP

#endif //!defined(TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H)
