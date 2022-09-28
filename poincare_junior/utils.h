#ifndef POINCARE_UTILS_H
#define POINCARE_UTILS_H

#include <stddef.h>
#include <stdint.h>

namespace Poincare {

namespace Utils {

#warning Ensure that we don't emit multiple functions

template <typename T, typename ...Args> using ConstActionByPointer = const T * (T::*)(Args...) const;
template <typename T, typename ...Args> using ConstActionByObject = const T (T::*)(Args...) const;

/* This helper wraps a action on const objects into a action on non-const objects.
 * It should be completely eliminated by the compiler after the type checking. */
template <typename T, typename ...Args>
constexpr inline T * DeconstifyPtr(ConstActionByPointer<T, Args...> action, T * object, Args... args) {
  return const_cast<T *>((object->*action)(args...));
}

template <typename T, typename ...Args>
constexpr inline T DeconstifyObj(ConstActionByObject<T, Args...> action, T * object, Args... args) {
  return T((object->*action)(args...));
}

typedef void (*Swap) (int i, int j, void * context, int numberOfElements);
typedef bool (*Compare) (int i, int j, void * context, int numberOfElements);

size_t AlignedSize(size_t realSize, size_t alignment);
size_t Gcd(size_t a, size_t b);
bool Rotate(uint8_t * dst, uint8_t * src, size_t len);
void Sort(Swap swap, Compare compare, void * context, int numberOfElements);
int ExtremumIndex(Compare compare, void * context, int numberOfElements, bool minimum);
bool FloatIsGreater(float xI, float xJ, bool nanIsGreatest);

}

}

#endif
