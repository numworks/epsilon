#include <poincare/symbol.h>
#include <poincare/context.h>
#include <stdlib.h>
#include "layout/string_layout.h"

Symbol::Symbol(char * name) {
  size_t length = strlen(name);
  m_name = (char *)malloc(sizeof(char)*length+1);
  memcpy(m_name, name, length);
  m_name[length] = 0;
}

Symbol::~Symbol() {
  free(m_name);
}

float Symbol::approximate(Context& context) {
  return context[m_name]->approximate(context);
}

expression_type_t Symbol::type() {
  return Symbol::Type;
}

ExpressionLayout * Symbol::createLayout(ExpressionLayout * parent) {
  size_t length = strlen(m_name);
  return new StringLayout(parent, m_name, length);
}
