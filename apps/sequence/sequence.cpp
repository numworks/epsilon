#include "sequence.h"
#include "local_context.h"
#include "../../poincare/src/layout/string_layout.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include <assert.h>
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
  /* We temporarely store other's required features to be able to access them
   * after setType (which erase all contents and index buffer) even in case of
   * self assignement */
  const char * contentText = other.text();
  const char * firstInitialText = other.m_firstInitialConditionText;
  const char * secondInitialText = other.m_secondInitialConditionText;
  int indexBuffer0 = other.m_indexBuffer[0];
  int indexBuffer1 = other.m_indexBuffer[1];
  Function::operator=(other);
  setType(other.m_type);
  setContent(contentText);
  setFirstInitialConditionContent(firstInitialText);
  setSecondInitialConditionContent(secondInitialText);
  m_indexBuffer[0] = indexBuffer0;
  m_indexBuffer[1] = indexBuffer1;
  return *this;
}

uint32_t Sequence::checksum() {
  char data[k_dataLengthInBytes/sizeof(char)] = {};
  strlcpy(data, text(), TextField::maxBufferSize());
  strlcpy(data+TextField::maxBufferSize(), firstInitialConditionText(), TextField::maxBufferSize());
  strlcpy(data+2*TextField::maxBufferSize(), secondInitialConditionText(), TextField::maxBufferSize());
  data[k_dataLengthInBytes-3] = (char)m_type;
  data[k_dataLengthInBytes-2] = name()!= nullptr ? name()[0] : 0;
  data[k_dataLengthInBytes-1] = isActive() ? 1 : 0;
  return Ion::crc32((uint32_t *)data, k_dataLengthInBytes>>2);
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
  tidy();
  /* Reset all contents */
  switch (m_type) {
    case Type::Explicite:
      setContent("");
      break;
    case Type::SingleRecurrence:
    {
      char ex[5] = "u(n)";
      ex[0] = name()[0];
      setContent(ex);
      break;
    }
    case Type::DoubleRecurrence:
    {
      char ex[12] = "u(n+1)+u(n)";
      ex[0] = name()[0];
      ex[7] = name()[0];
      setContent(ex);
      break;
    }
  }
  setFirstInitialConditionContent("");
  setSecondInitialConditionContent("");
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
}

Poincare::Expression * Sequence::firstInitialConditionExpression() const {
  if (m_firstInitialConditionExpression == nullptr) {
    m_firstInitialConditionExpression = Poincare::Expression::parse(m_firstInitialConditionText);
  }
  return m_firstInitialConditionExpression;
}

Poincare::Expression * Sequence::secondInitialConditionExpression() const {
  if (m_secondInitialConditionExpression == nullptr) {
    m_secondInitialConditionExpression = Poincare::Expression::parse(m_secondInitialConditionText);
  }
  return m_secondInitialConditionExpression;
}

Poincare::ExpressionLayout * Sequence::firstInitialConditionLayout() {
  if (m_firstInitialConditionLayout == nullptr && firstInitialConditionExpression() != nullptr) {
    m_firstInitialConditionLayout = firstInitialConditionExpression()->createLayout(Expression::FloatDisplayMode::Decimal);
  }
  return m_firstInitialConditionLayout;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionLayout() {
  if (m_secondInitialConditionLayout == nullptr && secondInitialConditionExpression()) {
    m_secondInitialConditionLayout = secondInitialConditionExpression()->createLayout(Expression::FloatDisplayMode::Decimal);
  }
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
    m_firstInitialConditionExpression = nullptr;
  }
  if (m_firstInitialConditionLayout != nullptr) {
    delete m_firstInitialConditionLayout;
    m_firstInitialConditionLayout = nullptr;
  }
  m_indexBuffer[0] = -1;
  m_indexBuffer[1] = -1;
}

