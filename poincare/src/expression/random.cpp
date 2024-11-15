#include "random.h"

#include <poincare/numeric/random.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include <cmath>

#include "approximation.h"
#include "dimension.h"
#include "float.h"
#include "k_tree.h"

namespace Poincare::Internal {

Random::Context::Context(bool isInitialized) : m_isInitialized(isInitialized) {
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
T Approximation::ApproximateRandom(const Tree* randomTree,
                                   const Context* approxCtx) {
  uint8_t seed = Random::GetSeed(randomTree);
  if (randomTree->isRandIntNoRep()) {
    assert(approxCtx->m_listElement >= 0);
    if (seed > 0) {
      seed += approxCtx->m_listElement;
    }
  }
  assert(seed <= Random::Context::k_maxNumberOfVariables);
  if (seed > 0) {
    if (!approxCtx || !approxCtx->m_randomContext.m_isInitialized) {
      return NAN;
    }
    T result = approxCtx->m_randomContext.m_list[seed - 1];
    if (!std::isnan(result)) {
      return result;
    }
  }
  // Context is needed with RandIntNoRep
  T result = ApproximateRandomHelper<T>(randomTree, approxCtx);
  if (seed > 0) {
    approxCtx->m_randomContext.m_list[seed - 1] = result;
  }
  return result;
}

template <typename T>
T Approximation::ApproximateRandomHelper(const Tree* randomTree,
                                         const Context* ctx) {
  if (randomTree->isRandom()) {
    return Poincare::random<T>();
  } else if (randomTree->isRandInt()) {
    return Random::RandomInt<T>(To<T>(randomTree->child(0), ctx),
                                To<T>(randomTree->child(1), ctx));
  }
  assert(randomTree->isRandIntNoRep());
  uint8_t seed = Random::GetSeed(randomTree);
  if (seed == 0) {
    // Cannot access a single element for unseeded RandIntNoRep.
    return NAN;
  }
  T a = To<T>(randomTree->child(0), ctx);
  T b = To<T>(randomTree->child(1), ctx);
  // Shorten the RandInt window since numbers have already been generated.
  T result = Random::RandomInt<T>(a, b - ctx->m_listElement);
  // Check all previously generated numbers, ordered by increasing value.
  T check = b + 1.0;
  T previousCheck = a - 1.0;
  for (int j = 0; j < ctx->m_listElement; j++) {
    // Find the next check : smallest value bigger than previousCheck
    Context tempCtx(*ctx);
    for (int k = 0; k < ctx->m_listElement; k++) {
      tempCtx.m_listElement = k;
      T value = ApproximateRandom<T>(randomTree, &tempCtx);
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
  while ((rand = Poincare::random<T>()) == static_cast<T>(1.0)) {
  }
  return std::floor(rand * range + a);
}

template float Approximation::ApproximateRandom<float>(const Tree*,
                                                       const Context*);
template double Approximation::ApproximateRandom<double>(const Tree*,
                                                         const Context*);
template float Approximation::ApproximateRandomHelper<float>(const Tree*,
                                                             const Context*);
template double Approximation::ApproximateRandomHelper<double>(const Tree*,
                                                               const Context*);
template float Random::RandomInt<float>(float, float);
template double Random::RandomInt<double>(double, double);

}  // namespace Poincare::Internal
