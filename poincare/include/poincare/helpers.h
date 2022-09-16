#ifndef POINCARE_HELPERS_H
#define POINCARE_HELPERS_H

#include <stddef.h>
#include <stdint.h>

namespace Poincare {

namespace Helpers {

typedef void (*Swap) (int i, int j, void * context, int numberOfElements);
typedef bool (*Compare) (int i, int j, void * context, int numberOfElements);

size_t AlignedSize(size_t realSize, size_t alignment);
size_t Gcd(size_t a, size_t b);
bool Rotate(uint32_t * dst, uint32_t * src, size_t len);
void Sort(Swap swap, Compare compare, void * context, int numberOfElements);
bool FloatIsGreater(float xI, float xJ, bool nanIsGreatest);

/* This is a default *Compare function. Context first three elements must be:
 * {ListNode *, ApproximationContext *, bool * nanIsGreatest} */
bool ListEvaluationComparisonAtIndex(int i, int j, void * context, int numberOfElements);
// Return true if observed and expected are approximately equal
template <typename T>
bool RelativelyEqual(T observed, T expected, T relativeThreshold);
}

}

#endif
