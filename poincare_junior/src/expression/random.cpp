#include "random.h"

#include <math.h>
#include <poincare/random.h>
#include <poincare_junior/src/expression/approximation.h>
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

uint8_t Random::SeedTreeNodes(Tree* tree, uint8_t seedOffset) {
  uint8_t currentSeed = seedOffset;
  Tree* u = tree;
  int descendants = 1;
  while (descendants > 0) {
    descendants--;
    if (u->isRandomNode()) {
      assert(currentSeed < UINT8_MAX);
      currentSeed += 1;
      SetSeed(u, currentSeed);
    }
    descendants += u->numberOfChildren();
    u = u->nextNode();
  }
  return currentSeed;
}

template <typename T>
T Random::Approximate(const Tree* randomTree, Context* context) {
  uint8_t seed = Random::GetSeed(randomTree);
  if (seed > 0) {
    if (!context) {
      return NAN;
    }
    T result = context->m_list[seed - 1];
    if (!std::isnan(result)) {
      return result;
    }
  }
  T result = Approximate<T>(randomTree);
  if (seed > 0) {
    context->m_list[seed - 1] = result;
  }
  return result;
}

template <typename T>
T Random::Approximate(const Tree* randomTree) {
  switch (randomTree->type()) {
    case BlockType::RandInt:
      return RandomInt<T>(Approximation::To<T>(randomTree->child(0)),
                          Approximation::To<T>(randomTree->child(1)));
    case BlockType::Random:
      return Poincare::Random::random<T>();
    default:
      assert(randomTree->type() == BlockType::RandIntNoRep);
      // TODO: Copy or factorize
      // Poincare::RandintNoRepeatNode::templatedApproximate<T>();
      // TODO_PCJ: Handle this.
      assert(false);
      return static_cast<T>(GetSeed(randomTree));
  }
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

template float Random::Approximate<float>(const Tree*);
template double Random::Approximate<double>(const Tree*);
template float Random::Approximate<float>(const Tree*, Context*);
template double Random::Approximate<double>(const Tree*, Context*);
template float Random::RandomInt<float>(float, float);
template double Random::RandomInt<double>(double, double);

}  // namespace PoincareJ
