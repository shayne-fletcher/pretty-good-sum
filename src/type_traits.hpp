#if !defined(TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H)
#  define TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H

#  include <type_traits>

namespace pgs { 

  struct no {};

  template<typename F, typename...Args>
  struct is_callable {

    template<class G, class...Qs>
    static auto check(G g, Qs...qs) -> decltype(g(qs...));

    static no check(...);

    static constexpr bool value = 
     !std::is_same<
        no
      , decltype(check (std::declval<F>(), std::declval<Args>()...))>::value;

  	//using type =
  	//	decltype(check(std::declval<F>(), std::declval<Args>()...));
  };

}//namespace pgs

#endif //!defined(TYPE_TRAITS_FF88A4C7_86DE_434F_93B7_B03DCA5B4578_H)
