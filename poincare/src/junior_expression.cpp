#include <poincare/junior_expression.h>
#include <poincare/junior_layout.h>
#include <poincare/matrix.h>
#include <poincare_junior/src/expression/conversion.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>

namespace Poincare {

Layout JuniorExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return JuniorLayout::Builder(PoincareJ::Layoutter::LayoutExpression(
      tree()->clone(), false, numberOfSignificantDigits, floatDisplayMode));
}

size_t JuniorExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  Expression e = PoincareJ::ToPoincareExpression(tree());
  return e.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

JuniorExpression JuniorExpression::Parse(const PoincareJ::Tree* layout,
                                         Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  // TODO_PCJ: Use context, addMissingParenthesis and parseForAssignment.
  return Builder(PoincareJ::Parser::Parse(layout));
}

JuniorExpression JuniorExpression::Parse(char const* string, Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  if (string[0] == 0) {
    return JuniorExpression();
  }
  PoincareJ::Tree* layout = PoincareJ::RackFromText(string);
  if (!layout) {
    return JuniorExpression();
  }
  JuniorExpression result =
      Parse(layout, context, addMissingParenthesis, parseForAssignment);
  layout->removeTree();
  return result;
}

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
  if (tree) {
    tree->removeTree();
  }
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
