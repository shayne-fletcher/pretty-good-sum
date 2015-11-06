#if !defined (RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H)
#  define RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H

//! \file recursive_union.hpp
//!
//! \brief A type modeling a "recursive union".
//!
//! The recursive union datatype here is designed to serve as the
//! implementation mechanism of the sum type.

#  include "logical.hpp"
#  include "recursive_wrapper.hpp"
#  include "type_traits.hpp"

#  include <stdexcept>

namespace pgs {

  //! \brief A type to model a sum constructor

  template <class> struct constructor {};

  //! \brief `recursive union<>` primary template

  template <class...> struct recursive_union {};

  //! \brief A type to model an overload

  template <class T> struct overload_tag {};

  template<std::size_t I, class T, class... Ts>
  struct recursive_union_indexer {

    static constexpr auto ref (recursive_union<T, Ts...>& u) {
      return union_indexer<I - 1, Ts...>::ref (u.r);
    }
    static constexpr auto ref (recursive_union<T, Ts...> const& u) {
      return union_indexer<I - 1, Ts...>::ref (u.r);
    }
    static constexpr auto ptr (recursive_union<T, Ts...>& u) {
      return union_indexer<I - 1, Ts...>::ptr (u.r);
    }
    static constexpr auto ptr (recursive_union<T, Ts...> const& u) {
      return union_indexer<I - 1, Ts...>::ptr (u.r);
    }
  };

  template <class T, class... Ts>
  struct recursive_union_indexer<0, T, Ts...> {
    static constexpr T& ref (recursive_union<T, Ts...>& u) {
      return u.v;
    }
    static constexpr T const& ref (recursive_union<T, Ts...> const& u) {
      return u.v;
    }
    static constexpr T* ptr (recursive_union<T, Ts...>& u) {
      return std::addressof (u.v);
    }
    static constexpr T const* ptr (recursive_union<T, Ts...> const& u) {
      return std::addressof (u.v);
    }
  };

  //! \brief Primary template
  //!
  //! \tparam R result type
  //! \tparam T head of the parameter pack
  //! \tparam Ts tail of the paramter pack
  //!
  //! If the `f` (of type `F`) under consideration can be applied to
  //! `t` (that is, applied to a `T`) do so, else continue searching
  //! by stripping `f` off the head of the parameter pack and
  //! recurively invoking `visit`.

  template <class R, class T, class... Ts>
  struct union_visitor {

    using result_type = R; //result type of 'visit'

    template <
      class O, class F, class... Fs,
      class = typename std::enable_if<is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O>, T const& t, F&& f, Fs&&...) {
      return std::forward<F>(f)(t);
    }

