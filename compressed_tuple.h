#pragma once
#include <tuple>
#include <type_traits>
namespace ctuple {

template <class... Ts>
struct TypeList {
  static inline constexpr size_t size = sizeof...(Ts);
};

namespace tlop {

template <class T, size_t Idx>
struct get_nth;
template <class T1, class... Ts>
struct get_nth<TypeList<T1, Ts...>, 0> {
  using type = T1;
};
template <class T1, class... Ts, size_t Idx>
struct get_nth<TypeList<T1, Ts...>, Idx> {
  using type = typename get_nth<TypeList<Ts...>, Idx - 1>::type;
};

template <class TL, class T>
struct find_first;
template <class T, class... Ts>
struct find_first<TypeList<T, Ts...>, T> {
  static inline constexpr size_t value = 0;
};
template <class T1, class... Ts, class T>
struct find_first<TypeList<T1, Ts...>, T> {
  static inline constexpr size_t value =
      find_first<TypeList<Ts...>, T>::value + 1;
};

template <class...>
struct push_back;
template <class... Ts, class... Tadd>
struct push_back<TypeList<Ts...>, Tadd...> {
  using type = TypeList<Ts..., Tadd...>;
};

template <class...>
struct push_front;
template <class... Ts>
struct push_front<TypeList<Ts...>> {
  using type = TypeList<Ts...>;
};
template <class Taddfirst, class... Ts, class... Tadd>
struct push_front<TypeList<Ts...>, Taddfirst, Tadd...> {
  using type = typename push_front<TypeList<Taddfirst, Ts...>, Tadd...>::type;
};

template <class...>
struct combine;
template <class... Ts>
struct combine<TypeList<Ts...>> {
  using type = TypeList<Ts...>;
};
template <class... Trest, class... Ts1, class... Ts2>
struct combine<TypeList<Ts1...>, TypeList<Ts2...>, Trest...> {
  using type = typename combine<TypeList<Ts1..., Ts2...>, Trest...>::type;
};

template <class T, template <class> class Selector>
struct select;
template <template <class> class Selector>
struct select<TypeList<>, Selector> {
  using type = TypeList<>;
};
template <class T1, class... Ts, template <class> class Selector>
struct select<TypeList<T1, Ts...>, Selector> {
  using type = typename combine<
      typename std::conditional<Selector<T1>::value, TypeList<T1>,
                                TypeList<>>::type,      //选出第一个
      typename select<TypeList<Ts...>, Selector>::type  //递归选剩下的
      >::type;
};

template <class T>
struct unique;
template <>
struct unique<TypeList<>> {
  using type = TypeList<>;
};
template <class T1, class... Ts>
struct unique<TypeList<T1, Ts...>> {
 private:
  template <class T>
  struct _different_from_t1 {
    static inline constexpr bool value = !std::is_same_v<T, T1>;
  };

 public:
  using type =
      combine<TypeList<T1>,  //第一个
              typename unique<typename select<
                  TypeList<Ts...>, _different_from_t1>::type  //去掉后面的T1
                              >::type>::type;
};

template <class T>
struct make_tuple;
template <class... Ts>
struct make_tuple<TypeList<Ts...>> {
  using type = std::tuple<Ts...>;
};

template <class TList, class T>
struct exist_in;
template <class T>
struct exist_in<TypeList<>, T> {
  static inline constexpr bool value = false;
};
template <class T, class... Ts>
struct exist_in<TypeList<T, Ts...>, T> {
  static inline constexpr bool value = true;
};
template <class T, class T1, class... Ts>
struct exist_in<TypeList<T1, Ts...>, T> {
  static inline constexpr bool value = exist_in<TypeList<Ts...>, T>::value;
};

}  // namespace tlop

namespace detail {

template <class T, size_t Idx_new>
struct idx_seq_push_front;
template <size_t... Idx, size_t Idx_new>
struct idx_seq_push_front<std::index_sequence<Idx...>, Idx_new> {
  using type = std::index_sequence<Idx_new, Idx...>;
};
//找出非空类型的位置
template <class T, size_t Current>
struct get_ct_real_idx_seq;
template <size_t Current>
struct get_ct_real_idx_seq<TypeList<>, Current> {
  using type = std::index_sequence<>;
};
template <class T1, class... Ts, size_t Current>
struct get_ct_real_idx_seq<TypeList<T1, Ts...>, Current> {
  using _next_result = get_ct_real_idx_seq<TypeList<Ts...>, Current + 1>::type;
  using type = std::conditional_t<
      !std::is_empty_v<T1>,
      typename idx_seq_push_front<_next_result, Current>::type, _next_result>;
};

//找出空类型的位置
template <class T, size_t Current>
struct get_ct_empty_idx_seq;
template <size_t Current>
struct get_ct_empty_idx_seq<TypeList<>, Current> {
  using type = std::index_sequence<>;
};
template <class T1, class... Ts, size_t Current>
struct get_ct_empty_idx_seq<TypeList<T1, Ts...>, Current> {
  using _next_result = get_ct_empty_idx_seq<TypeList<Ts...>, Current + 1>::type;
  using type = std::conditional_t<
      std::is_empty_v<T1>,
      typename idx_seq_push_front<_next_result, Current>::type, _next_result>;
};
template <class TL, size_t Idx>
struct ct_get_real_index;
template <class T, class... Ts>
struct ct_get_real_index<TypeList<T, Ts...>, 0> {
  static inline constexpr size_t value = 0;
};
template <class T, class... Ts, size_t Idx>
struct ct_get_real_index<TypeList<T, Ts...>, Idx> {
  static inline constexpr size_t value =
      ct_get_real_index<TypeList<Ts...>, Idx - 1>::value + !std::is_empty_v<T>
          ? 1
          : 0;
};
// template <class Tprev, class Tnext, size_t Idx>
// struct _make_ct_idx_seq_impl;
// template <class... Ts, size_t Idx>
// struct _make_ct_idx_seq_impl<TypeList<Ts...>, TypeList<>, Idx> {
//   using type = std::index_sequence<>;
// };
// template <class Tprev, class T1, class... Ts, size_t Idx>
// struct _make_ct_idx_seq_impl<Tprev, TypeList<T1, Ts...>, Idx> {
//   using _next_prev = tlop::push_back<Tprev, T1>::type;
//   using _next_next = TypeList<Ts...>;
//   using _next_result =
//       _make_ct_idx_seq_impl<_next_prev, _next_next, Idx + 1>::type;
//   using type = std::conditional_t<
//       tlop::exist_in<Tprev, T1>::value, _next_result,  //
//       T1存在于Tprev中，忽略 typename idx_seq_push_front<_next_result,
//                                   Idx>::type  // T1不存在于Tprev中，记录Idx
//
//       >;
// };
// template <class T>
// struct make_ct_idx_seq;
// template <class... Ts>
// struct make_ct_idx_seq<TypeList<Ts...>> {
//   using type = _make_ct_idx_seq_impl<TypeList<>, TypeList<Ts...>, 0>::type;
// };

template <class T>
struct is_not_empty {
  static inline constexpr bool value = !std::is_empty_v<T>;
};
template <class... Ts>
struct compressed_tuple_helper {
  using empty_list =
      typename tlop::select<TypeList<Ts...>, std::is_empty>::type;
  using real_list = typename tlop::select<TypeList<Ts...>, is_not_empty>::type;
  using base = tlop::make_tuple<real_list>::type;
};
struct init_by_tuple {};
}  // namespace detail
template <class... Ts>
class CompressedTuple;
template <>
class CompressedTuple<> : private std::tuple<> {
  using base = std::tuple<>;

