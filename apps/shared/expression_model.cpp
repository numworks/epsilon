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
  m_layout()
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

Layout ExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    Expression nonSimplifiedExpression = Expression::Parse(m_text);
    m_layout = PoincareHelpers::CreateLayout(nonSimplifiedExpression);
  }
  return m_layout;
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
  m_layout = Layout();
  m_expression = Expression();
}

void ExpressionModel::tidy() {
  m_layout = Layout();
  m_expression = Expression();
}

}
