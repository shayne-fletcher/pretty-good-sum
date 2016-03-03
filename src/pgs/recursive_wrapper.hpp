#if !defined(RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H)
#  define RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H

//! \file recursive_wrapper.hpp
//!
//! \brief A workaround for the absence of recursive types
//!
//! \copyright Copyright Eric Friedman, Itay Maman 2002-2003 (Boost
//! software license)
//! \copyright Copyright Shayne Fletcher, 2015

#include <type_traits>
#include <utility> // std::forward<>()

namespace BloombergLP {

namespace pgs {

//! \cond
template <class T>
class recursive_wrapper; //fwd. decl.
//! \endcond

//! \brief Primary template of a metafunction to classify a type as
//! `recursive_wrapper<>` or not
//!
//! \returns Type equivalent to one of `std::true_type` or
//! `std::false_type`
template <class T> 
struct is_recursive_wrapper : std::false_type 
{};

//! \brief Partial specialization for types that are recursive
//! wrappers
template <class T> 
struct is_recursive_wrapper<recursive_wrapper<T>> : std::true_type 
{};

//! \brief Primary template of a metafunction to compute the type
//! contained by a `recursive_wrapper<>`.
//
//! \returns `T` if `T` is not a `recursive_wrapper<>` else `T::type`
template <class T>
struct recursive_wrapper_unwrap {
  typedef T type; //!< For `T` not a recursive wrapper
};

//! \brief Partial specialization for types that are recursive
//! wrappers
template <class T>
struct recursive_wrapper_unwrap<recursive_wrapper<T>> {
  typedef T type; //!< The type of the value contained by a recursive
                  //!wrapper
};

//! \brief Alias template for the result of
//! `recursive_wrapper_unwrap<>`.
template <class W>
using recursive_wrapper_unwrap_t = 
  typename recursive_wrapper_unwrap<W>::type;

//! \class recursive_wrapper<>
//!
//! \brief A type to enable working around the absence of recursive
//! types
template <class T>
class recursive_wrapper {
private:
  T* p_;

private:  
  recursive_wrapper& assign (T const& rhs);

public:
  typedef T type;   //!< Alias `type` for `T`

public:

  //! Forwarding ctor (heap allocates `type` instance)
  template <class... Args> recursive_wrapper (Args&&... args);
  //! Copy ctor
  recursive_wrapper (recursive_wrapper const& rhs);
  //! Copy-construct from `type`
  recursive_wrapper (type const& rhs);
  //! Move-construct from `recursive_wrapper`
  recursive_wrapper(recursive_wrapper&& rhs);
  //! Move-construct from `type`
  recursive_wrapper(type&& rhs);
  //! Destructor (frees heap-allocated `type` instance)
  ~recursive_wrapper();

  //! Copy-assign from `recursive_wrapper`
  recursive_wrapper& operator=(recursive_wrapper const& rhs);
  //! Copy-assign from `type`
  recursive_wrapper& operator=(type const& rhs);
  //! Move-assign from `recursive_wrapper`
  recursive_wrapper& operator=(recursive_wrapper&& rhs) noexcept;
  //! Move-assign from `type`
  recursive_wrapper& operator=(type&& rhs);

  //! Swap with `recursive_wrapper`
  void swap(recursive_wrapper& rhs) noexcept;

  type& get(); //!< Accessor to the `type` instance
  type const& get() const; //!< Accessor to the `type` instance
  type* get_pointer(); //!< Accessor to the `type` instance
  type const* get_pointer() const; //!< Accessor to the `type` instance
};

//! \brief `true` if contained values compare equal, false otherwise
template <class T>
bool operator== (
  recursive_wrapper<T> const& lhs, recursive_wrapper<T> const& rhs) {
  return lhs.get () == rhs.get ();
}

//! \brief `true` if contained values compare not equal, `false`
//! otherwise
template <class T>
bool operator!= (
  recursive_wrapper<T> const& lhs, recursive_wrapper<T> const& rhs) {
  return !(lhs.get () == rhs.get ());
}

}//namespace pgs

//! \cond
namespace pgs {

template<class T>
  template <class... Args> 
recursive_wrapper<T>::recursive_wrapper (Args&&... args) : 
    p_(new T (std::forward<Args>(args)...)) {
}

template<class T>
recursive_wrapper<T>::recursive_wrapper (recursive_wrapper const& rhs) : 
    p_ (new T (rhs.get ())) {
}

template<class T>
recursive_wrapper<T>::recursive_wrapper (T const& rhs) : p_ (new T (rhs)) {
}

template<class T>
recursive_wrapper<T>::recursive_wrapper(recursive_wrapper&& rhs) : 
    p_ (new T (std::move (rhs.get ()))) {
}

template <class T>
recursive_wrapper<T>::recursive_wrapper(T&& rhs) : p_ (new T (std::move (rhs))) {
}

template <class T>
recursive_wrapper<T>::~recursive_wrapper() {
  delete p_; 
}

template <class T>
recursive_wrapper<T>& 
  recursive_wrapper<T>::operator=(recursive_wrapper const& rhs){ 
  return assign (rhs.get()); 
}

template <class T>
recursive_wrapper<T>& recursive_wrapper<T>::operator=(T const& rhs) { 
  return assign (rhs); 
}

template <class T>
recursive_wrapper<T>& recursive_wrapper<T>::assign (T const& rhs) { 
  this->get() = rhs; return *this; 
}

template <class T>
void recursive_wrapper<T>::swap(recursive_wrapper& rhs) noexcept { 
  std::swap (p_, rhs.p_); 
}

template <class T>
recursive_wrapper<T>&
recursive_wrapper<T>::operator=(recursive_wrapper&& rhs) noexcept { 
  swap (rhs); 
  return *this; 
}

template <class T>
recursive_wrapper<T>&
recursive_wrapper<T>::operator=(T&& rhs) { 
  get() = std::move (rhs); 
  return *this; 
}

template <typename T>
inline void swap(
  recursive_wrapper<T>& lhs, recursive_wrapper<T>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename T>
T& recursive_wrapper<T>::get() { return *get_pointer(); }

template <typename T>
T const& recursive_wrapper<T>::get() const { return *get_pointer(); }

template <typename T>
T* recursive_wrapper<T>::get_pointer() { return p_; }

template <typename T>
T const* recursive_wrapper<T>::get_pointer() const { return p_; }

}//namespace pgs

}//namespace BloombergLP

//! \endcond

#endif //!defined(RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H)