 public:
  base& get_tuple() & { return *this; }
  const base& get_tuple() const& { return *this; }
  base&& get_tuple() && { return std::move(*this); }
};
template <class... Ts>
class CompressedTuple : private detail::compressed_tuple_helper<Ts...>::base {
  using helper = detail::compressed_tuple_helper<Ts...>;
  using base = helper::base;

 public:
  template <class T, size_t... Idx_real, size_t... Idx_empty>
  constexpr CompressedTuple(detail::init_by_tuple, T&& tp,
                            std::index_sequence<Idx_real...>,
                            std::index_sequence<Idx_empty...>)
      : base(std::get<Idx_real>(tp)...) {
    ((new (std::bit_cast<
           typename tlop::get_nth<TypeList<Ts...>, Idx_empty>::type*>(this))
          typename tlop::get_nth<TypeList<Ts...>, Idx_empty>::type(
              std::get<Idx_empty>(tp))),
     ...);
  }

 public:
  constexpr CompressedTuple() : base() {}
  constexpr CompressedTuple(const Ts&... args)
      : CompressedTuple(
            detail::init_by_tuple{}, std::forward_as_tuple(args...),
            typename detail::get_ct_real_idx_seq<TypeList<Ts...>, 0>::type{},
            typename detail::get_ct_empty_idx_seq<TypeList<Ts...>, 0>::type{}) {
  }
  base& get_tuple() & { return *this; }
  const base& get_tuple() const& { return *this; }
  base&& get_tuple() && { return std::move(*this); }
};

template <size_t Idx, class... Ts>
const auto& cget(const CompressedTuple<Ts...>& ct) {
  using target_t = typename tlop::get_nth<TypeList<Ts...>, Idx>::type;
  if constexpr (std::is_empty_v<target_t>) {
    return *std::bit_cast<target_t*>(&ct);
  } else {
    return std::get<detail::ct_get_real_index<TypeList<Ts...>, Idx>::value>(
        ct.get_tuple());
  }
}
template <size_t Idx, class... Ts>
auto& cget(CompressedTuple<Ts...>& ct) {
  using target_t = typename tlop::get_nth<TypeList<Ts...>, Idx>::type;
  if constexpr (std::is_empty_v<target_t>) {
    return *std::bit_cast<target_t*>(&ct);
  } else {
    return std::get<detail::ct_get_real_index<TypeList<Ts...>, Idx>::value>(
        ct.get_tuple());
  }
}
template <size_t Idx, class... Ts>
auto&& cget(CompressedTuple<Ts...>&& ct) {
  using target_t = typename tlop::get_nth<TypeList<Ts...>, Idx>::type;
  if constexpr (std::is_empty_v<target_t>) {
    return *std::bit_cast<target_t*>(&ct);
  } else {
    return std::get<detail::ct_get_real_index<TypeList<Ts...>, Idx>::value>(
        ct.get_tuple());
  }
}

}  // namespace ctuple