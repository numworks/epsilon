#include "sequence.h"
#include <string.h>

using namespace Shared;

namespace Sequence {

Sequence::Sequence(const char * text, KDColor color) :
  Function(text, color),
  m_type(Type::Explicite),
  m_firstInitialConditionText(""),
  m_secondInitialConditionText(""),
  m_firstInitialConditionExpression(nullptr),
  m_secondInitialConditionExpression(nullptr),
  m_firstInitialConditionLayout(nullptr),
  m_secondInitialConditionLayout(nullptr)
{
}

Sequence::~Sequence() {
  ((Function *)this)->Shared::Function::~Function();
  if (m_firstInitialConditionLayout != nullptr) {
    delete m_firstInitialConditionLayout;
    m_firstInitialConditionLayout = nullptr;
  }
  if (m_secondInitialConditionLayout != nullptr) {
    delete m_secondInitialConditionLayout;
    m_secondInitialConditionLayout = nullptr;
  }
  if (m_firstInitialConditionExpression != nullptr) {
    delete m_firstInitialConditionExpression;
    m_firstInitialConditionExpression = nullptr;
  }
  if (m_secondInitialConditionExpression != nullptr) {
    delete m_secondInitialConditionExpression;
    m_secondInitialConditionExpression = nullptr;
  }
}

const char * Sequence::firstInitialConditionText() {
  return m_firstInitialConditionText;
}

const char * Sequence::secondInitialConditionText() {
  return m_secondInitialConditionText;
}

Sequence::Type Sequence::type() {
  return m_type;
}

void Sequence::setType(Type type) {
  m_type = type;
}

Poincare::Expression * Sequence::firstInitialConditionExpression() {
  return m_firstInitialConditionExpression;
}

Poincare::Expression * Sequence::secondInitialConditionExpression() {
  return m_secondInitialConditionExpression;
}

Poincare::ExpressionLayout * Sequence::firstInitialConditionLayout() {
  return m_firstInitialConditionLayout;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionLayout() {
  return m_secondInitialConditionLayout;
}

void Sequence::setFirstInitialConditionContent(const char * c) {
  strlcpy(m_firstInitialConditionText, c, sizeof(m_firstInitialConditionText));
  if (m_firstInitialConditionExpression != nullptr) {
    delete m_firstInitialConditionExpression;
  }
  m_firstInitialConditionExpression = Poincare::Expression::parse(m_firstInitialConditionText);
  if (m_firstInitialConditionLayout != nullptr) {
    delete m_firstInitialConditionLayout;
  }
  m_firstInitialConditionLayout = nullptr;
  if (m_firstInitialConditionExpression) {
    m_firstInitialConditionLayout = m_firstInitialConditionExpression->createLayout();
  }
}

void Sequence::setSecondInitialConditionContent(const char * c) {
  strlcpy(m_secondInitialConditionText, c, sizeof(m_secondInitialConditionText));
  if (m_secondInitialConditionExpression != nullptr) {
    delete m_secondInitialConditionExpression;
  }
  m_secondInitialConditionExpression = Poincare::Expression::parse(m_secondInitialConditionText);
  if (m_secondInitialConditionLayout != nullptr) {
    delete m_secondInitialConditionLayout;
  }
  m_secondInitialConditionLayout = nullptr;
  if (m_secondInitialConditionExpression) {
    m_secondInitialConditionLayout = m_secondInitialConditionExpression->createLayout();
  }
}

char Sequence::symbol() const {
  return 'n';
}

}
