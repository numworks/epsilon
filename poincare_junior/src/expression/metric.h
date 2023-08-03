#ifndef POINCARE_EXPRESSION_METRIC_H
#define POINCARE_EXPRESSION_METRIC_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Metric {
 public:
  Metric(const Tree* tree, const Tree* root)
      : m_tree(tree),
        m_root(root),
        m_algebraicRoot(AlgebraicRoot(tree, root)),
        m_numberOfVariables(NumberOfVariables(m_algebraicRoot)),
        m_treeSize(tree->treeSize()),
        m_type(tree->type()) {}

  // Return true if m_tree is better reduced.
  bool hasImproved() const;

 private:
  // Find the nearest algebraic root
  static const Tree* AlgebraicRoot(const Tree* tree, const Tree* root);
  // Number of polynomial variables
  static int NumberOfVariables(const Tree* tree);

  const Tree* m_tree;
  const Tree* m_root;
  const Tree* m_algebraicRoot;
  int m_numberOfVariables;
  int m_treeSize;
  BlockType m_type;
};

}  // namespace PoincareJ

#endif
