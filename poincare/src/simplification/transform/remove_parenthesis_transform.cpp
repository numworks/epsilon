#include "transform.h"
#include <assert.h>
#include <poincare/parenthesis.h>

bool Poincare::Simplification::RemoveParenthesisTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Parenthesis);

  Parenthesis * p = static_cast<Parenthesis *>(captures[0]);
  assert(p->numberOfOperands() == 1);

  static_cast<Hierarchy *>(p->parent())->replaceOperand(p, const_cast<Expression *>(p->operand(0)), false);
  delete p;

  return true;
}
