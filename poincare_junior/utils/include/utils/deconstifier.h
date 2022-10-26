#ifndef UTILS_DECONSTIFIER_H
#define UTILS_DECONSTIFIER_H

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

}

#endif
