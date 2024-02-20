#include <poincare/junior_expression.h>
#include <poincare/junior_layout.h>
#include <poincare/matrix.h>
#include <poincare_junior/src/expression/conversion.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/sign.h>

namespace Poincare {

JuniorExpression JuniorExpression::Builder(const PoincareJ::Tree* tree) {
  if (!tree) {
    return JuniorExpression();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorExpressionNode) + size);
  JuniorExpressionNode* node =
      new (bufferNode) JuniorExpressionNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorExpression&>(h);
}

JuniorExpression JuniorExpression::Builder(PoincareJ::Tree* tree) {
  JuniorExpression result = Builder(const_cast<const PoincareJ::Tree*>(tree));
  tree->removeTree();
  return result;
}

JuniorExpression JuniorExpression::Juniorize(Expression e) {
  if (e.isUninitialized() ||
      e.type() == ExpressionNode::Type::JuniorExpression) {
    // e is already a junior expression
    return static_cast<JuniorExpression&>(e);
  }
  return Builder(PoincareJ::FromPoincareExpression(e));
}

}  // namespace Poincare
