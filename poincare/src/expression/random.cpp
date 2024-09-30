#include "random.h"

#include <poincare/old/random.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include <cmath>

#include "approximation.h"
#include "dimension.h"
#include "float.h"
#include "k_tree.h"

namespace Poincare::Internal {

Random::Context::Context() {
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    m_list[i] = NAN;
  }
}

uint8_t Random::SeedRandomNodes(Tree* e, uint8_t maxSeed) {
  uint8_t currentSeed = maxSeed;
  int descendants = 1;
  while (descendants > 0) {
    descendants--;
    if (e->isRandomized()) {
      if (GetSeed(e) == 0) {
        // RandIntNoRep needs to reserve seed for each of its elements.
        /* RandIntNoRep dimension may have not been checked at this point and we
         * need its length. The dimension check for RandIntNoRep is
         * straighforward and can be done at this step. */
        int size = e->isRandIntNoRep() && Dimension::DeepCheck(e)
                       ? Dimension::ListLength(e)
                       : 1;
        assert(static_cast<int>(currentSeed) + size < UINT8_MAX);
        if (currentSeed + size > Context::k_maxNumberOfVariables) {
          assert(GetSeed(e) == 0);
          return currentSeed;
        }
        SetSeed(e, currentSeed + 1);
        currentSeed += size;
      } else {
        assert(GetSeed(e) <= maxSeed);
      }
    }
    descendants += e->numberOfChildren();
    e = e->nextNode();
  }
  return currentSeed;
}

template <typename T>
T Random::Approximate(const Tree* randomTree, Context* context, int listElement,
                      const void* approxCtx) {
  uint8_t seed = Random::GetSeed(randomTree);
  if (randomTree->isRandIntNoRep()) {
    assert(listElement >= 0);
    if (seed > 0) {
      seed += listElement;
    }
  }
  assert(seed <= Context::k_maxNumberOfVariables);
  if (seed > 0) {
    if (!context) {
      return NAN;
    }
    T result = context->m_list[seed - 1];
    if (!std::isnan(result)) {
      return result;
    }
  }
  // Context is needed with RandIntNoRep
  T result = PrivateApproximate<T>(randomTree, context, listElement, approxCtx);
  if (seed > 0) {
    context->m_list[seed - 1] = result;
  }
  return result;
}

template <typename T>
T Random::PrivateApproximate(const Tree* randomTree, Context* context,
                             int listElement, const void* approxCtx) {
  const Approximation::Context* ctx =
      static_cast<const Approximation::Context*>(approxCtx);
  if (randomTree->isRandom()) {
    return Poincare::Random::random<T>();
  } else if (randomTree->isRandInt()) {
    return RandomInt<T>(Approximation::To<T>(randomTree->child(0), ctx),
                        Approximation::To<T>(randomTree->child(1), ctx));
  }
  assert(randomTree->isRandIntNoRep());
  uint8_t seed = Random::GetSeed(randomTree);
  if (seed == 0) {
    // Cannot access a single element for unseeded RandIntNoRep.
    return NAN;
  }
  T a = Approximation::To<T>(randomTree->child(0), ctx);
  T b = Approximation::To<T>(randomTree->child(1), ctx);
  // Shorten the RandInt window since numbers have already been generated.
  T result = RandomInt<T>(a, b - listElement);
  // Check all previously generated numbers, ordered by increasing value.
  T check = b + 1.0;
  T previousCheck = a - 1.0;
  for (int j = 0; j < listElement; j++) {
    // Find the next check : smallest value bigger than previousCheck
    for (int k = 0; k < listElement; k++) {
      T value = Approximate<T>(randomTree, context, k, approxCtx);
      if (value > previousCheck && value < check) {
        check = value;
      }
    }
    /* With each checked values, map result to values not yet generated.
     * For example, a is 1 and b is 6. 1, 6 and 3 have been generated already.
     * Result can be 1/2/3. First checked value is 1. Result can now be 2/3/4.
     * Next checked value is 3, result can now be 2/4/5. Final checked value is
     * 6, so result stays 2/4/5. The possible value have not been generated
     * yet.*/
    if (result >= check) {
      result += 1.0;
    }
    previousCheck = check;
    check = b + 1.0;
  }
  return result;
}

/* We could adapt Poincare::Integer::RandomInt<T>() here instead, but in
 * practice this method is called on approximation only. Children may have
 * already approximated as well.
 * As a result, this simpler implementation is considered enough for now.
 * Output distribution and floating point precision may be studied if such a
 * result where to be displayed exactly. */
template <typename T>
T Random::RandomInt(T a, T b) {
  if (!std::isfinite(a) || !std::isfinite(b) || a > b || std::round(a) != a ||
      std::round(b) != b) {
    return NAN;
  }
  if (a == b) {
    return a;
  }
  T range = 1 + b - a;
  // Ugly way to avoid the rare case where rand is exactly 1.
  T rand;
  while ((rand = Poincare::Random::random<T>()) == static_cast<T>(1.0)) {
  }
  return std::floor(rand * range + a);
}

template float Random::Approximate<float>(const Tree*, Context*, int,
                                          const void*);
template double Random::Approximate<double>(const Tree*, Context*, int,
                                            const void*);
template float Random::PrivateApproximate<float>(const Tree*, Context*, int,
                                                 const void*);
template double Random::PrivateApproximate<double>(const Tree*, Context*, int,
                                                   const void*);
template float Random::RandomInt<float>(float, float);
template double Random::RandomInt<double>(double, double);

}  // namespace Poincare::Internal
