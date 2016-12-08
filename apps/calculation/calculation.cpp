#include "calculation.h"
#include <string.h>

namespace Calculation {

Calculation::Calculation() :
  m_expression(nullptr),
  m_layout(nullptr),
  m_evaluation(nullptr),
  m_evaluationLayout(nullptr)
{
}

Calculation & Calculation::operator= (const Calculation & other) {
  strlcpy(m_text, other.m_text, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = nullptr;
  if (other.m_expression) {
    m_expression = Expression::parse(m_text);
  }
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = nullptr;
  if (m_expression && other.m_layout) {
    m_layout = m_expression->createLayout();
  }
  if (m_evaluation != nullptr) {
    delete m_evaluation;
  }
  m_evaluation = nullptr;
  if (other.m_evaluation) {
    m_evaluation = other.m_evaluation->clone();
  }
  if (m_evaluationLayout != nullptr) {
    delete m_evaluationLayout;
  }
  m_evaluationLayout = nullptr;
  if (m_evaluation && other.m_evaluationLayout) {
    m_evaluationLayout = m_evaluation->createLayout();
  }
  return *this;
}

void Calculation::setContent(const char * c, Context * context) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = m_expression->createLayout();
  if (m_evaluation != nullptr) {
    delete m_evaluationLayout;
  }
  m_evaluation = m_expression->evaluate(*context);
  if (m_evaluationLayout != nullptr) {
    delete m_evaluationLayout;
  }
  m_evaluationLayout = m_evaluation->createLayout();
}

Calculation::~Calculation() {
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression != nullptr) {
    delete m_expression;
  }
  if (m_evaluation != nullptr) {
    delete m_evaluation;
  }
  if (m_evaluationLayout != nullptr) {
    delete m_evaluationLayout;
  }
}

const char * Calculation::text() {
  return m_text;
}

Expression * Calculation::expression() {
  return m_expression;
}

ExpressionLayout * Calculation::layout() {
  return m_layout;
}

Expression * Calculation::evaluation() {
  return m_evaluation;
}

ExpressionLayout * Calculation::evaluationLayout() {
  return m_evaluationLayout;
}

bool Calculation::isEmpty() {
  if (m_expression == nullptr) {
    return true;
  }
  return false;
}

}
