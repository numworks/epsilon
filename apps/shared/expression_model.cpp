#include "function.h"
#include "poincare_helpers.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

ExpressionModel::ExpressionModel() :
  m_text{0},
  m_expression(),
  m_layoutRef()
{
}

const char * ExpressionModel::text() const {
  return m_text;
}

Poincare::Expression ExpressionModel::expression(Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    m_expression = PoincareHelpers::ParseAndSimplify(m_text, *context);
  }
  return m_expression;
}

LayoutReference ExpressionModel::layoutRef() {
  if (m_layoutRef.isUninitialized()) {
    Expression nonSimplifiedExpression = Expression::parse(m_text);
    m_layoutRef = PoincareHelpers::CreateLayout(nonSimplifiedExpression);
  }
  return m_layoutRef;
}

bool ExpressionModel::isDefined() {
  return m_text[0] != 0;
}

bool ExpressionModel::isEmpty() {
  return m_text[0] == 0;
}

void ExpressionModel::setContent(const char * c) {
  strlcpy(m_text, c, sizeof(m_text));
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. And here we want to delete only
   * the m_layout and m_expression. */
  m_layoutRef = LayoutReference();
  m_expression = Expression();
}

void ExpressionModel::tidy() {
  m_layoutRef = LayoutReference();
  m_expression = Expression();
}

}
