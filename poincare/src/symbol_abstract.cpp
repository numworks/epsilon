#include <poincare/symbol_abstract.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/dependency.h>
#include <poincare/function.h>
#include <poincare/rational.h>
#include <poincare/sequence.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/helpers.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>
#include <algorithm>

namespace Poincare {

size_t SymbolAbstractNode::size() const {
  return nodeSize() + strlen(name()) + 1;
}

TrinaryBoolean SymbolAbstractNode::isPositive(Context * context) const {
  SymbolAbstract s(this);
  // No need to preserve undefined symbols here.
  Expression e = SymbolAbstract::Expand(s, context, true, SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  if (e.isUninitialized()) {
    return TrinaryBoolean::Unknown;
  }
  return e.isPositive(context);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  assert(type() == e->type());
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

int SymbolAbstractNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return std::min<int>(strlcpy(buffer, name(), bufferSize), bufferSize - 1);
}


bool SymbolAbstractNode::involvesCircularity(Context * context, int maxDepth, const char * * visitedSymbols, int numberOfVisitedSymbols) {
  // Check if this symbol has already been visited.
  for (int i = 0; i < numberOfVisitedSymbols; i++) {
    if (strcmp(name(), visitedSymbols[i]) == 0) {
      return true;
    }
  }

  // Check children of this (useful for function parameters)
  if (ExpressionNode::involvesCircularity(context, maxDepth, visitedSymbols, numberOfVisitedSymbols)) {
    return true;
  }

  // Check for circularity in the expression of the symbol and decrease depth
  Expression e = context->expressionForSymbolAbstract(SymbolAbstract(this), false);

  if (e.isUninitialized()) {
    return false;
  }

  maxDepth--;
  if (maxDepth < 0) {
    /* We went too deep into the check and consider the expression to be
     * circular. */
    return true;
  }
  visitedSymbols[numberOfVisitedSymbols] = name();
  numberOfVisitedSymbols++;
  return e.involvesCircularity(context, maxDepth, visitedSymbols, numberOfVisitedSymbols);
}

template <typename T, typename U>
T SymbolAbstract::Builder(const char * name, int length) {
  if (AliasesLists::k_thetaAliases.contains(name, length)) {
    name = AliasesLists::k_thetaAliases.mainAlias();
    length = strlen(name);
  }
  size_t size = sizeof(U) + length + 1;
  void * bufferNode = TreePool::sharedPool()->alloc(size);
  U * node = new (bufferNode) U(name, length);
  assert(node->size() == size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<T &>(h);
}

bool SymbolAbstract::hasSameNameAs(const SymbolAbstract & other) const {
  return strcmp(other.name(), name()) == 0;
}

size_t SymbolAbstract::TruncateExtension(char * dst, const char * src, size_t len) {
  return UTF8Helper::CopyUntilCodePoint(dst, len, src, '.');
}

bool SymbolAbstract::matches(const SymbolAbstract & symbol, ExpressionTrinaryTest test, Context * context, void * auxiliary) {
  // Undefined symbols must be preserved.
  Expression e = SymbolAbstract::Expand(symbol, context, false, ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  return !e.isUninitialized() && e.recursivelyMatches(test, context, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol, auxiliary);
}

void SymbolAbstract::checkForCircularityIfNeeded(Context * context, TrinaryBoolean * isCircular) {
  assert(*isCircular != TrinaryBoolean::True);
  if (*isCircular == TrinaryBoolean::Unknown) {
    const char * visitedSymbols[Expression::k_maxSymbolReplacementsCount];
    *isCircular = BinaryToTrinaryBool(involvesCircularity(context, Expression::k_maxSymbolReplacementsCount, visitedSymbols, 0));
  }
}

Expression SymbolAbstract::Expand(const SymbolAbstract & symbol, Context * context, bool clone, ExpressionNode::SymbolicComputation symbolicComputation) {
  assert(context);
  Expression e = context->expressionForSymbolAbstract(symbol, clone);
  /* Replace all the symbols iteratively. This prevents a memory failure when
   * symbols are defined circularly. Symbols defined in a parametered function
   * will be preserved as long as the function is defined within this symbol. */
  e = Expression::ExpressionWithoutSymbols(e, context, symbolicComputation);
  if (!e.isUninitialized() && symbol.type() == ExpressionNode::Type::Function) {
    Dependency d = Dependency::Builder(e);
    d.addDependency(symbol.childAtIndex(0));
    return std::move(d);
  }
  return e;
}

template Constant SymbolAbstract::Builder<Constant, ConstantNode>(char const*, int);
template Function SymbolAbstract::Builder<Function, FunctionNode>(char const*, int);
template Sequence SymbolAbstract::Builder<Sequence, SequenceNode>(char const*, int);
template Symbol SymbolAbstract::Builder<Symbol, SymbolNode>(char const*, int);


}