void Sequence::setSecondInitialConditionContent(const char * c) {
  strlcpy(m_secondInitialConditionText, c, sizeof(m_secondInitialConditionText));
  if (m_secondInitialConditionExpression != nullptr) {
    delete m_secondInitialConditionExpression;
    m_secondInitialConditionExpression = nullptr;
  }
  if (m_secondInitialConditionLayout != nullptr) {
    delete m_secondInitialConditionLayout;
    m_secondInitialConditionLayout = nullptr;
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
  if (m_nameLayout == nullptr) {
    m_nameLayout = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  return m_nameLayout;
}

Poincare::ExpressionLayout * Sequence::definitionName() {
  if (m_definitionName == nullptr) {
    if (m_type == Type::Explicite) {
      m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n ", 2, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_type == Type::SingleRecurrence) {
      m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n+1 ", 4, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_type == Type::DoubleRecurrence) {
      m_definitionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("n+2 ", 4, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
  }
  return m_definitionName;
}

Poincare::ExpressionLayout * Sequence::firstInitialConditionName() {
  if (m_firstInitialConditionName == nullptr) {
    if (m_type == Type::SingleRecurrence) {
      m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("0", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_type == Type::DoubleRecurrence) {
      m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("0", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
  }
  return m_firstInitialConditionName;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionName() {
  if (m_secondInitialConditionName == nullptr) {
    if (m_type == Type::DoubleRecurrence) {
      m_secondInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout("1", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);

    }
  }
  return m_secondInitialConditionName;
}

bool Sequence::isDefined() {
  switch (m_type) {
    case Type::Explicite:
      return strlen(text()) != 0;
    case Type::SingleRecurrence:
      return strlen(text()) != 0 && strlen(firstInitialConditionText()) != 0;
    default:
      return strlen(text()) != 0 && strlen(firstInitialConditionText()) != 0 && strlen(secondInitialConditionText()) != 0;
  }
}

bool Sequence::isEmpty() {
  switch (m_type) {
    case Type::Explicite:
      return Function::isEmpty();
    case Type::SingleRecurrence:
      return Function::isEmpty() && strlen(m_firstInitialConditionText) == 0;
    default:
      return Function::isEmpty() && strlen(m_firstInitialConditionText) == 0 && strlen(m_secondInitialConditionText) == 0;
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
        m_buffer[0] = firstInitialConditionExpression()->approximate(*context);
        return m_buffer[0];
      }
      LocalContext subContext = LocalContext(context);
      Poincare::Symbol nSymbol = Poincare::Symbol(symbol());
      int start = m_indexBuffer[0] < 0 || m_indexBuffer[0] > n ? 0 : m_indexBuffer[0];
      float un = m_indexBuffer[0] < 0 || m_indexBuffer[0] > n ? firstInitialConditionExpression()->approximate(*context) : m_buffer[0];
      for (int i = start; i < n; i++) {
        subContext.setValueForSequenceRank(un, name(), 0);
        Poincare::Complex e = Poincare::Complex::Float(i);
        subContext.setExpressionForSymbolName(&e, &nSymbol);
        un = expression()->approximate(subContext);
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
        return firstInitialConditionExpression()->approximate(*context);
      }
      if (n == 1) {
        m_indexBuffer[0] = 0;
        m_buffer[0] = firstInitialConditionExpression()->approximate(*context);
        m_indexBuffer[1] = 1;
        m_buffer[1] = secondInitialConditionExpression()->approximate(*context);
        return m_buffer[1];
      }
      LocalContext subContext = LocalContext(context);
      Poincare::Symbol nSymbol = Poincare::Symbol(symbol());
      int start = m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_indexBuffer[0] : 0;
      float un = m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_buffer[0] : firstInitialConditionExpression()->approximate(*context);
      float un1 =  m_indexBuffer[0] >= 0 && m_indexBuffer[0] < n && m_indexBuffer[1] > 0 && m_indexBuffer[1] <= n && m_indexBuffer[0] + 1 == m_indexBuffer[1] ? m_buffer[1] : secondInitialConditionExpression()->approximate(*context);
      for (int i = start; i < n-1; i++) {
        subContext.setValueForSequenceRank(un, name(), 0);
        subContext.setValueForSequenceRank(un1, name(), 1);
        Poincare::Complex e = Poincare::Complex::Float(i);
        subContext.setExpressionForSymbolName(&e, &nSymbol);
        un = un1;
        un1 = expression()->approximate(subContext);
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
    /* When |start| >> 1.0f, start + 1.0f = start. In that case, quit the
     * infinite loop. */
    if (i == i-1.0f || i == i+1.0f) {
      return NAN;
    }
    result += evaluateAtAbscissa(i, context);
  }
  return result;
}

void Sequence::tidy() {
  Function::tidy();
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

}
