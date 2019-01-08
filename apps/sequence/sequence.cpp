#include "sequence.h"
#include "sequence_store.h"
#include "cache_context.h"
#include <poincare/layout_helper.h>
#include <poincare/char_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/integer.h>
#include "../shared/poincare_helpers.h"
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
  m_firstInitialConditionExpression(),
  m_secondInitialConditionExpression(),
  m_firstInitialConditionLayout(),
  m_secondInitialConditionLayout(),
  m_nameLayout(),
  m_definitionName(),
  m_firstInitialConditionName(),
  m_secondInitialConditionName(),
  m_initialRank(0)
{
}

uint32_t Sequence::checksum() {
  constexpr size_t dataSize = k_dataLengthInBytes/sizeof(char);
  char data[dataSize] = {};
  strlcpy(data, text(), dataSize);
  strlcpy(data+TextField::maxBufferSize(), firstInitialConditionText(), dataSize - TextField::maxBufferSize());
  strlcpy(data+2*TextField::maxBufferSize(), secondInitialConditionText(), dataSize - 2*TextField::maxBufferSize());
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
  m_firstInitialConditionName = Layout();
  m_secondInitialConditionName = Layout();
}

Poincare::Expression Sequence::firstInitialConditionExpression(Context * context) const {
  if (m_firstInitialConditionExpression.isUninitialized()) {
    m_firstInitialConditionExpression = PoincareHelpers::ParseAndSimplify(m_firstInitialConditionText, *context);
  }
  return m_firstInitialConditionExpression;
}

Poincare::Expression Sequence::secondInitialConditionExpression(Context * context) const {
  if (m_secondInitialConditionExpression.isUninitialized()) {
    m_secondInitialConditionExpression = PoincareHelpers::ParseAndSimplify(m_secondInitialConditionText, *context);
  }
  return m_secondInitialConditionExpression;
}

Poincare::Layout Sequence::firstInitialConditionLayout() {
  if (m_firstInitialConditionLayout.isUninitialized()) {
    Expression nonSimplifedExpression = Expression::Parse(m_firstInitialConditionText);
    if (!nonSimplifedExpression.isUninitialized()) {
      m_firstInitialConditionLayout = PoincareHelpers::CreateLayout(nonSimplifedExpression);
    }
  }
  return m_firstInitialConditionLayout;
}

Poincare::Layout Sequence::secondInitialConditionLayout() {
  if (m_secondInitialConditionLayout.isUninitialized()) {
    Expression nonSimplifedExpression = Expression::Parse(m_secondInitialConditionText);
    if (!nonSimplifedExpression.isUninitialized()) {
      m_secondInitialConditionLayout = PoincareHelpers::CreateLayout(nonSimplifedExpression);
    }
  }
  return m_secondInitialConditionLayout;
}

void Sequence::setFirstInitialConditionContent(const char * c) {
  strlcpy(m_firstInitialConditionText, c, sizeof(m_firstInitialConditionText));
  m_firstInitialConditionExpression = Expression();
  m_firstInitialConditionLayout = Layout();
}

void Sequence::setSecondInitialConditionContent(const char * c) {
  strlcpy(m_secondInitialConditionText, c, sizeof(m_secondInitialConditionText));
  m_secondInitialConditionExpression = Expression();
  m_secondInitialConditionLayout = Layout();
}

int Sequence::numberOfElements() {
  return (int)m_type + 1;
}

Poincare::Layout Sequence::nameLayout() {
  if (m_nameLayout.isUninitialized()) {
    m_nameLayout = HorizontalLayout(
        CharLayout(name()[0], KDFont::SmallFont),
        VerticalOffsetLayout(CharLayout('n', KDFont::SmallFont), VerticalOffsetLayoutNode::Type::Subscript)
      );
  }
  return m_nameLayout;
}

