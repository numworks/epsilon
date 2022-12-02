#ifndef UTILS_ARRAY_H
#define UTILS_ARRAY_H

#include <array>

namespace Array {

template<typename T, typename U>
using Action = U (*)(T, void *);

template<typename T, typename U, size_t N>
static std::array<U,N> MapAction(std::array<T,N> array, void * context, Action<T, U> action) {
  std::array<U,N> newArray;
  for (size_t i = 0; i < N; i++) {
    newArray[i] = action(array[i], context);
  }
  return newArray;
}

}

#endif
