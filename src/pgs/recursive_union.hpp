#if !defined (RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H)
#  define RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H

//! \file recursive_union.hpp
//!
//! \brief A type modeling a "recursive union".
//!
//! The recursive union datatype here is designed to serve as the
//! implementation mechanism of the sum type.

#  include <pgs/logical.hpp>
#  include <pgs/recursive_wrapper.hpp>
#  include <pgs/type_traits.hpp>

#  include <stdexcept>
#  include <type_traits>
#  include <iostream>

namespace BloombergLP {

namespace pgs {

  //! \brief A type to model a sum constructor
  template <class> struct constructor {};

  //! \brief `recursive union<>` primary template
  template <class...> struct recursive_union {};

  //! \brief A type to model an overload
  template <class T> struct overload_tag {};

  //! \brief Dereference the value field in a
  //! `recursive_union<>`. This case handles values that are not
  //! `recursive_wrapper` instances.
  template <bool is_recursive_wrapper, class T, class... Ts>
  struct recursive_union_dereference {

    //! \brief Produce a non-`const` reference to the value field of
    //! the provided union
    static constexpr T& ref (recursive_union<T, Ts...>& u) {
      return u.v;
    }
    //! \brief Produce a `const` reference to the value field of the
    //! provided union
    static constexpr T const& ref (recursive_union<T, Ts...> const& u) {
      return u.v;
    }
    //! \brief Produce a non-`const` pointer to the value field of the
    //! provided union
    static constexpr T* ptr (recursive_union<T, Ts...>& u) {
      return std::addressof (u.v);
    }
    //! \brief Produce a `const` pointer to the value field of the
    //! provided union
    static constexpr T const* ptr (recursive_union<T, Ts...> const& u) {
      return std::addressof (u.v);
    }
  };

  //! \brief Dereference the value field in a
  //! `recursive_union<>`. This specialization handles values that are
  //! `recursive_wrapper` instances.
  template <class T, class... Ts>
  struct recursive_union_dereference<true, T, Ts...> {

    //! \brief Produce a non-`const` reference to the object referred
    //! to by the value field of the provided union
    static auto ref (recursive_union<T, Ts...>& u)
      -> decltype (u.v.get ())& {
      return u.v.get ();
    }
    //! \brief Produce a `const` reference to the object referred
    //! to by the value field of the provided union
    static auto ref (recursive_union<T, Ts...> const& u)
      -> decltype (u.v.get ()) const& {
      return u.v.get ();
    }
    //! \brief Produce a non-`const` pointer to the object referred
    //! to by the value field of the provided union
    static constexpr auto ptr (recursive_union<T, Ts...>& u) 
      -> decltype (u.v.get ())* {
      return std::addressof (u.v.get ());
    }
    //! \brief Produce a `const` pointer to the object referred
    //! to by the value field of the provided union
    static constexpr auto ptr (recursive_union<T, Ts...> const& u) 
      -> decltype (u.v.get ()) const*  {
      return std::addressof (u.v.get ());
    }
  };

  //! \brief The purpose of this type is to walk a `recursive_union<>`
  //! and return the value field of the `I`th union in the chain.
  template<std::size_t I, class T, class... Ts>
  struct recursive_union_indexer {

    //!\brief Decrement `I`, strip off `T` and recurse
    static constexpr auto ref (recursive_union<T, Ts...>& u)
      -> decltype (recursive_union_indexer<I - 1, Ts...>::ref (u.r))
    {
      return recursive_union_indexer<I - 1, Ts...>::ref (u.r);
    }
    //!\brief Decrement `I`, strip off `T` and recurse
    static constexpr auto ref (recursive_union<T, Ts...> const& u) 
      -> decltype (recursive_union_indexer<I - 1, Ts...>::ref (u.r))
    {
      return recursive_union_indexer<I - 1, Ts...>::ref (u.r);
    }
    //!\brief Decrement `I`, strip off `T` and recurse
    static constexpr auto ptr (recursive_union<T, Ts...>& u) 
      -> decltype (recursive_union_indexer<I - 1, Ts...>::ptr (u.r)) {
      return recursive_union_indexer<I - 1, Ts...>::ptr (u.r);
    }
    //!\brief Decrement `I`, strip off `T` and recurse
    static constexpr auto ptr (recursive_union<T, Ts...> const& u) 
      -> decltype (recursive_union_indexer<I - 1, Ts...>::ptr (u.r)) {
      return recursive_union_indexer<I - 1, Ts...>::ptr (u.r);
    }
  };

