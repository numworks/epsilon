#ifndef POINCARE_EXPRESSION_RANDOM_H
#define POINCARE_EXPRESSION_RANDOM_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Random {
  /* Random nodes have a random seed metadata.
   * Identical node and seed should approximate to the same value.
   * A null seed indicates a un-seeded node.
   * For simplicity, distinct nodes cannot have a same seed. */
 public:
  // Takes a Tree containing un-seeded random nodes only, and seed them.
  static bool SeedTreeNodes(Tree* tree);
  // Approximate a root tree's random (and seeded) nodes inplace.
  template <typename T>
  static bool ApproximateTreeNodes(Tree* tree);
  static uint8_t GetSeed(const Tree* randomTree) {
    assert(randomTree->isRandomNode());
    return randomTree->nodeValue(0);
  }
  template <typename T>
  static T ApproximateIgnoringSeed(const Tree* randomTree);

 private:
  static void SetSeed(Tree* randomTree, uint8_t seed) {
    assert(randomTree->isRandomNode() && GetSeed(randomTree) == 0);
    randomTree->setNodeValue(0, seed);
  }
};

}  // namespace PoincareJ

#endif
