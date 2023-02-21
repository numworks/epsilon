#ifndef POINCARE_HELPERS_H
#define POINCARE_HELPERS_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cmath>

namespace Poincare {

class Helpers {
 public:
  typedef void (*Swap)(int i, int j, void* context, int numberOfElements);
  typedef bool (*Compare)(int i, int j, void* context, int numberOfElements);

  static size_t AlignedSize(size_t realSize, size_t alignment);
  static size_t Gcd(size_t a, size_t b);

  static bool Rotate(uint32_t* dst, uint32_t* src, size_t len);
  static void Sort(Swap swap, Compare compare, void* context,
                   int numberOfElements);
  static bool FloatIsGreater(float xI, float xJ, bool nanIsGreatest);

  /* This is a default *Compare function. Context first three elements must be:
   * {ListNode *, ApproximationContext *, bool * nanIsGreatest} */
  static bool ListEvaluationComparisonAtIndex(int i, int j, void* context,
                                              int numberOfElements);
  // Return true if observed and expected are approximately equal
  template <typename T>
  static bool RelativelyEqual(T observed, T expected, T relativeThreshold);

  /* FIXME : This can be replaced by std::string_view when moving to C++17 */
  constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
    return *s1 == *s2 &&
           ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
  }

  constexpr static int StringLength(const char* string) {
    int result = 0;
    while (string[result] != 0) {
      result++;
    }
    return result;
  }

  constexpr static inline bool EqualOrBothNan(double a, double b) {
    return a == b || (std::isnan(a) && std::isnan(b));
  }

  template <typename T>
  constexpr static T SquareRoot(T x) {
    return x >= 0 ? SquareRootHelper(x, x, static_cast<T>(0.))
                  : static_cast<T>(NAN);
  }

 private:
  // Helper for the compile-time square root
  template <typename T>
  constexpr static T SquareRootHelper(T x, T a, T b) {
    return a == b ? a
                  : SquareRootHelper(x, static_cast<T>(0.5) * (a + x / a), a);
  }
};

}  // namespace Poincare

#endif
