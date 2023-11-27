#include "random.h"

#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include "float.h"
#include "k_tree.h"

namespace PoincareJ {

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
bool Random::ApproximateTreeNodes(Tree* tree) {
  bool result = false;
  // Store approximation for each seeds at the end of the pool.
  EditionReference approximations = SharedEditionPool->push<BlockType::List>(0);
  for (Tree* u : tree->selfAndDescendants()) {
    if (u->isRandomNode()) {
      uint8_t seed = GetSeed(u);
      assert(seed > 0);
      while (seed >= approximations->numberOfChildren()) {
        // Some seeds might have been simplified out, or reordered.
        NAry::AddChild(approximations, KUndef->clone());
      }
      Tree* approximationForSeed = approximations->child(seed);
      if (approximationForSeed->isUndefined()) {
        // First time this seed is encountered, compute once its approximation.
        approximationForSeed->moveTreeOverTree(
            SharedEditionPool->push<FloatType<T>::type>(
                ApproximateIgnoringSeed<T>(u)));
      }
      u->cloneTreeOverTree(approximationForSeed);
      result = true;
    }
  }
  approximations->removeTree();
  return result;
}

template <typename T>
T Random::ApproximateIgnoringSeed(const Tree* randomTree) {
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

template bool Random::ApproximateTreeNodes<float>(Tree* tree);
template float Random::ApproximateIgnoringSeed<float>(const Tree* randomTree);
template bool Random::ApproximateTreeNodes<double>(Tree* tree);
template double Random::ApproximateIgnoringSeed<double>(const Tree* randomTree);

}  // namespace PoincareJ
