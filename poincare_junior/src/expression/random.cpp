#include "random.h"

#include <math.h>
#include <poincare/random.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/n_ary.h>

#include "float.h"
#include "k_tree.h"

namespace PoincareJ {

Random::Context::Context() {
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    m_list[i] = NAN;
  }
}

uint8_t Random::SeedTreeNodes(Tree* tree) {
  uint8_t currentSeed = 0;
  Tree* u = tree;
  int descendants = 1;
  while (descendants > 0) {
    descendants--;
    if (u->isRandomNode()) {
      // RandIntNoRep needs to reserve seed for each of its elements.
      int size = u->isRandIntNoRep() ? Dimension::GetListLength(u) : 1;
      assert(static_cast<int>(currentSeed) + size < UINT8_MAX);
      if (currentSeed + size > Context::k_maxNumberOfVariables) {
        assert(GetSeed(u) == 0);
        return currentSeed;
      }
      SetSeed(u, currentSeed + 1);
      currentSeed += size;
    }
    descendants += u->numberOfChildren();
    u = u->nextNode();
  }
  return currentSeed;
}

template <typename T>
T Random::Approximate(const Tree* randomTree, Context* context,
                      int listElement) {
  uint8_t seed = Random::GetSeed(randomTree);
  if (randomTree->isRandIntNoRep() && seed > 0) {
    seed += listElement;
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
  T result = PrivateApproximate<T>(randomTree, context, listElement);
  if (seed > 0) {
    context->m_list[seed - 1] = result;
  }
  return result;
}

template <typename T>
T Random::PrivateApproximate(const Tree* randomTree, Context* context,
                             int listElement) {
  if (randomTree->isRandom()) {
    return Poincare::Random::random<T>();
  } else if (randomTree->isRandInt()) {
    return RandomInt<T>(Approximation::To<T>(randomTree->child(0)),
                        Approximation::To<T>(randomTree->child(1)));
  }
  assert(randomTree->isRandIntNoRep());
  uint8_t seed = Random::GetSeed(randomTree);
  if (seed == 0) {
    // Cannot access a single element for unseeded RandIntNoRep.
    return NAN;
  }
  T a = Approximation::To<T>(randomTree->child(0));
  T b = Approximation::To<T>(randomTree->child(1));
  // Shorten the RandInt window since numbers have already been generated.
  T result = RandomInt<T>(a, b - listElement);
  // Check all previously generated numbers, ordered by increasing value.
  T check = b + 1.0;
  T previousCheck = a - 1.0;
  for (int j = 0; j < listElement; j++) {
    // Find the next check : smallest value bigger than previousCheck
    for (int k = 0; k < listElement; k++) {
      T value = Approximate<T>(randomTree, context, k);
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
  if (std::isnan(a) || std::isnan(b) || a > b || std::round(a) != a ||
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

template float Random::Approximate<float>(const Tree*, Context*, int);
template double Random::Approximate<double>(const Tree*, Context*, int);
template float Random::PrivateApproximate<float>(const Tree*, Context*, int);
template double Random::PrivateApproximate<double>(const Tree*, Context*, int);
template float Random::RandomInt<float>(float, float);
template double Random::RandomInt<double>(double, double);

}  // namespace PoincareJ
