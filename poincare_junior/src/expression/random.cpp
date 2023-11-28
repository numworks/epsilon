#include "random.h"

#include <math.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/n_ary.h>

#include "float.h"
#include "k_tree.h"

namespace PoincareJ {

Random::Context::Context()
    : m_list(SharedEditionPool->push<BlockType::List>(0)) {}

Random::Context::~Context() { m_list->removeTree(); }

bool Random::SeedTreeNodes(Tree* tree) {
  uint8_t currentSeed = 0;
  for (Tree* u : tree->selfAndDescendants()) {
    if (u->isRandomNode()) {
      assert(currentSeed < UINT8_MAX);
      currentSeed += 1;
      SetSeed(u, currentSeed);
    }
  }
  return currentSeed > 0;
}

template <typename T>
T Random::Approximate(const Tree* randomTree, Context* context) {
  uint8_t seed = Random::GetSeed(randomTree);
  Tree* approximationForSeed;
  if (seed > 0) {
    if (!context) {
      return NAN;
    }
    assert(!context->m_list.isUninitialized());
    while (seed > context->m_list->numberOfChildren()) {
      // Some seeds might have been simplified out, or reordered.
      NAry::AddChild(context->m_list, KUndef->clone());
    }
    approximationForSeed = context->m_list->child(seed - 1);
    if (!approximationForSeed->isUndefined()) {
      return Approximation::To<T>(approximationForSeed, nullptr);
    }
  }
  T result = Approximate<T>(randomTree);
  if (seed > 0) {
    assert(approximationForSeed);
    approximationForSeed->moveTreeOverTree(
        SharedEditionPool->push<FloatType<T>::type>(result));
  }
  return result;
}

template <typename T>
T Random::Approximate(const Tree* randomTree) {
  switch (randomTree->type()) {
    case BlockType::RandInt:
      // TODO: Copy or factorize Poincare::Integer::RandomInt<T>();
      return static_cast<T>(GetSeed(randomTree));
    case BlockType::Random:
      // TODO: Copy or factorize Poincare::Random::random<T>();
      return static_cast<T>(GetSeed(randomTree));
    default:
      assert(randomTree->type() == BlockType::RandIntNoRep);
      // TODO: Copy or factorize
      // Poincare::RandintNoRepeatNode::templatedApproximate<T>();
      // TODO: Handle lists in approximation.
      assert(false);
      return static_cast<T>(GetSeed(randomTree));
  }
}

template float Random::Approximate<float>(const Tree*);
template double Random::Approximate<double>(const Tree*);
template float Random::Approximate<float>(const Tree*, Context*);
template double Random::Approximate<double>(const Tree*, Context*);

}  // namespace PoincareJ
