#include "function.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

ExpressionModel::ExpressionModel() :
  m_text{0},
  m_expression(nullptr),
  m_layoutRef(nullptr)
{
}

ExpressionModel::~ExpressionModel() {
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. */
  if (m_layoutRef.isDefined()) {
    m_layoutRef = LayoutRef(nullptr);
  }
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
}

ExpressionModel& ExpressionModel::operator=(const ExpressionModel& other) {
  // Self-assignment is benign
  setContent(other.m_text);
  return *this;
}

const char * ExpressionModel::text() const {
  return m_text;
}

Poincare::Expression * ExpressionModel::expression(Poincare::Context * context) const {
  if (m_expression == nullptr) {
    m_expression = Expression::ParseAndSimplify(m_text, *context);
  }
  return m_expression;
}

LayoutRef ExpressionModel::layoutRef() {
  if (!m_layoutRef.isDefined()) {
    Expression * nonSimplifiedExpression = Expression::parse(m_text);
    if (nonSimplifiedExpression != nullptr) {
      m_layoutRef = nonSimplifiedExpression->createLayout(PrintFloat::Mode::Decimal);
      delete nonSimplifiedExpression;
    }
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
  if (m_layoutRef.isDefined()) {
    m_layoutRef = LayoutRef(nullptr);
  }
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
}

void ExpressionModel::tidy() {
  if (m_layoutRef.isDefined()) {
    m_layoutRef = LayoutRef(nullptr);
  }
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
}

}
