#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/dependency.h>
#include <poincare/function.h>
#include <poincare/helpers.h>
#include <poincare/k_tree.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/sequence.h>
#include <poincare/symbol.h>
#include <poincare/symbol_abstract.h>
#include <poincare/undefined.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/memory/tree_stack.h>
#include <string.h>

#include <algorithm>

namespace Poincare {

SymbolAbstractNode::SymbolAbstractNode(const char *newName, int length)
    : ExpressionNode() {
  assert(NameLengthIsValid(newName, length));
  strlcpy(m_name, newName, length + 1);
}

size_t SymbolAbstractNode::NameWithoutQuotationMarks(char *buffer,
                                                     size_t bufferSize,
                                                     const char *name,
                                                     size_t nameLength) {
  if (NameHasQuotationMarks(name, nameLength)) {
    assert(bufferSize > nameLength - 2);
    size_t result = strlcpy(buffer, name + 1, bufferSize) - 1;
    buffer[nameLength - 2] = 0;  // Remove the last '""
    return result;
  }
  assert(bufferSize > nameLength);
  return strlcpy(buffer, name, bufferSize);
}

OExpression SymbolAbstractNode::replaceSymbolWithExpression(
    const SymbolAbstract &symbol, const OExpression &expression) {
  return SymbolAbstract(this).replaceSymbolWithExpression(symbol, expression);
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

TrinaryBoolean SymbolAbstractNode::isPositive(Context *context) const {
  SymbolAbstract s(this);
  // No need to preserve undefined symbols here.
  OExpression e = SymbolAbstract::Expand(
      s, context, true,
      SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  if (e.isUninitialized()) {
    return TrinaryBoolean::Unknown;
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

bool SymbolAbstractNode::involvesCircularity(Context *context, int maxDepth,
                                             const char **visitedSymbols,
                                             int numberOfVisitedSymbols) {
  if (otype() == ExpressionNode::Type::Sequence) {
    return ExpressionNode::involvesCircularity(
        context, maxDepth, visitedSymbols, numberOfVisitedSymbols);
  }
  // Check if this symbol has already been visited.
  for (int i = 0; i < numberOfVisitedSymbols; i++) {
    if (strcmp(name(), visitedSymbols[i]) == 0) {
      return true;
    }
  }

  // Check children of this (useful for function parameters)
  if (ExpressionNode::involvesCircularity(context, maxDepth, visitedSymbols,
                                          numberOfVisitedSymbols)) {
    return true;
  }

  // Check for circularity in the expression of the symbol and decrease depth
  maxDepth--;
  if (maxDepth < 0) {
    /* We went too deep into the check and consider the expression to be
     * circular. */
    return true;
  }
  visitedSymbols[numberOfVisitedSymbols] = m_name;
  numberOfVisitedSymbols++;

  OExpression symbolAbstract;
  if (otype() == ExpressionNode::Type::Function) {
    // This is like cloning, but without the symbol.
    symbolAbstract = Function::Builder(name(), strlen(name()),
                                       Symbol::Builder(UCodePointUnknown));
  } else {
    assert(otype() == ExpressionNode::Type::Symbol);
    symbolAbstract = SymbolAbstract(this);
  }

  OExpression e = context->expressionForSymbolAbstract(
      static_cast<SymbolAbstract &>(symbolAbstract), false);

  return !e.isUninitialized() &&
         e.involvesCircularity(context, maxDepth, visitedSymbols,
                               numberOfVisitedSymbols);
}

const char *SymbolAbstract::name() const {
  return PoincareJ::Symbol::GetName(tree());
}

bool SymbolAbstract::hasSameNameAs(const SymbolAbstract &other) const {
  return strcmp(other.name(), name()) == 0;
}

bool SymbolAbstract::matches(const SymbolAbstract &symbol,
                             JuniorExpression::ExpressionTrinaryTest test,
                             Context *context, void *auxiliary,
                             JuniorExpression::IgnoredSymbols *ignoredSymbols) {
  // Undefined symbols must be preserved.
  JuniorExpression e = SymbolAbstract::Expand(
      symbol, context, true,
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  return !e.isUninitialized() &&
         e.recursivelyMatches(test, context,
                              SymbolicComputation::DoNotReplaceAnySymbol,
                              auxiliary, ignoredSymbols);
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

void SymbolAbstract::checkForCircularityIfNeeded(Context *context,
                                                 TrinaryBoolean *isCircular) {
  assert(*isCircular != TrinaryBoolean::True);
  if (*isCircular == TrinaryBoolean::Unknown) {
    const char *visitedSymbols[OExpression::k_maxSymbolReplacementsCount];
    *isCircular = BinaryToTrinaryBool(involvesCircularity(
        context, OExpression::k_maxSymbolReplacementsCount, visitedSymbols, 0));
  }
}

JuniorExpression SymbolAbstract::Expand(
    const SymbolAbstract &symbol, Context *context, bool clone,
    SymbolicComputation symbolicComputation) {
  assert(context);
  JuniorExpression e = context->expressionForSymbolAbstract(symbol, clone);
  /* Replace all the symbols iteratively. This prevents a memory failure when
   * symbols are defined circularly. Symbols defined in a parametered function
   * will be preserved as long as the function is defined within this symbol. */
  e = JuniorExpression::ExpressionWithoutSymbols(e, context,
                                                 symbolicComputation);
  if (!e.isUninitialized() && symbol.type() == ExpressionNode::Type::Function) {
    e = JuniorExpression::Create(KDep(KA, KSet(KB)),
                                 {.KA = e, .KB = e.childAtIndex(0)});
  }
  return e;
}

}  // namespace Poincare
