#include "sequence.h"
#include "local_context.h"
#include "../../poincare/src/layout/string_layout.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include <string.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

Sequence::Sequence(const char * text, KDColor color) :
  Function(text, color),
  m_type(Type::Explicite),
  m_firstInitialConditionText(""),
  m_secondInitialConditionText(""),
  m_firstInitialConditionExpression(nullptr),
  m_secondInitialConditionExpression(nullptr),
  m_firstInitialConditionLayout(nullptr),
  m_secondInitialConditionLayout(nullptr),
  m_nameLayout(nullptr),
  m_definitionName(nullptr),
  m_firstInitialConditionName(nullptr),
  m_secondInitialConditionName(nullptr),
  m_indexBuffer{-1, -1},
  m_buffer{NAN, NAN}
{
}

Sequence::~Sequence() {
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
  if (m_nameLayout != nullptr) {
    delete m_nameLayout;
    m_nameLayout = nullptr;
  }
  if (m_definitionName != nullptr) {
    delete m_definitionName;
    m_definitionName = nullptr;
  }
  if (m_firstInitialConditionName != nullptr) {
    delete m_firstInitialConditionName;
    m_firstInitialConditionName = nullptr;
  }
  if (m_secondInitialConditionName != nullptr) {
    delete m_secondInitialConditionName;
    m_secondInitialConditionName = nullptr;
  }
}

Sequence& Sequence::operator=(const Sequence& other) {
  // Self-assignment is benign
  Function::operator=(other);
  setType(other.m_type);
  setFirstInitialConditionContent(other.m_firstInitialConditionText);
  setSecondInitialConditionContent(other.m_secondInitialConditionText);
  m_indexBuffer[0] = other.m_indexBuffer[0];
  m_indexBuffer[1] = other.m_indexBuffer[1];
  return *this;
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
  if (m_nameLayout != nullptr) {
    delete m_nameLayout;
    m_nameLayout = nullptr;
  }
  if (m_definitionName != nullptr) {
    delete m_definitionName;
    m_definitionName = nullptr;
  }
  if (m_firstInitialConditionName != nullptr) {
    delete m_firstInitialConditionName;
    m_firstInitialConditionName = nullptr;
  }
  if (m_secondInitialConditionName != nullptr) {
    delete m_secondInitialConditionName;
    m_secondInitialConditionName = nullptr;
  }
  m_nameLayout = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  if (m_type == Type::Explicite) {
    m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n ", 2, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_type == Type::SingleRecurrence) {
    m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n+1 ", 4, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("0", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_type == Type::DoubleRecurrence) {
    m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n+2 ", 4, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("0", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    m_secondInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("1", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
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

void Sequence::setContent(const char * c) {
  Function::setContent(c);
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
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
    m_firstInitialConditionLayout = m_firstInitialConditionExpression->createLayout(Expression::FloatDisplayMode::Decimal);
  }
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
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
    m_secondInitialConditionLayout = m_secondInitialConditionExpression->createLayout(Expression::FloatDisplayMode::Decimal);
  }
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
}

char Sequence::symbol() const {
  return 'n';
}

int Sequence::numberOfElements() {
  return (int)m_type + 1;
}

Poincare::ExpressionLayout * Sequence::nameLayout() {
  return m_nameLayout;
}

Poincare::ExpressionLayout * Sequence::definitionName() {
  return m_definitionName;
}

Poincare::ExpressionLayout * Sequence::firstInitialConditionName() {
  return m_firstInitialConditionName;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionName() {
  return m_secondInitialConditionName;
}

bool Sequence::isDefined() {
  switch (m_type) {
    case Type::Explicite:
      return expression() != nullptr;
    case Type::SingleRecurrence:
      return expression() != nullptr && m_firstInitialConditionExpression != nullptr;
    default:
      return expression() != nullptr && m_firstInitialConditionExpression != nullptr && m_secondInitialConditionExpression != nullptr;
  }
}

float Sequence::evaluateAtAbscissa(float x, Poincare::Context * context) const {
  float n = roundf(x);
  switch (m_type) {
    case Type::Explicite:
      if (n < 0) {
        return NAN;
      }
      return Shared::Function::evaluateAtAbscissa(n, context);
    case Type::SingleRecurrence:
    {
      if (n < 0 || n > k_maxRecurrentRank) {
        return NAN;
      }
      if (n == 0) {
        m_indexBuffer[0] = 0;
        m_buffer[0] = m_firstInitialConditionExpression->approximate(*context);
        return m_buffer[0];
      }
      LocalContext subContext = LocalContext(context);
      Poincare::Symbol nSymbol = Poincare::Symbol(symbol());
      int start = m_indexBuffer[0] < 0 || m_indexBuffer[0] > n ? 0 : m_indexBuffer[0];
      float un = m_indexBuffer[0] < 0 || m_indexBuffer[0] > n ? m_firstInitialConditionExpression->approximate(*context) : m_buffer[0];
      for (int i = start; i < n; i++) {
        subContext.setSequenceRankValue(un, 0);
        Poincare::Complex e = Poincare::Complex::Float(i);
        subContext.setExpressionForSymbolName(&e, &nSymbol);
        un = m_expression->approximate(subContext);
      }
      m_buffer[0] = un;
      m_indexBuffer[0] = n;
      return un;
    }
    default:
    {
      if (n < 0 || n > k_maxRecurrentRank) {
        return NAN;
      }
      if (n == 0) {
        return m_firstInitialConditionExpression->approximate(*context);
      }
      if (n == 1) {
        m_indexBuffer[0] = 0;
        m_buffer[0] = m_firstInitialConditionExpression->approximate(*context);
        m_indexBuffer[1] = 1;
        m_buffer[1] = m_secondInitialConditionExpression->approximate(*context);
        return m_buffer[1];
      }
      LocalContext subContext = LocalContext(context);
      Poincare::Symbol nSymbol = Poincare::Symbol(symbol());
      int start = m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_indexBuffer[0] : 0;
      float un = m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_buffer[0] : m_firstInitialConditionExpression->approximate(*context);
      float un1 =  m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_buffer[1] : m_secondInitialConditionExpression->approximate(*context);
      for (int i = start; i < n-1; i++) {
        subContext.setSequenceRankValue(un, 0);
        subContext.setSequenceRankValue(un1, 1);
        Poincare::Complex e = Poincare::Complex::Float(i);
        subContext.setExpressionForSymbolName(&e, &nSymbol);
        un = un1;
        un1 = m_expression->approximate(subContext);
      }
      m_buffer[0] = un;
      m_indexBuffer[0] = n-1;
      m_buffer[1] = un1;
      m_indexBuffer[1] = n;
      return un1;
    }
  }
}

float Sequence::sumOfTermsBetweenAbscissa(float start, float end, Context * context) {
  float result = 0.0f;
  if (end-start > k_maxNumberOfTermsInSum || start + 1.0f == start) {
    return NAN;
  }
  for (float i = roundf(start); i <= roundf(end); i = i + 1.0f) {
    result += evaluateAtAbscissa(i, context);
  }
  return result;
}

}
