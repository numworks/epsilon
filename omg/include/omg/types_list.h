#pragma once

#include <stddef.h>

namespace OMG {

template <typename... Types>
struct TypesList;

template <>
struct TypesList<> {
  template <typename U>
  using Append = TypesList<U>;

  using Reverse = TypesList<>;

  template <typename F, typename... Args>
  static void ForEach(Args...) {}
};

template <typename T, typename... Types>
struct TypesList<T, Types...> {
  template <typename U>
  using Append = TypesList<T, Types..., U>;

  using Reverse = typename TypesList<Types...>::Reverse::template Append<T>;

  /* The first template argument should be trivially constructible, and contain
   * a templated operator(). */
  template <typename F, typename... Args>
  static void ForEach(Args... args) {
    F{}.template operator()<T>(args...);
    TypesList<Types...>::template ForEach<F>(args...);
  }
};

}  // namespace OMG
