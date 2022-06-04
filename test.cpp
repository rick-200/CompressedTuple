#include <iostream>

#include "compressed_tuple.h"
using namespace ctuple;

class A {};
class B {};
class C : private A, private B {};
class D {};

int main() {
  CompressedTuple<int, A, B, double, C, D> ct = {123,     A{}, B{},
                                                 123.456, C{}, D{}};
  cget<0>(ct) = 234;
  auto x0 = cget<0>(ct);
  auto x1 = cget<1>(ct);
  auto x2 = cget<2>(ct);
  auto x3 = cget<3>(ct);
  auto x4 = cget<4>(ct);
  printf("%llu\n", sizeof(ct));
  printf("%d\n", x0);
  printf("%f\n", x3);
  (void)x0;
  (void)x1;
  (void)x2;
  (void)x3;
  (void)x4;

  return 0;
}

// using namespace rllp;
// template <input_iterator_of<char> Tit>
// class calc_parser {
//   lazy_parser<Tit, int64_t> _factor;
//   lazy_parser<Tit, int64_t> _muldiv;
//   lazy_parser<Tit, int64_t> _addsub;
//   lazy_parser<Tit, int64_t> _exp;
//
//  public:
//   using parser_input = char;
//   using parser_output = int64_t;
//
//  public:
//   calc_parser() {
//     _factor %= pint64 |
//                (lit("(") >> _exp >> lit(")"))[([](const auto& val) -> int64_t
//                {
//                  return std::get<1>(val);
//                })];
//     _exp %= _factor;
//   }
//   bool parse(Tit& begin, const Tit& end, int64_t& result) const {
//     return _exp.parse(begin, end, result);
//   }
// };
//
// int64_t calc(const char* begin, const char* end) {
//   calc_parser<const char*> cp;
//   int64_t res;
//   cp.parse(begin, end, res);
//   return res;
//   // auto addsub =
//   //     (pint64 >> lit("+") >>
//   //      pint64)[([](const std::tuple<int64_t, unused_type, int64_t>& t) {
//   //       return std::get<0>(t) + std::get<2>(t);
//   //     })];
//   // auto par = addsub;
//   // int64_t ret;
//   // par.parse(begin, end, ret);
//   // return ret;
// }
////
//// template <class T>
//// void f(const T& s) {
////  (void)s;
////  printf("T&&\n");
////}
//// template <class T>
//// requires std::is_rvalue_reference_v<T&&>
//// void f(T&& s) {
////  (void)s;
////  T ss = std::move(s);
////  printf("const T&&\n");
////}
//
// int main() {
//  // std::is_rvalue_reference<const int&&>::value;
//  std::string s = "(123)";
//  // std::string sss = std::move(crs);
//  // f(crs);
//  calc(s.c_str(), s.c_str() + s.length());
//  //  static_assert(parser<int64>);
//  //  auto x =
//  //      (string_literial("{") >> int64{} >>
//  //       string_literial(
//  //           "}"))[([](const std::tuple<unused_type, int64_t, unused_type>&
//  t)
//  //           {
//  //        return std::get<1>(t);
//  //      })];
//  //  decltype(x)::parser_output out;
//  //  const char* s = "{1234}";
//  //  const char* begin = s;
//  //  const char* end = s + 9;
//
//  // x.parse(begin, end, out);
//}
