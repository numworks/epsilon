#include <poincare/symbol_abstract.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/expression.h>
#include <poincare/helpers.h>
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

size_t SymbolAbstract::TruncateExtension(char * dst, const char * src, size_t len) {
  const char * cur = src;
  const char * end = src+len-1;
  while (*cur != '.' && cur < end) {
    *dst++ = *cur++;
  }
  *dst = 0;
  return cur-src;
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
    e = e.replaceSymbolWithExpression(Symbol::Builder(Symbol::SpecialSymbols::UnknownX), symbol.childAtIndex(0));
  }
  return e;
}

bool SymbolAbstract::isReal(const SymbolAbstract & symbol, Context & context) {
  Expression e = SymbolAbstract::Expand(symbol, context, false);
  if (e.isUninitialized()) {
    return true;
  }
  return e.isReal(context);
}

}
