#include "sequence.h"
#include "sequence_store.h"
#include "cache_context.h"
#include "../../poincare/src/layout/string_layout.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include <string.h>
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

Sequence::Sequence(const char * text, KDColor color) :
  Function(text, color),
  m_type(Type::Explicit),
  m_firstInitialConditionText(),
  m_secondInitialConditionText(),
  m_firstInitialConditionExpression(nullptr),
  m_secondInitialConditionExpression(nullptr),
  m_firstInitialConditionLayout(nullptr),
  m_secondInitialConditionLayout(nullptr),
  m_nameLayout(nullptr),
  m_definitionName(nullptr),
  m_firstInitialConditionName(nullptr),
  m_secondInitialConditionName(nullptr),
  m_initialRank(0)
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
  Function::operator=(other);
  setType(other.m_type);
  setInitialRank(other.m_initialRank);
  setContent(contentText);
  setFirstInitialConditionContent(firstInitialText);
  setSecondInitialConditionContent(secondInitialText);
  return *this;
}

uint32_t Sequence::checksum() {
  char data[k_dataLengthInBytes/sizeof(char)] = {};
  strlcpy(data, text(), TextField::maxBufferSize());
  strlcpy(data+TextField::maxBufferSize(), firstInitialConditionText(), TextField::maxBufferSize());
  strlcpy(data+2*TextField::maxBufferSize(), secondInitialConditionText(), TextField::maxBufferSize());
  int * intAdress = (int *)(&data[3*TextField::maxBufferSize()]);
  *intAdress = m_initialRank;
  data[k_dataLengthInBytes-3] = (char)m_type;
  data[k_dataLengthInBytes-2] = name()!= nullptr ? name()[0] : 0;
  data[k_dataLengthInBytes-1] = (char)(isActive() ? 1 : 0);
  return Ion::crc32((uint32_t *)data, k_dataLengthInBytes/sizeof(uint32_t));
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
  if (m_type == Type::Explicit) {
    setInitialRank(0);
  }
  m_type = type;
  tidy();
  /* Reset all contents */
  switch (m_type) {
    case Type::Explicit:
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
}

void Sequence::setInitialRank(int rank) {
  m_initialRank = rank;
  if (m_firstInitialConditionName != nullptr) {
    delete m_firstInitialConditionName;
    m_firstInitialConditionName = nullptr;
  }
  if (m_secondInitialConditionName != nullptr) {
    delete m_secondInitialConditionName;
    m_secondInitialConditionName = nullptr;
  }
}

Poincare::Expression * Sequence::firstInitialConditionExpression(Context * context) const {
  if (m_firstInitialConditionExpression == nullptr) {
    m_firstInitialConditionExpression = Poincare::Expression::ParseAndSimplify(m_firstInitialConditionText, *context);
  }
  return m_firstInitialConditionExpression;
}

Poincare::Expression * Sequence::secondInitialConditionExpression(Context * context) const {
  if (m_secondInitialConditionExpression == nullptr) {
    m_secondInitialConditionExpression = Poincare::Expression::ParseAndSimplify(m_secondInitialConditionText, *context);
  }
  return m_secondInitialConditionExpression;
}

Poincare::ExpressionLayout * Sequence::firstInitialConditionLayout() {
  if (m_firstInitialConditionLayout == nullptr) {
    Expression * nonSimplifedExpression = Expression::parse(m_firstInitialConditionText);
    if (nonSimplifedExpression) {
      m_firstInitialConditionLayout = nonSimplifedExpression->createLayout(PrintFloat::Mode::Decimal);
      delete nonSimplifedExpression;
    }
  }
  return m_firstInitialConditionLayout;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionLayout() {
  if (m_secondInitialConditionLayout == nullptr) {
    Expression * nonSimplifedExpression = Expression::parse(m_secondInitialConditionText);
    if (nonSimplifedExpression) {
      m_secondInitialConditionLayout = nonSimplifedExpression->createLayout(PrintFloat::Mode::Decimal);
      delete nonSimplifedExpression;
    }
  }
  return m_secondInitialConditionLayout;
}

void Sequence::setContent(const char * c) {
  Function::setContent(c);
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
    if (m_type == Type::Explicit) {
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
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(m_initialRank).writeTextInBuffer(buffer, k_initialRankNumberOfDigits+1);
  if (m_firstInitialConditionName == nullptr) {
    if (m_type == Type::SingleRecurrence) {
      m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_type == Type::DoubleRecurrence) {
      m_firstInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
  }
  return m_firstInitialConditionName;
}

Poincare::ExpressionLayout * Sequence::secondInitialConditionName() {
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(m_initialRank+1).writeTextInBuffer(buffer, k_initialRankNumberOfDigits+1);
  if (m_secondInitialConditionName == nullptr) {
    if (m_type == Type::DoubleRecurrence) {
      m_secondInitialConditionName = new BaselineRelativeLayout(new StringLayout(name(), 1), new StringLayout(buffer, strlen(buffer), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);

    }
  }
  return m_secondInitialConditionName;
}

bool Sequence::isDefined() {
  switch (m_type) {
    case Type::Explicit:
      return strlen(text()) != 0;
    case Type::SingleRecurrence:
      return strlen(text()) != 0 && strlen(firstInitialConditionText()) != 0;
    default:
      return strlen(text()) != 0 && strlen(firstInitialConditionText()) != 0 && strlen(secondInitialConditionText()) != 0;
  }
}

bool Sequence::isEmpty() {
  switch (m_type) {
    case Type::Explicit:
      return Function::isEmpty();
    case Type::SingleRecurrence:
      return Function::isEmpty() && strlen(m_firstInitialConditionText) == 0;
    default:
      return Function::isEmpty() && strlen(m_firstInitialConditionText) == 0 && strlen(m_secondInitialConditionText) == 0;
  }
}

template<typename T>
T Sequence::templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const {
  T n = std::round(x);
  int sequenceIndex = name()[0] == SequenceStore::k_sequenceNames[0][0] ? 0 : 1;
  if (sqctx->iterateUntilRank<T>(n)) {
    return sqctx->valueOfSequenceAtPreviousRank<T>(sequenceIndex, 0);
  }
  return NAN;
}

template<typename T>
T Sequence::approximateToNextRank(int n, SequenceContext * sqctx) const {
  if (n < m_initialRank || n < 0) {
    return NAN;
  }
  CacheContext<T> ctx = CacheContext<T>(sqctx);
  T un = sqctx->valueOfSequenceAtPreviousRank<T>(0, 0);
  T unm1 = sqctx->valueOfSequenceAtPreviousRank<T>(0, 1);
  T unm2 = sqctx->valueOfSequenceAtPreviousRank<T>(0, 2);
  T vn = sqctx->valueOfSequenceAtPreviousRank<T>(1, 0);
  T vnm1 = sqctx->valueOfSequenceAtPreviousRank<T>(1, 1);
  T vnm2 = sqctx->valueOfSequenceAtPreviousRank<T>(1, 2);
  Poincare::Symbol nSymbol(symbol());
  Poincare::Symbol vnSymbol(Symbol::SpecialSymbols::vn);
  Poincare::Symbol vn1Symbol(Symbol::SpecialSymbols::vn1);
  Poincare::Symbol unSymbol(Symbol::SpecialSymbols::un);
  Poincare::Symbol un1Symbol(Symbol::SpecialSymbols::un1);
  switch (m_type) {
    case Type::Explicit:
    {
      ctx.setValueForSymbol(un, &unSymbol);
      ctx.setValueForSymbol(vn, &vnSymbol);
      Poincare::Complex<T> e = Poincare::Complex<T>::Float(n);
      ctx.setExpressionForSymbolName(&e, &nSymbol, *sqctx);
      return expression(sqctx)->template approximateToScalar<T>(ctx);
    }
    case Type::SingleRecurrence:
    {
      if (n == m_initialRank) {
        return firstInitialConditionExpression(sqctx)->template approximateToScalar<T>(*sqctx);
      }
      ctx.setValueForSymbol(un, &un1Symbol);
      ctx.setValueForSymbol(unm1, &unSymbol);
      ctx.setValueForSymbol(vn, &vn1Symbol);
      ctx.setValueForSymbol(vnm1, &vnSymbol);
      Poincare::Complex<T> e = Poincare::Complex<T>::Float(n-1);
      ctx.setExpressionForSymbolName(&e, &nSymbol, *sqctx);
      return expression(sqctx)->template approximateToScalar<T>(ctx);
    }
    default:
    {
      if (n == m_initialRank) {
        return firstInitialConditionExpression(sqctx)->template approximateToScalar<T>(*sqctx);
      }
      if (n == m_initialRank+1) {
        return secondInitialConditionExpression(sqctx)->template approximateToScalar<T>(*sqctx);
      }
      ctx.setValueForSymbol(unm1, &un1Symbol);
      ctx.setValueForSymbol(unm2, &unSymbol);
      ctx.setValueForSymbol(vnm1, &vn1Symbol);
      ctx.setValueForSymbol(vnm2, &vnSymbol);
      Poincare::Complex<T> e = Poincare::Complex<T>::Float(n-2);
      ctx.setExpressionForSymbolName(&e, &nSymbol, *sqctx);
      return expression(sqctx)->template approximateToScalar<T>(ctx);
    }
  }
}

double Sequence::sumBetweenBounds(double start, double end, Context * context) const {
  double result = 0.0;
  if (end-start > k_maxNumberOfTermsInSum || start + 1.0 == start) {
    return NAN;
  }
  for (double i = std::round(start); i <= std::round(end); i = i + 1.0) {
    /* When |start| >> 1.0, start + 1.0 = start. In that case, quit the
     * infinite loop. */
    if (i == i-1.0 || i == i+1.0) {
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

template double Sequence::templatedApproximateAtAbscissa<double>(double, SequenceContext*) const;
template float Sequence::templatedApproximateAtAbscissa<float>(float, SequenceContext*) const;
template double Sequence::approximateToNextRank<double>(int, SequenceContext*) const;
template float Sequence::approximateToNextRank<float>(int, SequenceContext*) const;
}
