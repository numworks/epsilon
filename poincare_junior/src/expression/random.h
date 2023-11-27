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
  static uint8_t GetSeed(const Tree* randomTree) {
    assert(randomTree->isRandomNode());
    return randomTree->nodeValue(0);
  }
};

}  // namespace PoincareJ

#endif