Poincare::Layout Sequence::definitionName() {
  if (m_definitionName.isUninitialized()) {
    if (m_type == Type::Explicit) {
      m_definitionName = HorizontalLayout(
        CharLayout(name()[0], KDFont::LargeFont),
        VerticalOffsetLayout(LayoutHelper::String("n", 1, KDFont::LargeFont), VerticalOffsetLayoutNode::Type::Subscript)
      );
    }
    if (m_type == Type::SingleRecurrence) {
      m_definitionName = HorizontalLayout(
        CharLayout(name()[0], KDFont::LargeFont),
        VerticalOffsetLayout(LayoutHelper::String("n+1", 3, KDFont::LargeFont), VerticalOffsetLayoutNode::Type::Subscript)
      );
    }
    if (m_type == Type::DoubleRecurrence) {
      m_definitionName = HorizontalLayout(
        CharLayout(name()[0], KDFont::LargeFont),
        VerticalOffsetLayout(LayoutHelper::String("n+2", 3, KDFont::LargeFont), VerticalOffsetLayoutNode::Type::Subscript)
      );
    }
  }
  return m_definitionName;
}

Poincare::Layout Sequence::firstInitialConditionName() {
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(m_initialRank).serialize(buffer, k_initialRankNumberOfDigits+1);
  if (m_firstInitialConditionName.isUninitialized()
      && (m_type == Type::SingleRecurrence
       || m_type == Type::DoubleRecurrence))
  {
    Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer), KDFont::LargeFont);
    m_firstInitialConditionName = HorizontalLayout(
        CharLayout(name()[0], KDFont::LargeFont),
        VerticalOffsetLayout(indexLayout, VerticalOffsetLayoutNode::Type::Subscript)
      );
  }
  return m_firstInitialConditionName;
}

Poincare::Layout Sequence::secondInitialConditionName() {
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(m_initialRank+1).serialize(buffer, k_initialRankNumberOfDigits+1);
  if (m_secondInitialConditionName.isUninitialized()) {
    if (m_type == Type::DoubleRecurrence) {
      Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer), KDFont::LargeFont);
      m_secondInitialConditionName = HorizontalLayout(
        CharLayout(name()[0], KDFont::LargeFont),
        VerticalOffsetLayout(indexLayout, VerticalOffsetLayoutNode::Type::Subscript)
      );
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
  Poincare::Symbol vnSymbol("v(n)", 4);
  Poincare::Symbol vn1Symbol("v(n+1)", 6);
  Poincare::Symbol unSymbol("u(n)", 4);
  Poincare::Symbol un1Symbol("u(n+1)", 6);
  Preferences * preferences = Poincare::Preferences::sharedPreferences();
  switch (m_type) {
    case Type::Explicit:
    {
      ctx.setValueForSymbol(un, unSymbol);
      ctx.setValueForSymbol(vn, vnSymbol);
      return expression(sqctx).approximateWithValueForSymbol(symbol(), (T)n, ctx, preferences->angleUnit());
    }
    case Type::SingleRecurrence:
    {
      if (n == m_initialRank) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpression(sqctx), *sqctx);
      }
      ctx.setValueForSymbol(un, un1Symbol);
      ctx.setValueForSymbol(unm1, unSymbol);
      ctx.setValueForSymbol(vn, vn1Symbol);
      ctx.setValueForSymbol(vnm1, vnSymbol);
      return expression(sqctx).approximateWithValueForSymbol(symbol(), (T)(n-1), ctx, preferences->angleUnit());
    }
    default:
    {
      if (n == m_initialRank) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpression(sqctx), *sqctx);
      }
      if (n == m_initialRank+1) {
        return PoincareHelpers::ApproximateToScalar<T>(secondInitialConditionExpression(sqctx), *sqctx);
      }
      ctx.setValueForSymbol(unm1, un1Symbol);
      ctx.setValueForSymbol(unm2, unSymbol);
      ctx.setValueForSymbol(vnm1, vn1Symbol);
      ctx.setValueForSymbol(vnm2, vnSymbol);
      return expression(sqctx).approximateWithValueForSymbol(symbol(), (T)(n-2), ctx, preferences->angleUnit());
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
  m_firstInitialConditionLayout = Layout();
  m_secondInitialConditionLayout = Layout();
  m_firstInitialConditionExpression = Expression();
  m_secondInitialConditionExpression = Expression();
  m_nameLayout = Layout();
  m_definitionName = Layout();
  m_firstInitialConditionName = Layout();
  m_secondInitialConditionName = Layout();
}

template double Sequence::templatedApproximateAtAbscissa<double>(double, SequenceContext*) const;
template float Sequence::templatedApproximateAtAbscissa<float>(float, SequenceContext*) const;
template double Sequence::approximateToNextRank<double>(int, SequenceContext*) const;
template float Sequence::approximateToNextRank<float>(int, SequenceContext*) const;
}
