#if !defined(RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H)
#  define RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H

/*!
 * \file recursive_wrapper.hpp
 */

// Copyright Eric Friedman, Itay Maman 2002-2003

#include <type_traits>

namespace pgs {

template <class> class recursive_wrapper; //fwd. decl.

//is_recursive_wrapper

template <class> 
struct is_recursive_wrapper : std::false_type 
{};
template <class T> 
struct is_recursive_wrapper<recursive_wrapper<T>> : std::true_type 
{};

//unwrap_recursive_wrapper

template <class T>
struct unwrap_recursive_wrapper {
  typedef T type;
};
template <class T>
struct unwrap_recursive_wrapper<recursive_wrapper<T>> {
  typedef T type;
};
template <class W>
using unwrap_recursive_wrapper_t = typename unwrap_recursive_wrapper<W>::type;

//recursive_wrapper

template <class T>
class recursive_wrapper {
private:
  T* p_;

private:  
  recursive_wrapper& assign (T const& rhs);

public:
  typedef T type;

public:

  template <class... Args> recursive_wrapper (Args&&... args);
  recursive_wrapper (recursive_wrapper const& rhs);
  recursive_wrapper (T const& rhs);
  recursive_wrapper(recursive_wrapper&& rhs);
  recursive_wrapper(T&& rhs);

  ~recursive_wrapper();

  recursive_wrapper& operator=(recursive_wrapper const& rhs);
  recursive_wrapper& operator=(T const& rhs);
  recursive_wrapper& operator=(recursive_wrapper&& rhs) noexcept;
  recursive_wrapper& operator=(T&& rhs);
  void swap(recursive_wrapper& rhs) noexcept;

  T& get();
  T const& get() const;
  T* get_pointer();
  T const* get_pointer() const;
};

template<class T>
  template <class... Args> 
recursive_wrapper<T>::recursive_wrapper (Args&&... args) : 
    p_(new T (std::forward<Args>(args)...)) 
{}

template<class T>
recursive_wrapper<T>::recursive_wrapper (recursive_wrapper const& rhs) : 
    p_ (new T (rhs.get ())) 
{}

template<class T>
recursive_wrapper<T>::recursive_wrapper (T const& rhs) : p_ (new T (rhs)) 
{}

template<class T>
recursive_wrapper<T>::recursive_wrapper(recursive_wrapper&& rhs) : 
    p_ (new T (std::move (rhs.get ()))) 
  {}

template <class T>
recursive_wrapper<T>::recursive_wrapper(T&& rhs) : p_ (new T (std::move (rhs))) 
{}

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

#endif //!defined(RECURSIVE_WRAPPER_77D62153_38D6_4311_8B5C_384D740BDF6E_H)
