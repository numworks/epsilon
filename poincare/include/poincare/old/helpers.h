#ifndef POINCARE_HELPERS_H
#define POINCARE_HELPERS_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cmath>

namespace Poincare {

class OList;
template <typename T>
class ListComplex;

class Helpers {
 public:
  typedef void (*Swap)(int i, int j, void* context, int numberOfElements);
  typedef bool (*Compare)(int i, int j, void* context, int numberOfElements);

  template <typename T>
  struct ListSortPack {
    OList* list;
    ListComplex<T>* listComplex;
    bool scalars;
  };

  static size_t AlignedSize(size_t realSize, size_t alignment);
  static size_t Gcd(size_t a, size_t b);

  static bool Rotate(uint32_t* dst, uint32_t* src, size_t len);
  static void Sort(Swap swap, Compare compare, void* context,
                   int numberOfElements);

  template <typename T>
  static void SwapInList(int i, int j, void* context, int numberOfElements);
  template <typename T>
  static bool CompareInList(int i, int j, void* context, int numberOfElements);

  // Return true if observed and expected are approximately equal
  template <typename T>
  static bool RelativelyEqual(T observed, T expected, T relativeThreshold);

  /* FIXME : This can be replaced by std::string_view when moving to C++17 */
  constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
    return *s1 == *s2 &&
           ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
  }

  constexpr static inline bool EqualOrBothNan(double a, double b) {
    return a == b || (std::isnan(a) && std::isnan(b));
  }
};

}  // namespace Poincare

#endif
