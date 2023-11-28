#ifndef POINCARE_EXPRESSION_RANDOM_H
#define POINCARE_EXPRESSION_RANDOM_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Random {
  /* Random nodes have a random seed metadata.
   * Identical node and seed should approximate to the same value.
   * A null seed indicates a un-seeded node.
   * For simplicity, distinct nodes cannot have a same seed. */
 public:
  /* Random context leaves a list tree on the EditionPool to temporarily store
   * the random approximations for each seeds. */
  class Context {
   public:
    explicit Context();
    ~Context();
    EditionReference m_list;

   private:
    /* Make Copy constructor, Move constructor, Copy assignment and Move
     * assignment inaccessible. */
    Context(const Context& other);
    Context(Context&& other);
    Context& operator=(const Context& other);
    Context& operator=(Context&& other);
  };
  // Takes a Tree containing un-seeded random nodes only, and seed them.
  static bool SeedTreeNodes(Tree* tree);
  static uint8_t GetSeed(const Tree* randomTree) {
    assert(randomTree->isRandomNode());
    return randomTree->nodeValue(0);
  }
  template <typename T>
  static T Approximate(const Tree* randomTree, Context* context);

 private:
  template <typename T>
  static T Approximate(const Tree* randomTree);
  static void SetSeed(Tree* randomTree, uint8_t seed) {
    assert(randomTree->isRandomNode() && GetSeed(randomTree) == 0);
    randomTree->setNodeValue(0, seed);
  }
};

}  // namespace PoincareJ

#endif
