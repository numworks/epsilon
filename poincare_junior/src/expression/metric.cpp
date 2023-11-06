#include "metric.h"

#include <poincare_junior/src/expression/polynomial.h>

namespace PoincareJ {

bool Metric::hasImproved() const {
  assert(m_algebraicRoot == AlgebraicRoot(m_tree, m_root));
  // Warning: m_algebraicRoot may not be simplified, even if m_tree is.
  int numberOfVariables = NumberOfVariables(m_algebraicRoot);
  if (numberOfVariables != m_numberOfVariables) {
    return numberOfVariables < m_numberOfVariables;
  }
  BlockType type = m_tree->type();
  if (type != m_type) {
    // Addition > Multiplication > Anything.
    for (BlockType betterType :
         {BlockType::Addition, BlockType::Complex, BlockType::Multiplication}) {
      if (m_type == betterType || type == betterType) {
        return type == betterType;
      }
    }
  }
  int treeSize = m_tree->treeSize();
  if (treeSize != m_treeSize) {
    return treeSize < m_treeSize;
  }
  /* Use the type order as order of preference. In case of identical type, false
   * must be returned to prevent any further simplification loop (A->B->A->...)
   */
  return static_cast<uint8_t>(type) < static_cast<uint8_t>(m_type);
}

const Tree* Metric::AlgebraicRoot(const Tree* tree, const Tree* root) {
  const Tree* parent = root->parentOfDescendant(tree);
  if (parent) {
    if (parent->isAddition() || parent->isComplex()) {
      return parent;
    }
    if (parent->isMultiplication()) {
      return AlgebraicRoot(parent, root);
    }
  }
  return tree;
}

int Metric::NumberOfVariables(const Tree* tree) {
  Tree* variables = PolynomialParser::GetVariables(tree);
  int result = variables->numberOfChildren();
  variables->removeTree();
  return result;
}

}  // namespace PoincareJ
