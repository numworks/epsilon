#include <poincare/symbol_abstract.h>
#include <poincare/symbol.h>
#include <poincare/expression.h>
#include <poincare/helpers.h>
#include <string.h>

namespace Poincare {

void SymbolAbstractNode::setName(const char * newName, int length) {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

size_t SymbolAbstractNode::size() const {
  return SymbolAbstract::AlignedNodeSize(strlen(name()), nodeSize());
}

void SymbolAbstractNode::initToMatchSize(size_t goalSize) {
  assert(goalSize != nodeSize());
  assert(goalSize > nodeSize());
  size_t nameSize = goalSize - nodeSize();
  char * modifiableName = const_cast<char *>(name());
  for (size_t i = 0; i < nameSize - 1; i++) {
    modifiableName[i] = 'a';
  }
  modifiableName[nameSize-1] = 0;
  assert(size() == goalSize);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
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
    e = e.replaceSymbolWithExpression(Symbol(Symbol::SpecialSymbols::UnknownX), symbol.childAtIndex(0));
  }
  return e;
}

/* TreePool uses adresses and sizes that are multiples of 4 in order to make
 * node moves faster.*/
size_t SymbolAbstract::AlignedNodeSize(size_t nameLength, size_t nodeSize) {
  return Helpers::AlignedSize(nodeSize+nameLength+1, 4);
}

}
