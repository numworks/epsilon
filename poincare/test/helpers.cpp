#include <poincare/helpers.h>

#include "helper.h"

static inline void assert_gcd_is(size_t a, size_t b, size_t g) {
  quiz_assert(Poincare::Helpers::Gcd(a, b) == g);
}

QUIZ_CASE(poincare_helpers_gcd) {
  assert_gcd_is(1, 1, 1);
  assert_gcd_is(2, 3, 1);
  assert_gcd_is(7, 5, 1);
  assert_gcd_is(3, 9, 3);
}

QUIZ_CASE(poincare_helpers_insert_simple_swap) {
  constexpr size_t bufSize = 3;
  uint32_t buf[bufSize];
  for (size_t i = 0; i < bufSize; i++) {
    buf[i] = (uint32_t)i;
  }

  Poincare::Helpers::Rotate(&buf[0], &buf[1], 1);
  quiz_assert(buf[0] == 1);
  quiz_assert(buf[1] == 0);
  quiz_assert(buf[2] == 2);

  Poincare::Helpers::Rotate(&buf[2], &buf[0], 1);
  quiz_assert(buf[0] == 0);
  quiz_assert(buf[1] == 1);
  quiz_assert(buf[2] == 2);
}

static inline void test_rotate(uint32_t buf[], size_t bufSize, size_t dstIndex,
                               size_t srcIndex, size_t len) {
  quiz_assert(len == 0 || (srcIndex + len - 1 < bufSize));

  Poincare::Helpers::Rotate(&buf[dstIndex], &buf[srcIndex], len);

  if (len == 0 || srcIndex == dstIndex ||
      (dstIndex > srcIndex && dstIndex < srcIndex + len)) {
    for (size_t i = 0; i < bufSize; i++) {
      quiz_assert(buf[i] == i);
    }
  } else if (dstIndex < srcIndex) {
    /* Typical case:
     *
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
     *         ^¨¨¨¨¨¨¨^---^
     *        Dst       Src       Len = 1
     *
     * After rotation:
     * | 0 | 1 | 4 | 2 | 3 | 5 | 6 |
     *          --- ¨¨¨¨¨¨¨
     * */
    for (size_t i = 0; i < dstIndex; i++) {
      quiz_assert(buf[i] == i);
    }
    for (size_t i = dstIndex; i < dstIndex + len; i++) {
      quiz_assert(buf[i] == srcIndex + (i - dstIndex));
    }
    for (size_t i = dstIndex + len; i < srcIndex + len; i++) {
      quiz_assert(buf[i] == dstIndex + (i - (dstIndex + len)));
    }
    for (size_t i = srcIndex + len; i < bufSize; i++) {
      quiz_assert(buf[i] == i);
    }
  } else {
    /* Typical case:
     *
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
     *         ^-------^¨¨¨^
     *         Src        Dst       Len = 2
     *
     * After rotation:
     * | 0 | 1 | 4 | 2 | 3 | 5 | 6 |
     *          ¨¨¨ -------
     * */
    for (size_t i = 0; i < srcIndex; i++) {
      quiz_assert(buf[i] == i);
    }
    size_t srcDataNewPosition = srcIndex + dstIndex - (srcIndex + len);
    for (size_t i = srcIndex; i < srcDataNewPosition; i++) {
      quiz_assert(buf[i] == i + len);
    }
    for (size_t i = srcDataNewPosition; i < dstIndex; i++) {
      quiz_assert(buf[i] == srcIndex + i - srcDataNewPosition);
    }
    for (size_t i = dstIndex; i < bufSize; i++) {
      quiz_assert(buf[i] == i);
    }
  }
}

QUIZ_CASE(poincare_helpers_rotate) {
  // We test all dst, src and len combinations for a buffer of size 100
  constexpr size_t bufSize = 100;
  uint32_t buf[bufSize];
  for (size_t dst = 0; dst < bufSize; dst++) {
    for (size_t src = 0; src < bufSize; src++) {
      for (size_t len = 0; len < bufSize - src + 1; len++) {
        // Reset the start buffer
        for (size_t i = 0; i < bufSize; i++) {
          buf[i] = (uint32_t)i;
        }
        // Launch the test
        test_rotate(buf, bufSize, dst, src, len);
      }
    }
  }
}
