#if !defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
#  define SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H

#  include "recursive_union.hpp"

#  include <cstddef>

namespace pgs {

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

template <class T, class... Ts>
struct index_of {
  static auto const value = detail::index_of_impl<0u, T, Ts...>::value;
};

template <class... Ts>
class sum_type {
private:
  std::size_t cons;
  recursive_union<Ts...> data;
  
public:

  template <class T, class... Args>
  explicit sum_type (constructor<T> t, Args&&... args);

  sum_type () = delete;
  sum_type (sum_type const& other);
  sum_type (sum_type&& other);

  ~sum_type();

  sum_type& operator= (sum_type const& other);

  template <class R, class... Fs> R match(Fs&&... fs) const;
  template <class R, class... Fs> R match(Fs&&... fs);
  template <class... Fs> void match(Fs&&... fs) const;
  template <class... Fs> void match(Fs&&... fs);

};

template <class... Ts>
sum_type<Ts...>::sum_type (sum_type const& other) : cons (other.cons) {
  data.copy (cons, other.data);
}

template <class... Ts>
sum_type<Ts...>::sum_type (sum_type&& other) : cons (other.cons) {
  data.move (cons, std::move (other.data));
}

template <class... Ts>
  template <class T, class... Args>
  sum_type<Ts...>::sum_type (constructor<T> t, Args&&... args)
  : data (t, std::forward<Args>(args)...), cons (index_of<T, Ts...>::value)
{}

template <class... Ts>
sum_type<Ts...>::~sum_type() {
  data.destruct (cons);
}

template <class... Ts>
sum_type<Ts...>& sum_type<Ts...>::operator= (sum_type const& other) {
  if (std::addressof (other) == this)
    return *this;

  data.destruct (const);
  cons = s.cons;
  data.copy (cons, s.data);

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

}//namespace pgs

#endif //!defined(SUM_F8718480_DC1C_4410_84C0_DDDA2C2FED94_H)