    template <
      class F, class O, class... Fs,
      class = typename std::enable_if<!is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O> o, T const& t, F&&, Fs&&... fs) {
      return union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }

    template <
      class O, class F, class... Fs,
      class = typename std::enable_if<is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O>, T& t, F&& f, Fs&&...) {
      return std::forward<F>(f)(t);
    }

    template <
      class F, class O, class... Fs,
      class = typename std::enable_if<!is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O> o, T& t, F&&, Fs&&...fs) {
      return union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
  };

  //! \brief Partial specialization for `void` result type
  //!
  //! If the `f` (of type `F`) under consideration can be applied to
  //! `t` (that is, applied to a `T`) do so, else continue searching
  //! by stripping `f` off the head of the parameter pack and
  //! recurively invoking `visit`.

  template <class T, class... Ts>
  struct union_visitor<void, T, Ts...> {

    using result_type = void;//result type of 'visit'

    template <
      class O, class F, class... Fs,
      class = typename std::enable_if<is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O>, T const& t, F&& f, Fs&&...) {
      std::forward<F>(f)(t);
    }

    template <
      class F, class O, class... Fs,
      class = typename std::enable_if<!is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O> o, T const& t, F&&, Fs&&... fs) {
      union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }

    template <
      class O, class F, class... Fs,
      class = typename std::enable_if<is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O>, T& t, F&& f, Fs&&...) {
      std::forward<F>(f)(t);
    }

    template <
      class F, class O, class... Fs,
      class = typename std::enable_if<!is_callable<F, T>::value>::type
      >
    static result_type visit (overload_tag<O> o, T& t, F&&, Fs&&...fs) {
      union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
  };

  //! \class invalid_sum_type_access
  //!
  //! \brief Exception type raised on an invalid access into a
  //! `recursive_union<>`

  struct invalid_sum_type_access : std::logic_error {
    explicit invalid_sum_type_access (std::string const& what)
      : std::logic_error {what}
    {}
    explicit invalid_sum_type_access (std::string&& what)
      : std::logic_error {std::move (what)}
    {}
    explicit invalid_sum_type_access (char const* what)
      : std::logic_error {what}
    {}
  };

  //! \class range
  //!
  //! \brief Compile time sequence of integers

  template <std::size_t...> struct range {};

  //! \cond
  namespace detail {
    template <std::size_t Z, std::size_t N, std::size_t... Ns>
    struct mk_range  : mk_range <Z, N - 1, N, Ns...>
    {};

    template <std::size_t Z, std::size_t... Ns>
    struct mk_range<Z, Z, Ns...> {
      using type = range<Z, Ns...>;
    };
  }//namespace detail
  //! \endcond

  //! \brief Alias type for a range

  template <std::size_t Z, std::size_t N>
  using range_t =  typename detail::mk_range<Z, N>::type;

  //! \brief Partial specialization
  //!
  //! \tparam R result type
  //! \tparam Ts Paramter pack
  //!
  //! Base case for result type is `R`

  template <class R, class... Ts>
  struct union_visitor<R, range<>, Ts...> {
  
    using result_type = R;//result type of 'visit'
  
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...> const&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  
    template <class... Fs>
    static result_type visit (recursive_union<Ts...>&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \tparam R result type
  //! \tparam Ts Paramter pack
  //!
  //! Base case for result type is `void`

  template <class... Ts>
  struct union_visitor<void, range<>, Ts...> {
  
    using result_type = void;//the result 'visit'
  
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...> const&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...>&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \tparam R result type
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack (of union types)
  //! \tparam Ts Tail of the parameter pack (of union types)
  //!
  //! Result type is `R`, `range<>` is non-empty.

  template <class R, std::size_t I, std::size_t... Is, class T, class... Ts>
  struct union_visitor<R, range<I, Is...>, T, Ts...> {
  
    using type = T; //the type of the value
    using result_type = R; //result type of 'visit'
  
    //'const' overload ('recursive_union<type, Ts...> const& u')
    template <class... Fs>
    static result_type visit (
      recursive_union<type, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<type> o{};
        return union_visitor<result_type, T>::visit(
                                        o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        return union_visitor<result_type, range<Is...>, Ts...>::visit(
                                         u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //'non-const' overload ('recursive_union<T, Ts...>& u')
    template <class... Fs>
    static result_type visit (
      recursive_union<T, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<T> o{};
        return union_visitor<result_type, T>::visit(
                                          o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        return union_visitor<result_type, range<Is...>, Ts...>::visit(
                                           u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \tparam R result type
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)

  template <class R, std::size_t I, std::size_t... Is, class T, class... Ts>
  struct union_visitor<R, range<I, Is...>, recursive_wrapper<T>, Ts...> {
  
    using type = T; //the type held by the value
    using U = recursive_wrapper<type>;//the type of the value
    using result_type = R;//the type returned by 'visit'
    
    //'const' overload ('recursive_union<recursive_wrapper<type>, Ts...>
    //const& u')
    template <class... Fs>
    static result_type visit (
       recursive_union<U, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is of type recursive_wrapper<type>, call 'get ()'
        overload_tag<type> o{};
        return union_visitor<result_type, type>::visit(
                                     o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        return union_visitor<result_type, range<Is...>, Ts...>::visit(
                                            u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //'non-const' overload ('recursive_union<U, Ts...>& u')
    template <class... Fs>
    static result_type visit (
      recursive_union<U, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is of type recursive_wrapper<type>, call 'get ()'
        overload_tag<type> o{};
        return union_visitor<result_type, T>::visit(
                                      o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        return union_visitor<result_type, range<Is...>, Ts...>::visit(
                                             u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)

  template <std::size_t I, std::size_t... Is, class T, class... Ts>
  struct union_visitor<void, range<I, Is...>, T, Ts...> {
  
    using type = T; //the type of the value
    using result_type = void; //result type of 'visit'
  
    //'const' overload ('recursive_union<type, Ts...> const& u')
    template <class... Fs>
    static result_type
    visit (
      recursive_union<type, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<type> o{};
        union_visitor<result_type, type>::visit(
                                              o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        union_visitor<result_type, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //'non-const' overload ('recursive_union<T, Ts...>& u')
    template <class... Fs>
    static result_type
    visit (
      recursive_union<T, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<type> o{};
        union_visitor<result_type, type>::visit(
                                              o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        union_visitor<result_type, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
  };
  
  //! \brief Partial specialization
  //!
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)

  //U='recursive_wrapper<T>', 'Ts', return type 'void'
  template <std::size_t I, std::size_t... Is, class T, class... Ts>
  struct union_visitor<void, range<I, Is...>, recursive_wrapper<T>, Ts...> {
  
    using type = T; //the type held by the value
    using U = recursive_wrapper<type>;//the type of the value
    using result_type = void;//the type returned by 'visit'
  
    //'const' overload ('recursive_union<recursive_wrapper<type>, Ts...>
    //const& u')
    template <class... Fs>
    static result_type visit (
       recursive_union<U, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is a reference (call 'get ()')
        overload_tag<type> o{};
        union_visitor<void, type>::visit(
                                       o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        union_visitor<void, range<Is...>, Ts...>::visit (
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //'non-const' overload ('recursive_union<U, Ts...>& u')
    template <class... Fs>
    static result_type visit (
      recursive_union<U, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is a reference (call 'get ()')
        overload_tag<type> o{};
        union_visitor<void, type>::visit(
                                       o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        union_visitor<void, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Full specialization

  template <>
  struct recursive_union<> {
    void copy (std::size_t, recursive_union const&) {}
    void move (std::size_t, recursive_union&&) {}
    void destruct (std::size_t) {}
    bool compare (std::size_t, recursive_union const&) const { return false; }
  };
  
  #  if defined(_MSC_VER)
  #    pragma warning(push)
  #    pragma warning(disable:4624)
  #  endif//defined (_MSC_VER)
  
  //! \brief Partial specialization
  //!
  //! \tparam T Type of `v` in the union
  //! \tparam Ts `recursive_union<Ts...>` is the type of `r` in the union

  template <class T, class... Ts>
  struct recursive_union<T, Ts...> {
  
    //! \brief Default ctor

    recursive_union () 
    {}
  
    //! \brief Construct (a `T`) into `v`

    template <class... Args>
    explicit recursive_union (constructor<T>, Args&&... args) 
      : v (std::forward<Args>(args)...)
    {}
  
    //! \brief Construct (a `recursive_wrapper<T>`) into `v`
    //!
    //! \details `U` is not `T` but `T` is a recursive wrapper and `U`
    //! is the type contained in `T`

    template <class U, class... Args,
    typename std::enable_if<
       and_<
        is_recursive_wrapper<T>
      , std::is_same<U, unwrap_recursive_wrapper_t<T>>>::value, int>::type = 0
    >
    explicit recursive_union (constructor<U>, Args&&... args)
      noexcept (std::is_nothrow_constructible<U, Args...>::value)
    : v (std::forward<Args>(args)...)
    {}
  
    //! \brief Construct into `r`
    //!
    //! \details `U` is not `T` and `T` is not a recursive wrapper or,
    //! `U` is not the type contained in `T`

    template <class U, class... Args,
    typename std::enable_if<
      !and_<
        is_recursive_wrapper<T>
      , std::is_same<U, unwrap_recursive_wrapper_t<T>>>::value, int>::type = 0
    >
    explicit recursive_union (constructor<U> t, Args&&... args)
      noexcept(std::is_nothrow_constructible<Ts..., constructor<U>, Args...>::value)
      : r (t, std::forward<Args>(args)...)
    {}
  
    //! \brief Dtor

    ~recursive_union () 
    {}
  
    //! \brief Copy
    //!
    //! \details If `i` is \f$0\f$ then emplace copy-construct `v`
    //! from `u.v` else, recursively invoke `copy` on `r` and a
    //! decremented `i`.
    //!
    //! \param i When zero, the destination of the copy
    //! \param u Source of the copy

    void copy (std::size_t i, recursive_union const& u)
      noexcept(
       std::is_nothrow_copy_constructible<T>::value
       && noexcept (std::declval<recursive_union>().r.copy (i - 1, u.r))
      ) {
      //std::cout << "recursive_union.copy ()\n";
      if (i == 0) {
        new (std::addressof (v)) T (u.v);
      }
      else {
        r.copy (i - 1, u.r);
      }
    }
  
    //! \brief Move
    //!
    //! \details If `i` is \f$0\f$ then emplace move-construct `v`
    //! from `u.v` else, recursively invoke `move` on `r` and a
    //! decremented `i`.
    //!
    //! \param i When zero, the destination of the move
    //! \param u Source of the move

    void move (std::size_t i, recursive_union&& u) 
      noexcept (
        std::is_nothrow_move_constructible<T>::value
       && noexcept (std::declval<recursive_union>().r.move (i - 1, std::move (u.r)))
       ) {
      //std::cout << "recursive_union.move ()\n";
      if (i == 0) {
        new (std::addressof (v)) T (std::move (u.v));
      }
      else {
        r.move (i - 1, std::move (u.r));
      }
    }
  
    //! \brief Destruct
    //!
    //! \details If `i` is \f$0\f$ then destruct `v` else, recursively
    //!  invoke `destruct` on `r` and a decremented `i`.
    //!
    //! \param i When zero, the destination of the destruction

    void destruct (std::size_t i)
      noexcept (std::is_nothrow_destructible<T>::value 
       && noexcept (std::declval<recursive_union>().r.destruct (i - 1))) {
      if (i == 0) {
        v.~T ();
      }
      else {
        r.destruct (i - 1);
      }
    }
  
    //! \brief Equality comparison
    //!
    //!
    //! \details If `i` is \f$0\f$ then compare `v` against `rhs.v`
    //!  else, recursively invoke `compare` on `r` and a decremented
    //!  `i`.
    //!
    //! \param i When zero, indicates the LHS object of the
    //! comparision
    //! \param rhs The value to compare the LHS against

    bool compare (size_t i, recursive_union const& rhs) const 
      noexcept {
      return i == 0 ? v == rhs.v : r.compare (i - 1, rhs.r);
    }
  
    //! \brief An anonymous (smart) union
    //! \details An anonymous union consisting of a field `v` of type
    //! `T` and a field `r` of type `recursive_union<Ts...>`
    union {
      T v; //!< Value or...
      recursive_union<Ts...> r; //< ... recursive union
    };
  };
  
#  if defined(_MSC_VER)
#    pragma warning(pop)
#  endif//defined (_MSC_VER)

}//namespace pgs

#endif //!defined (RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H)
