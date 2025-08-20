#include "random.h"

#include <poincare/random.h>
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

uint8_t Random::GetMaxSeed(const Tree* e) {
  uint8_t maxSeed = 0;
  for (const Tree* child : e->selfAndDescendants()) {
    if (child->isRandomized()) {
      maxSeed = std::max(maxSeed, GetSeed(child));
    }
  }
  return maxSeed;
}

uint8_t Random::SeedRandomNodes(Tree* e, uint8_t maxSeed) {
  int currentSeed = maxSeed;
  for (Tree* descendant : e->selfAndDescendants()) {
    if (!descendant->isRandomized()) {
      continue;
    }
    if (GetSeed(descendant) != 0) {
      assert(GetSeed(descendant) <= maxSeed);
      continue;
    }
    /* RandIntNoRep and RandInt of lists needs to reserve seed for each of
     * its elements. */
    int size = 1;
    if ((descendant->isRandIntNoRep() || descendant->isRandInt()) &&
        Dimension::DeepCheck(descendant)) {
      /* RandIntNoRep dimension may have not been checked at this point and
       * we need its length. The dimension check for RandIntNoRep is
       * straighforward and can be done at this step. */
      // Keep a size 1 to increment currentSeed anyway.
      size = std::max(Dimension::ListLength(descendant), 1);
    }
    if (descendant->isRandIntNoRep() &&
        currentSeed + size > Context::k_maxNumberOfVariables) {
      /* RandIntNoRep should not be seeded if all its content doesn't fit in the
       * Context */
      size = 0;
      // Keep seed 0
    } else {
      /* Continue seeding RandInt and Random node even past the
       * [k_maxNumberOfVariables] limit as they will behave like seed 0 */
      SetSeed(descendant, currentSeed + 1);
    }
    currentSeed += size;
    if (currentSeed >= k_maxNumberOfSeeds) {
      // No more seeds available
      break;
    }
  }
  return currentSeed;
}

/* Approximate a RandInNoRep tree, this function ignores
 * [approxCtx->m_listElement], instead it uses the parameter [listElement].
 * This hack allows calling this function without needing to make a
 * local copy of the context to changed [m_listElement], because we usually have
 * a [const Context*] */
template <typename T>
T PrivateApproximateRandIntNoRep(const Tree* randInNoRep,
                                 const Approximation::Context* approxCtx,
                                 const int listElement, const T* min = nullptr,
                                 const T* max = nullptr) {
  assert(randInNoRep->isRandIntNoRep());
  assert(listElement >= 0);
  uint8_t seed = Random::GetSeed(randInNoRep);
  if (seed == 0) {
    // Cannot access a single element for unseeded RandIntNoRep.
    return NAN;
  }
  assert(listElement + seed <= Random::Context::k_maxNumberOfVariables);
  seed += listElement;
  Random::Context::VariableType* cachedValue =
      &approxCtx->m_randomContext.m_list[seed - 1];
  if (!std::isnan(*cachedValue)) {
    return *cachedValue;
  }
  using Approximation::Private::PrivateTo;
  T a = min ? *min : PrivateTo<T>(randInNoRep->child(0), approxCtx);
  T b = max ? *max : PrivateTo<T>(randInNoRep->child(1), approxCtx);
  assert(!min || *min == PrivateTo<T>(randInNoRep->child(0), approxCtx));
  assert(!max || *max == PrivateTo<T>(randInNoRep->child(1), approxCtx));

  // Shorten the RandInt window since numbers have already been generated.
  T result = Random::RandomInt<T>(a, b - listElement);
  // Check all previously generated numbers, ordered by increasing value.
  T check = b + 1.0;
  T previousCheck = a - 1.0;
  for (int j = 0; j < listElement; j++) {
    // Find the next check : smallest value bigger than previousCheck
    for (int k = 0; k < listElement; k++) {
      T value =
          PrivateApproximateRandIntNoRep<T>(randInNoRep, approxCtx, k, &a, &b);
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
    } else {
      // Result is strictly smaller than all remaining values to check.
      break;
    }
    previousCheck = check;
    check = b + 1.0;
  }
  *cachedValue = result;
  return *cachedValue;
}

template <typename T>
T Approximation::Private::ApproximateRandom(const Tree* randomTree,
                                            const Context* approxCtx) {
  uint8_t seed = Random::GetSeed(randomTree);
  bool validSeed = seed > 0 && seed <= Random::Context::k_maxNumberOfVariables;
  if (validSeed &&
      (!approxCtx || !approxCtx->m_randomContext.m_isInitialized)) {
    return NAN;
  }
  if (randomTree->isRandIntNoRep()) {
    return PrivateApproximateRandIntNoRep<T>(randomTree, approxCtx,
                                             approxCtx->m_listElement);
  }
  if (!validSeed) {
    return ApproximateRandomHelper<T>(randomTree, approxCtx);
  }
  Random::Context::VariableType* cachedValue =
      &approxCtx->m_randomContext.m_list[seed - 1];
  if (std::isnan(*cachedValue)) {
    // Context is needed with RandIntNoRep
    *cachedValue = ApproximateRandomHelper<T>(randomTree, approxCtx);
  }
  return *cachedValue;
}

template <typename T>
T ApproximateRandomHelper(const Tree* randomTree,
                          const Approximation::Context* ctx) {
  using Approximation::Private::PrivateTo;
  if (randomTree->isRandom()) {
    return Poincare::random<T>();
  }
  assert(randomTree->isRandInt());
  return Random::RandomInt<T>(PrivateTo<T>(randomTree->child(0), ctx),
                              PrivateTo<T>(randomTree->child(1), ctx));
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

template float Approximation::Private::ApproximateRandom<float>(const Tree*,
                                                                const Context*);
template double Approximation::Private::ApproximateRandom<double>(
    const Tree*, const Context*);
template float ApproximateRandomHelper<float>(const Tree*,
                                              const Approximation::Context*);
template double ApproximateRandomHelper<double>(const Tree*,
                                                const Approximation::Context*);
template float Random::RandomInt<float>(float, float);
template double Random::RandomInt<double>(double, double);
template float PrivateApproximateRandIntNoRep<float>(
    const Tree*, const Approximation::Context*, const int, const float*,
    const float*);
template double PrivateApproximateRandIntNoRep<double>(
    const Tree*, const Approximation::Context*, const int, const double*,
    const double*);

}  // namespace Poincare::Internal