  //! \brief The purpose of this type is to walk a `recursive_union<>`
  //! and return the value field of the `I`th union in the chain.
  template <class T, class... Ts>
  struct recursive_union_indexer<0, T, Ts...> {
    //! \brief Dereference the value field to produce a non-`const`
    //! reference
    static constexpr auto ref (recursive_union<T, Ts...>& u) 
      -> decltype (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u)) {
      return recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u);
    }
    //! \brief Dereference the value field to produce a `const`
    //! reference
    static constexpr auto ref (recursive_union<T, Ts...> const& u) 
      -> decltype (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u)) {
      return recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u);
    }
    //! \brief Dereference the value field to produce a non-`const`
    //! pointer
    static constexpr auto ptr (recursive_union<T, Ts...>& u) 
      -> decltype (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u))* {
      return std::addressof (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u));
    }
    //! \brief Dereference the value field to produce a `const`
    //! pointer
    static constexpr auto ptr (recursive_union<T, Ts...> const& u) 
      -> decltype (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u)) const* {
      return std::addressof (recursive_union_dereference<is_recursive_wrapper<T>::value, T, Ts...>::ref (u));
    }
  };

  //! \brief Primary template
  //!
  //! \anchor recursive_union_visitor_find_applicable_closure1
  //!
  //! \tparam R return type
  //! \tparam T head of the parameter pack
  //! \tparam Ts tail of the paramter pack
  //!
  //! If the `f` (of type `F`) under consideration can be applied to
  //! `t` (that is, applied to a `T`) do so, else continue searching
  //! by stripping `f` off the head of the parameter pack and
  //! recurively invoking `visit`.
  template <class R, class T, class... Ts>
  struct recursive_union_visitor {

    using result_type = R; //!< The return type of `visit`

    //! \brief `f` is callable on `t` (of type `T const&`)
    template <
      class O, class F, class... Fs,
      class = pgs::enable_if_t<is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O>, T const& t, F&& f, Fs&&...) {
      return std::forward<F>(f)(t);
    }
    //! \brief `f` is not callable on `t` (of type `T const&`),
    //! recurse
    template <
      class F, class O, class... Fs,
      class = pgs::enable_if_t<!is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O> o, T const& t, F&&, Fs&&... fs) {
      return recursive_union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
    //! \brief `f` is callable on `t` (of type `T&`)
    template <
      class O, class F, class... Fs,
      class = pgs::enable_if_t<is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O>, T& t, F&& f, Fs&&...) {
      return std::forward<F>(f)(t);
    }
    //! \brief `f` is not callable on `t` (of type `T&`)
    template <
      class F, class O, class... Fs,
      class = pgs::enable_if_t<!is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O> o, T& t, F&&, Fs&&...fs) {
      return recursive_union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
  };

  //! \brief Partial specialization for `void` return type
  //!
  //! \anchor recursive_union_visitor_find_applicable_closure2
  //!
  //! If the `f` (of type `F`) under consideration can be applied to
  //! `t` (that is, applied to a `T`) do so, else continue searching
  //! by stripping `f` off the head of the parameter pack and
  //! recurively invoking `visit`.
  template <class T, class... Ts>
  struct recursive_union_visitor<void, T, Ts...> {

    using result_type = void;//!< return type of `visit`

    //! \brief `f` is callable on `t` (of type `T const&`)
    template <
      class O, class F, class... Fs,
      class = pgs::enable_if_t<is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O>, T const& t, F&& f, Fs&&...) {
      std::forward<F>(f)(t);
    }
    //! \brief `f` is not callable on `t` (of type `T const&`),
    //! recurse
    template <
      class F, class O, class... Fs,
      class = pgs::enable_if_t<!is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O> o, T const& t, F&&, Fs&&... fs) {
      recursive_union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
    //! \brief `f` is callable on `t` (of type `T&`)
    template <
      class O, class F, class... Fs,
      class = pgs::enable_if_t<is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O>, T& t, F&& f, Fs&&...) {
      std::forward<F>(f)(t);
    }
    //! \brief `f` is not callable on `t` (of type `T&`)
    template <
      class F, class O, class... Fs,
      class = pgs::enable_if_t<!is_callable<F, T>::value, void>
      >
    static result_type visit (overload_tag<O> o, T& t, F&&, Fs&&...fs) {
      recursive_union_visitor::visit (o, t, std::forward<Fs>(fs)...);
    }
  };

  //! \class invalid_sum_type_access
  //!
  //! \brief Exception type raised on an invalid access into a
  //! `recursive_union<>`
  struct invalid_sum_type_access : std::logic_error {
    //! \brief Construct from `std::string const&`
    explicit invalid_sum_type_access (std::string const& what)
      : std::logic_error {what}
    {}
    //! \brief Construct from `std::string&&`
    explicit invalid_sum_type_access (std::string&& what)
      : std::logic_error {std::move (what)}
    {}
    //! \brief Construct from `char const*`
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
  //! \tparam R return type
  //! \tparam Ts Paramter pack
  //!
  //! Base case for return type is `R`
  template <class R, class... Ts>
  struct recursive_union_visitor<R, range<>, Ts...> {
  
    using result_type = R;//!< The return type of `visit`
  
    //! \brief This definition applies when `Ts...` is empty
    //! \brief Calls to this function always throw.
    //!
    //! \throws invalid_sum_type_access
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...> const&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }

    //! \brief This definition applies when `Ts...` is empty
    //! \brief Calls to this function always throw.
    //!
    //! \throws invalid_sum_type_access
    template <class... Fs>
    static result_type visit (recursive_union<Ts...>&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  };
  
  //! \brief Partial specialization
  //!
  //!
  //!
  //! \tparam R return type
  //! \tparam Ts Paramter pack
  //!
  //! Base case for return type is `void`
  template <class... Ts>
  struct recursive_union_visitor<void, range<>, Ts...> {
  
    using result_type = void;//!< The return type of `visit`
  
    //! \brief This definition applies when `Ts...` is empty
    //! \brief Calls to this function always throw.
    //!
    //! \throws invalid_sum_type_access
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...> const&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  
    //! \brief This definition applies when `Ts...` is empty
    //! \brief Calls to this function always throw.
    //!
    //! \throws invalid_sum_type_access
    template <class... Fs>
    static result_type visit (
      recursive_union<Ts...>&, std::size_t, Fs&&...) {
      throw invalid_sum_type_access{""};
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \anchor recursive_union_visitor_find_active_type1
  //!
  //! The role of this visitor is to "walk" the recursive union until
  //! the active type is reached. When it is, the form of visit that
  //! searches for the right closure to apply to the value there is
  //! invoked (see \ref recursive_union_visitor_find_applicable_closure1 "recurs  //!
  //! \tparam R return type
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack (of union types)
  //! \tparam Ts Tail of the parameter pack (of union types)
  //!
  //! return type is `R`, `range<>` is non-empty. This specialization
  //! applies when the head of the parameter pack is not a
  //! `reference_wrapper<>`
  template <class R, std::size_t I, std::size_t... Is, class T, class... Ts>
  struct recursive_union_visitor<R, range<I, Is...>, T, Ts...> {
  
    using type = T; //!< The type of the value
    using result_type = R; //!< The return type of `visit`
  
    //! \brief `const` overload (`recursive_union<type, Ts...> const&`)
    template <class... Fs>
    static result_type visit (
      recursive_union<type, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<type> o{};
        return recursive_union_visitor<result_type, T>::visit(
                                        o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        return recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                         u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //! \brief non-`const` overload (`recursive_union<type, Ts...>&`)
    template <class... Fs>
    static result_type visit (
      recursive_union<T, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a reference (no call 'get ()')
        overload_tag<T> o{};
        return recursive_union_visitor<result_type, T>::visit(
                                          o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        return recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                           u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \anchor recursive_union_visitor_find_active_type2
  //!
  //! The role of this visitor is to "walk" the recursive union until
  //! the active type is reached. When it is, the form of visit that
  //! searches for the right closure to apply to the value there is
  //! invoked (see \ref recursive_union_visitor_find_applicable_closure1 "recursive union visitor for finding a matching closure")
  //!
  //! \tparam R return type
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)
  //! 
  //! return type is `R`, `range<>` is non-empty. This specialization
  //! applies when the head of the parameter pack is a
  //! `recursive_wrapper<>`
  template <class R, std::size_t I, std::size_t... Is, class T, class... Ts>
  struct recursive_union_visitor<R, range<I, Is...>, recursive_wrapper<T>, Ts...> {
  
    using type = T; //!< The type held by the value
    using U = recursive_wrapper<type>;//!< The type of the value
    using result_type = R; //!< The type returned by `visit`
    
    //! \brief `const` overload (`recursive_union<U, Ts...> const&`)
    template <class... Fs>
    static result_type visit (
       recursive_union<U, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is of type recursive_wrapper<type>, call 'get ()'
        overload_tag<type> o{};
        return recursive_union_visitor<result_type, type>::visit(
                                     o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        return recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                            u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //! \brief non-`const` overload (`recursive_union<U, Ts...>&`)
    template <class... Fs>
    static result_type visit (
      recursive_union<U, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is of type recursive_wrapper<type>, call 'get ()'
        overload_tag<type> o{};
        return recursive_union_visitor<result_type, T>::visit(
                                      o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        return recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                             u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Partial specialization
  //!
  //! \anchor recursive_union_visitor_find_active_type3
  //!
  //! The role of this visitor is to "walk" the recursive union until
  //! the active type is reached. When it is, the form of visit that
  //! searches for the right closure to apply to the value there is
  //! invoked (see \ref recursive_union_visitor_find_applicable_closure2 "recursive union visitor for finding a matching closure")
  //!
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)
  //!
  //! return type is `void`, `range<>` is non-empty.! This specialization
  //! applies when the head of the parameter pack is not a
  //! `recursive_wrapper<>`
  template <std::size_t I, std::size_t... Is, class T, class... Ts>
  struct recursive_union_visitor<void, range<I, Is...>, T, Ts...> {
  
    using type = T; //!< The type of the value
    using result_type = void; //!< The return type of `visit`
  
    //! \brief 'const' overload ('recursive_union<type, Ts...> const& u')
    template <class... Fs>
    static result_type
    visit (
      recursive_union<type, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a wrapper (no call 'get ()')
        overload_tag<type> o{};
        recursive_union_visitor<result_type, type>::visit(
                                              o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //! \brief 'non-const' overload ('recursive_union<T, Ts...>& u')
    template <class... Fs>
    static result_type
    visit (
      recursive_union<T, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is not a wrapper (no call 'get ()')
        overload_tag<type> o{};
        recursive_union_visitor<result_type, type>::visit(
                                              o, u.v, std::forward<Fs>(fs)...);
      }
      else {
        recursive_union_visitor<result_type, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
  };
  
  //! \brief Partial specialization
  //!
  //! \anchor recursive_union_visitor_find_active_type4
  //!
  //! The role of this visitor is to "walk" the recursive union until
  //! the active type is reached. When it is, the form of visit that
  //! searches for the right closure to apply to the value there is
  //! invoked (see \ref recursive_union_visitor_find_applicable_closure2 "recursive union visitor for finding a matching closure")
  //!
  //! \tparam I Head of an integer pack
  //! \tparam I Tail of an integer pack
  //! \tparam T Head of the parameter pack
  //! \tparam Ts Tail of the parameter pack (of union types)
  //!
  //! return type is `void`, `range<>` is non-empty. This specialization
  //! applies when the head of the parameter pack is a
  //! `recursive_wrapper<>`
  template <std::size_t I, std::size_t... Is, class T, class... Ts>
  struct recursive_union_visitor<
     void, range<I, Is...>, recursive_wrapper<T>, Ts...> {

    //U='recursive_wrapper<T>', 'Ts', return type 'void'
  
    using type = T; //!< The type held by the value
    using U = recursive_wrapper<type>;//!< The type of the value
    using result_type = void;//!< The type returned by `visit`
  
    //! \brief `const` overload (`recursive_union<U, Ts...> const&`)
    template <class... Fs>
    static result_type visit (
       recursive_union<U, Ts...> const& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is a reference (call 'get ()')
        overload_tag<type> o{};
        recursive_union_visitor<void, type>::visit(
                                       o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        recursive_union_visitor<void, range<Is...>, Ts...>::visit (
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  
    //! \brief non-`const` overload (`recursive_union<U, Ts...>&`)
    template <class... Fs>
    static result_type visit (
      recursive_union<U, Ts...>& u, std::size_t i, Fs&&... fs) {
      if (i == I) {
        //'u' is a reference (call 'get ()')
        overload_tag<type> o{};
        recursive_union_visitor<void, type>::visit(
                                       o, u.v.get (), std::forward<Fs>(fs)...);
      }
      else {
        recursive_union_visitor<void, range<Is...>, Ts...>::visit(
                                              u.r, i, std::forward<Fs>(fs)...);
      }
    }
  };
  
  //! \brief Full specialization
  //! 
  //! This specialization applies when there are no more "cases" in
  //! the sum
  template <>
  struct recursive_union<> {
    //! \brief `copy` is a no-op
    void copy (std::size_t, recursive_union const&) {}
    //! \brief `move` is a no-op
    void move (std::size_t, recursive_union&&) {}
    //! \brief `destruct` is a no-op
    void destruct (std::size_t) {}
    //! \brief `compare` returns `false`
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
  
    //! \brief Construct a `T` into `v`
    //!
    //! `T` and `U` are the same
    template <class... Args>
    explicit recursive_union (constructor<T>, Args&&... args) 
      : v (std::forward<Args>(args)...)
    {}

    //! \brief Construct (a `recursive_wrapper<T>`) into `v`
    //!
    //! `T` and `U` are different but `T` is a recursive wrapper and
    //! `U` is the type contained in `T`
    template <class U, class... Args,
    pgs::enable_if_t<
      is_recursive_wrapper_containing<T, U>::value, int> = 0
    >
    explicit recursive_union (constructor<U>, Args&&... args)
      noexcept (std::is_nothrow_constructible<U, Args...>::value)
    : v (std::forward<Args>(args)...)
    {}

    //! \brief Construct into `r`
    //!
    //! `T` and `U` are different and `T` is not a reference wrapper
    //! with `U` the type contained in `T`.
   template <class U, class... Args,
   pgs::enable_if_t<
      and_<
          not_is_same<T, U>
        , not_is_recursive_wrapper_containing<T, U>>::value, int> = 0
    >
    explicit recursive_union (constructor<U> t, Args&&... args)
      noexcept(
         std::is_nothrow_constructible<Ts..., constructor<U>, Args...>::value
      )
      : r (t, std::forward<Args>(args)...)
    {}
  
    //! \brief Dtor
    ~recursive_union () 
    {}
  
    //! \brief Copy
    //!
    //! If `i` is \f$0\f$ then emplace copy-construct `v`
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
    //! If `i` is \f$0\f$ then emplace move-construct `v`
    //! from `u.v` else, recursively invoke `move` on `r` and a
    //! decremented `i`.
    //!
    //! \param i When zero, the destination of the move
    //! \param u Source of the move
    void move (std::size_t i, recursive_union&& u) 
      noexcept (
        std::is_nothrow_move_constructible<T>::value
       && noexcept (std::declval<recursive_union>().r.move (
                                                  i - 1, std::move (u.r)))
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
    //! If `i` is \f$0\f$ then destruct `v` else, recursively invoke
    //! `destruct` on `r` and a decremented `i`.
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
    //! If `i` is \f$0\f$ then compare `v` against `rhs.v` else,
    //!  recursively invoke `compare` on `r` and a decremented `i`.
    //!
    //! \param i When zero, indicates the LHS object of the
    //! comparision
    //! \param rhs The value to compare the LHS against
    bool compare (size_t i, recursive_union const& rhs) const 
      noexcept {
        return i == 0 ? v == rhs.v : r.compare (i - 1, rhs.r);
    }
  
    //! \brief An anonymous union
    //!
    //! An anonymous union consisting of a field `v` of type `T` and a
    //! field `r` of type `recursive_union<Ts...>`
    union {
      T v; //!< Value or...
      recursive_union<Ts...> r; //!< ... recursive union
    };
  };
  
#  if defined(_MSC_VER)
#    pragma warning(pop)
#  endif//defined (_MSC_VER)

}//namespace pgs

}//namespace BloombergLP

#endif //!defined (RECURSIVE_UNION_93F14ED0_E99B_4CF3_8A9B_EBE084D9079C_H)
