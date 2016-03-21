#include <pgs/pgs.hpp>

#include <algorithm>
#include <ostream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace BloombergLP {
namespace qfd {
namespace contract {

struct E_unary_operation;

using expression_t = pgs::sum_type<
  pgs::recursive_wrapper<E_unary_operation>
>;

namespace unop_tags {

  struct inv {};
  struct abs {};

} // namespace tags

template <class T>
struct Unary_operation {
  expression_t exp;

  template <class E>
  explicit//Comment this line to observe compile failure 
    Unary_operation (E&& e) : exp { std::forward<E>(e) }
  {}

};

using unary_operation_t = pgs::sum_type<
    Unary_operation<unop_tags::inv>
  , Unary_operation<unop_tags::abs>
>;

struct E_unary_operation {
  unary_operation_t op;

  template <class T> 
  explicit E_unary_operation (T&& o) : op {o} 
  {}
};

inline std::ostream& operator << (

  std::ostream& os, unary_operation_t const& u) {
  return u.match<std::ostream&> (
   [&](Unary_operation<unop_tags::inv> const& u) -> std::ostream& { return os << "inv"; },
   [&](pgs::otherwise) -> std::ostream& { return os; }
  );

}


} // namespace contract 
} // namespace qfd 
} // namespace BloombergLP 

int main () {

  return 0;
}
