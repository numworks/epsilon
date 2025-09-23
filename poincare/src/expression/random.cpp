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
    /* NOTE:
     * Random reserves one seed, to store the random value.
     * RandInt of lists needs to reserve seed for each of its elements.
     * RandIntNoRep only reserves 1 seed, used to store the LCG seed. */
    int size = 1;
    if (descendant->isRandInt() && Dimension::DeepCheck(descendant)) {
      /* RandInt dimension may have not been checked at this point and
       * it may contain a list, we need its length. */
      // Keep a size 1 to increment currentSeed anyway.
      size = std::max(Dimension::ListLength(descendant), 1);
    }
    if (descendant->isRandIntNoRep() &&
        currentSeed + size > Context::k_maxNumberOfVariables) {
      // RandIntNoRep should not be seeded if its seed won't fit in the Context
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

/* A Linear Congruential Generator (lcg) is a seeded pseudo-random generator
 * that yield a fixed sequence of uint32_t starting at a given seed.
 * The formula is: X(n+1) = A * X(n) + C % M
 * Since we want M=2^32, we choose A and C accordingly, see
 * https://en.wikipedia.org/wiki/Linear_congruential_generator#Parameters_in_common_use
 * With these parameters, the LCG has a period of M before repeating.
 * From a given X(0) (the seed) the sequence X(n) will contain the set of all
 * uint32_t before starting to repeat at X(2^32) */
static uint32_t Lcg(uint32_t* state) {
  constexpr uint32_t A = 1664525U;
  constexpr uint32_t C = 1013904223U;
  // NOTE: %M operation is guaranteed by the uint32_t overflow
  *state = A * (*state) + C;
  return *state;
}

/* [RandIntNoRepInRangeOfIndex] uses the LCG sequence of a given [seed] to
 * returns an [uint32_t] below [range] that is unique depending on [index]. */
static uint32_t RandIntNoRepInRangeOfIndex(uint32_t range, uint32_t seed,
                                           uint8_t index) {
  /* The uniqueness of the return value is guaranteed by this uint32_t[256]
   * array. It will store the previously returned values.
   * i.e.: sequence[i] == RandIntNoRepInRangeOfIndex(range,seed,i)
   * This approach allows for computing sequentially unique element of a
   * sequence without any external state, at the cost of recomputing all the
   * previous term of the sequence each time.
   * Main downside: Computing the last new value (when range == index) takes on
   * average range [Lcg] calls */
  uint32_t sequence[NAry::k_maxNumberOfChildren + 1];
  uint32_t maxComputedIndex = 0;
  uint32_t lcgState = seed;

  assert(index < range);

  while (maxComputedIndex <= index) {
    /* [Lcg] computes and returns the next state i.e. the next value in the LCG
     * sequence
     * [nextValueInRange] is the next candidate for the randintnorep value */
    uint32_t nextValueInRange = Lcg(&lcgState) % range;
    bool alreadySelected = false;
    // Check if [next] is already in the [sequence]
    for (uint32_t i = 0; i < maxComputedIndex; i++) {
      if (sequence[i] == nextValueInRange) {
        alreadySelected = true;
        break;
      }
    }
    if (!alreadySelected) {
      sequence[maxComputedIndex++] = nextValueInRange;
      if (maxComputedIndex > index) {
        return nextValueInRange;
      }
    }
  }
  OMG::unreachable();
}

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
  assert(0 <= approxCtx->m_listElement && approxCtx->m_listElement < length &&
         length <= range);
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
