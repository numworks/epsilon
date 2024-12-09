#include <omg/utf8_decoder.h>
#include <omg/utf8_helper.h>
#include <poincare/k_tree.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/dependency.h>
#include <poincare/old/function.h>
#include <poincare/old/parenthesis.h>
#include <poincare/old/rational.h>
#include <poincare/old/sequence.h>
#include <poincare/old/symbol.h>
#include <poincare/old/symbol_abstract.h>
#include <poincare/old/undefined.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree_stack.h>
#include <string.h>

#include <algorithm>

namespace Poincare {

SymbolAbstractNode::SymbolAbstractNode(const char *newName, int length)
    : ExpressionNode() {
#if 0
  assert(NameLengthIsValid(newName, length));
#endif
  strlcpy(m_name, newName, length + 1);
}

OExpression SymbolAbstractNode::replaceSymbolWithExpression(
    const SymbolAbstract &symbol, const OExpression &expression) {
  assert(false);
}

ExpressionNode::LayoutShape SymbolAbstractNode::leftLayoutShape() const {
  UTF8Decoder decoder(m_name);
  decoder.nextCodePoint();
  // nextCodePoint asserts that the first character is non-null
  if (decoder.nextCodePoint() == UCodePointNull) {
    return ExpressionNode::LayoutShape::OneLetter;
  }
  return ExpressionNode::LayoutShape::MoreLetters;
}

size_t SymbolAbstractNode::size() const {
  return nodeSize() + strlen(name()) + 1;
}

OMG::Troolean SymbolAbstractNode::isPositive(Context *context) const {
  SymbolAbstract s(this);
  // No need to preserve undefined symbols here.
  OExpression e = SymbolAbstract::Expand(
      s, context, true, SymbolicComputation::ReplaceAllSymbols);
  if (e.isUninitialized()) {
    return OMG::Troolean::Unknown;
  }
  return e.isPositive(context);
}

int SymbolAbstractNode::simplificationOrderSameType(
    const ExpressionNode *e, bool ascending, bool ignoreParentheses) const {
  assert(otype() == e->otype());
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

size_t SymbolAbstractNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return std::min<size_t>(strlcpy(buffer, name(), bufferSize), bufferSize - 1);
}

const char *SymbolAbstract::name() const {
  return Internal::Symbol::GetName(tree());
}

bool SymbolAbstract::hasSameNameAs(const SymbolAbstract &other) const {
  return strcmp(other.name(), name()) == 0;
}

// Implemented in JuniorExpression::replaceSymbolWithExpression
#if 0
JuniorExpression SymbolAbstract::replaceSymbolWithExpression(
    const SymbolAbstract &symbol, const JuniorExpression &expression) {
  deepReplaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == type() && hasSameNameAs(symbol)) {
    JuniorExpression exp = expression.clone();
    if (numberOfChildren() > 0) {
      assert(isOfType(
          {ExpressionNode::Type::Function, ExpressionNode::Type::Sequence}));
      assert(numberOfChildren() == 1 && symbol.numberOfChildren() == 1);
      JuniorExpression myVariable = childAtIndex(0).clone();
      JuniorExpression symbolVariable = symbol.childAtIndex(0);
      if (symbolVariable.type() == ExpressionNode::Type::Symbol) {
        exp = exp.replaceSymbolWithExpression(symbolVariable.convert<Symbol>(),
                                              myVariable);
      } else if (!myVariable.isIdenticalTo(symbolVariable)) {
        return *this;
      }
    }
  JuniorExpression p = parent();
  if (!p.isUninitialized() &&
      p.node()->childAtIndexNeedsUserParentheses(exp, p.indexOfChild(*this))) {
    exp = Parenthesis::Builder(exp);
  }
    replaceWithInPlace(exp);
    return exp;
  }
  return *this;
}
#endif

JuniorExpression SymbolAbstract::Expand(
    const SymbolAbstract &symbol, Context *context, bool clone,
    SymbolicComputation symbolicComputation) {
  assert(context);
#if 0
  JuniorExpression e = context->expressionForUserNamed(symbol, clone);
  /* Replace all the symbols iteratively. This prevents a memory failure when
   * symbols are defined circularly. Symbols defined in a parametered function
   * will be preserved as long as the function is defined within this symbol. */
  e = JuniorExpression::ExpressionWithoutSymbols(e, context,
                                                 symbolicComputation);
  if (!e.isUninitialized() && symbol.isUserFunction()) {
    e = JuniorExpression::Create(KDep(KA, KDepList(KB)),
                                 {.KA = e, .KB = symbol.tree()->child(0)});
  }
  return e;
#endif
}

}  // namespace Poincare
