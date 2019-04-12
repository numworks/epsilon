#include <poincare/symbol_abstract.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/function.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/expression.h>
#include <poincare/helpers.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>

namespace Poincare {

size_t SymbolAbstractNode::size() const {
  return nodeSize() + strlen(name()) + 1;
}

ExpressionNode::Sign SymbolAbstractNode::sign(Context * context) const {
  SymbolAbstract s(this);
  Expression e = SymbolAbstract::Expand(s, *context, false);
  if (e.isUninitialized()) {
    return Sign::Unknown;
  }
  return e.sign(context);
}

Expression SymbolAbstractNode::setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  SymbolAbstract sa(this);
  Expression e = SymbolAbstract::Expand(sa, *context, true);
  assert(!e.isUninitialized());
  sa.replaceWithInPlace(e);
  return e.setSign(s, context, complexFormat, angleUnit, target);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  assert(type() == e->type());
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

template <typename T, typename U>
T SymbolAbstract::Builder(const char * name, int length) {
  size_t size = sizeof(U) + length + 1;
  void * bufferNode = TreePool::sharedPool()->alloc(size);
  U * node = new (bufferNode) U(name, length);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<T &>(h);
}

size_t SymbolAbstract::TruncateExtension(char * dst, const char * src, size_t len) {
  return UTF8Helper::CopyUntilCodePoint(dst, len, src, '.');
}

bool SymbolAbstract::matches(const SymbolAbstract & symbol, ExpressionTest test, Context & context) {
  Expression e = SymbolAbstract::Expand(symbol, context, false);
  return !e.isUninitialized() && test(e, context, false);
}

Expression SymbolAbstract::Expand(const SymbolAbstract & symbol, Context & context, bool clone) {
  bool isFunction = symbol.type() == ExpressionNode::Type::Function;
  /* Always clone the expression for Function because we are going to alter e
   * by replacing all UnknownX in it. */
  Expression e = context.expressionForSymbol(symbol, clone || isFunction);
  /* Replace all the symbols iteratively. This prevents a memory failure when
   * symbols are defined circularly. */
  e = Expression::ExpressionWithoutSymbols(e, context);
  if (!e.isUninitialized() && isFunction) {
    // TODO: when SequenceFunction is created, we want to specify which unknown variable to replace (UnknownX or UnknownN)
    e = e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknownX), symbol.childAtIndex(0));
  }
  return e;
}

bool SymbolAbstract::isReal(const SymbolAbstract & symbol, Context & context) {
  Expression e = SymbolAbstract::Expand(symbol, context, false);
  if (e.isUninitialized()) {
    return false;
  }
  return e.isReal(context);
}

template Constant SymbolAbstract::Builder<Constant, ConstantNode>(char const*, int);
template Function SymbolAbstract::Builder<Function, FunctionNode>(char const*, int);
template Symbol SymbolAbstract::Builder<Symbol, SymbolNode>(char const*, int);

}
