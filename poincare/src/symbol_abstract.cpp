#include <poincare/symbol_abstract.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/function.h>
#include <poincare/rational.h>
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

ExpressionNode::Sign SymbolAbstractNode::sign(Context * context) const {
  SymbolAbstract s(this);
  Expression e = SymbolAbstract::Expand(s, context, false);
  if (e.isUninitialized()) {
    return Sign::Unknown;
  }
  return e.sign(context);
}

Expression SymbolAbstractNode::setSign(ExpressionNode::Sign s, ReductionContext reductionContext) {
  SymbolAbstract sa(this);
  Expression e = SymbolAbstract::Expand(sa, reductionContext.context(), true);
  assert(!e.isUninitialized());
  sa.replaceWithInPlace(e);
  return e.setSign(s, reductionContext);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  assert(type() == e->type());
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

int SymbolAbstractNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return std::min<int>(strlcpy(buffer, name(), bufferSize), bufferSize - 1);
}

template <typename T, typename U>
T SymbolAbstract::Builder(const char * name, int length) {
  size_t size = sizeof(U) + length + 1;
  void * bufferNode = TreePool::sharedPool()->alloc(size);
  U * node = new (bufferNode) U(name, length);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<T &>(h);
}

bool SymbolAbstract::hasSameNameAs(const SymbolAbstract & other) const {
  return strcmp(other.name(), name()) == 0;
}

size_t SymbolAbstract::TruncateExtension(char * dst, const char * src, size_t len) {
  return UTF8Helper::CopyUntilCodePoint(dst, len, src, '.');
}

bool SymbolAbstract::matches(const SymbolAbstract & symbol, ExpressionTest test, Context * context) {
  Expression e = SymbolAbstract::Expand(symbol, context, false);
  return !e.isUninitialized() && e.recursivelyMatches(test, context, false);
}

Expression SymbolAbstract::Expand(const SymbolAbstract & symbol, Context * context, bool clone, ExpressionNode::SymbolicComputation symbolicComputation) {
  if (symbolicComputation == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits
    || symbolicComputation == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndReplaceUnits)
  {
    return Undefined::Builder();
  }
  bool shouldNotReplaceSymbols = symbolicComputation == ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions;
  if (symbol.type() == ExpressionNode::Type::Symbol && shouldNotReplaceSymbols) {
    return clone ? symbol.clone() : *const_cast<SymbolAbstract *>(&symbol);
  }
  Expression e = context->expressionForSymbolAbstract(symbol, clone);
  /* Replace all the symbols iteratively. This prevents a memory failure when
   * symbols are defined circularly. */
  e = Expression::ExpressionWithoutSymbols(e, context, shouldNotReplaceSymbols);
  return e;
}

template Constant SymbolAbstract::Builder<Constant, ConstantNode>(char const*, int);
template Function SymbolAbstract::Builder<Function, FunctionNode>(char const*, int);
template Symbol SymbolAbstract::Builder<Symbol, SymbolNode>(char const*, int);

}
