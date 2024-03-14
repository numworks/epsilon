#ifndef POINCARE_EXPRESSION_RANDOM_H
#define POINCARE_EXPRESSION_RANDOM_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* For now, random node prevent some simplifications in parametric trees.
 *
 * TODO: If we want to explicit parametric trees containing random nodes:
 * - Random nodes should have a System node equivalent, handling the seed
 *   -> This will also prevent from layouting them, and remove some edge cases
 *      in builtins.
 * - SystemRandom nodes must have the seeds as a child instead of metadata.
 *   -> This seed can be any expression (expected to be an integer)
 *      Complex example: with r() random() and r(SEED) the seeded random:
 *        r() + sum(r() + sum(r(),j, 0, k-1) + r(), i, 0, n-1) + r() ->
 *         Would be seeded with
 *        r(0) + sum(r(1+i) + sum(r(1+n+i*k+j, j, 0, k-1) +
 *        r(1+n+n*k+i), i, 0, n) + r(1+n+n*k+n+1)
 * - Before approximation, optimize the seeds :
 *   -> Remove seed if there are no duplicates (and handle approximation)
 *   -> Shift seeds to a lower unused seed is possible
 */

class Random {
  /* Random nodes have a random seed metadata.
   * Identical node and seed should approximate to the same value.
   * A null seed indicates a un-seeded node.
   * For simplicity, distinct nodes cannot have a same seed. */
 public:
  class Context {
   public:
    using VariableType = double;
    Context();
    static constexpr int k_maxNumberOfVariables = 16;
    VariableType m_list[k_maxNumberOfVariables];
  };
  /* Takes a Tree containing un-seeded random nodes only, and seed them. Return
   * the last seed. */
  static uint8_t SeedTreeNodes(Tree* tree);
  static uint8_t GetSeed(const Tree* randomTree) {
    assert(randomTree->isRandomNode());
    return randomTree->nodeValue(0);
  }
  template <typename T>
  static T Approximate(const Tree* randomTree, Context* context,
                       int listElement);

 private:
  template <typename T>
  static T PrivateApproximate(const Tree* randomTree, Context* context,
                              int listElement);
  static void SetSeed(Tree* randomTree, uint8_t seed) {
    assert(randomTree->isRandomNode() && GetSeed(randomTree) == 0);
    randomTree->setNodeValue(0, seed);
  }
  template <typename T>
  static T RandomInt(T a, T b);
};

}  // namespace PoincareJ

#endif
