#include "random.h"

#include <ion.h>
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
    if (descendant->isRandInt() && Dimension::DeepCheck(descendant)) {
      /* RandInt dimension may have not been checked at this point and
       * it may contain a list, we need its length. */
      // Keep a size 1 to increment currentSeed anyway.
      size = std::max(Dimension::ListLength(descendant), 1);
    }
    if (descendant->isRandIntNoRep() &&
        currentSeed + size > Context::k_maxNumberOfVariables) {
      /* RandIntNoRep should not be seeded if its seed won't fit in the Context
       */
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
#define LCG_A 1664525U
#define LCG_C 1013904223U

static uint32_t Lcg(uint32_t* state) {
  *state = LCG_A * (*state) + LCG_C;
  return *state;
}

static uint32_t RandIntNoRepInRangeOfIndex(uint32_t range, uint32_t seed,
                                           uint8_t index) {
  uint32_t selected[1 << 8];
  uint32_t sequence_index = 0;
  uint32_t state = seed;

  while (sequence_index <= index) {
    // next is the next value in the sequence, if not already selected
    uint32_t next = Lcg(&state) % range;
    bool already_selected = false;
    for (uint32_t i = 0; i < sequence_index; i++) {
      if (selected[i] == next) {
        already_selected = true;
        break;
      }
    }
    if (!already_selected) {
      selected[sequence_index++] = next;
      if (sequence_index > index) {
        return next;
      }
    }
  }
  OMG::unreachable();
}

/* Approximate a RandInNoRep tree, this function ignores
 * [approxCtx->m_listElement], instead it uses the parameter [listElement].
 * This hack allows calling this function without needing to make a
 * local copy of the context to changed [m_listElement], because we usually have
 * a [const Context*] */
double PrivateApproximateRandIntNoRep(const Tree* randInNoRep,
                                      const Approximation::Context* approxCtx,
                                      uint8_t seed) {
  assert(randInNoRep->isRandIntNoRep());
  uint8_t lcgSeedIndex = seed;
  if (lcgSeedIndex <= 0 ||
      lcgSeedIndex > Random::Context::k_maxNumberOfVariables) {
    // Cannot access a single element for unseeded RandIntNoRep.
    return NAN;
  }
  Random::Context::VariableType* lcgSeed =
      &approxCtx->m_randomContext.m_list[lcgSeedIndex - 1];
  if (std::isnan(*lcgSeed)) {
    *lcgSeed = Ion::random();
    /* NOTE Storing a uint32_t without loss of precision is only possible in a
     * double not in a float */
    static_assert(std::is_same<double, Random::Context::VariableType>::value);
  }
  double min = Approximation::Private::PrivateTo<double>(randInNoRep->child(0),
                                                         approxCtx);
  double max = Approximation::Private::PrivateTo<double>(randInNoRep->child(1),
                                                         approxCtx);
  assert(min == std::round(min) && max == std::round(max));

  uint32_t range = static_cast<uint32_t>(max - min + 1);
#if ASSERTIONS
  double length = Approximation::Private::PrivateTo<double>(
      randInNoRep->child(2), approxCtx);
  assert(approxCtx->m_listElement < length && length <= range);
#endif
  return min + RandIntNoRepInRangeOfIndex(range,
                                          static_cast<uint32_t>(*lcgSeed),
                                          approxCtx->m_listElement);
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
    if (!validSeed) {
      return NAN;
    }
    return PrivateApproximateRandIntNoRep(randomTree, approxCtx, seed);
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
}  // namespace Poincare::Internal
