#include "sequence.h"
#include "cache_context.h"
#include "sequence_store.h"
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/integer.h>
#include "../shared/poincare_helpers.h"
#include <string.h>
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

void Sequence::tidy() {
  m_definitionHandle.tidyName();
  StorageFunction::tidy(); // m_definitionName.tidy()
  m_firstInitialConditionHandle.tidy();
  m_firstInitialConditionHandle.tidyName();
  m_secondInitialConditionHandle.tidy();
  m_secondInitialConditionHandle.tidyName();
  m_nameLayout = Layout();
}

Sequence::Type Sequence::type() const {
  return recordData()->type();
}

int Sequence::initialRank() const {
  return recordData()->initialRank();
}

void Sequence::setType(Type t) {
  if (t == type()) {
    return;
  }
  if (type() == Type::Explicit) {
    setInitialRank(0);
  }
  recordData()->setType(t);
  tidy();
  /* Reset all contents */
  switch (t) {
    case Type::Explicit:
      setContent("");
      break;
    case Type::SingleRecurrence:
    {
      char ex[5] = "u(n)";
      ex[0] = fullName()[0];
      setContent(ex);
      break;
    }
    case Type::DoubleRecurrence:
    {
      char ex[12] = "u(n+1)+u(n)";
      char name = fullName()[0];
      ex[0] = name;
      ex[7] = name;
      setContent(ex);
      break;
    }
  }
  setFirstInitialConditionContent("");
  setSecondInitialConditionContent("");
}

void Sequence::setInitialRank(int rank) {
  recordData()->setInitialRank(rank);
  m_firstInitialConditionHandle.tidyName();
  m_secondInitialConditionHandle.tidyName();
}

Poincare::Layout Sequence::nameLayout() {
  if (m_nameLayout.isUninitialized()) {
    m_nameLayout = HorizontalLayout::Builder(
        CodePointLayout::Builder(fullName()[0], KDFont::SmallFont),
        VerticalOffsetLayout::Builder(CodePointLayout::Builder(Symbol(), KDFont::SmallFont), VerticalOffsetLayoutNode::Type::Subscript)
      );
  }
  return m_nameLayout;
}

bool Sequence::isDefined() {
  SequenceRecordData * data = recordData();
  switch (type()) {
    case Type::Explicit:
      return StorageFunction::isDefined();
    case Type::SingleRecurrence:
      return StorageFunction::isDefined() && data->firstInitialConditionSize() > 0;
    default:
      return StorageFunction::isDefined() && data->firstInitialConditionSize() > 0 && data->secondInitialConditionSize() > 0;
  }
}

bool Sequence::isEmpty() {
  SequenceRecordData * data = recordData();
  switch (type()) {
    case Type::Explicit:
      return StorageFunction::isEmpty();
    case Type::SingleRecurrence:
      return StorageFunction::isEmpty() && data->firstInitialConditionSize() == 0;
    default:
      return StorageFunction::isEmpty() && data->firstInitialConditionSize() == 0 && data->secondInitialConditionSize() == 0;
  }
}

template<typename T>
T Sequence::templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const {
  T n = std::round(x);
  int sequenceIndex = fullName()[0] == SequenceStore::k_sequenceNames[0][0] ? 0 : 1;
  if (sqctx->iterateUntilRank<T>(n)) {
    return sqctx->valueOfSequenceAtPreviousRank<T>(sequenceIndex, 0);
  }
  return NAN;
}

template<typename T>
T Sequence::approximateToNextRank(int n, SequenceContext * sqctx) const {
  if (n < initialRank() || n < 0) {
    return NAN;
  }
  const char unknownN[2] = {Poincare::Symbol::SpecialSymbols::UnknownN, 0};
  CacheContext<T> ctx = CacheContext<T>(sqctx);
  T un = sqctx->valueOfSequenceAtPreviousRank<T>(0, 0);
  T unm1 = sqctx->valueOfSequenceAtPreviousRank<T>(0, 1);
  T unm2 = sqctx->valueOfSequenceAtPreviousRank<T>(0, 2);
  T vn = sqctx->valueOfSequenceAtPreviousRank<T>(1, 0);
  T vnm1 = sqctx->valueOfSequenceAtPreviousRank<T>(1, 1);
  T vnm2 = sqctx->valueOfSequenceAtPreviousRank<T>(1, 2);
  Poincare::Symbol vnSymbol = Symbol::Builder("v(n)", 4);
  Poincare::Symbol vn1Symbol = Symbol::Builder("v(n+1)", 6);
  Poincare::Symbol unSymbol = Symbol::Builder("u(n)", 4);
  Poincare::Symbol un1Symbol = Symbol::Builder("u(n+1)", 6);
  switch (type()) {
    case Type::Explicit:
    {
      ctx.setValueForSymbol(un, unSymbol);
      ctx.setValueForSymbol(vn, vnSymbol);
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)n, ctx);
    }
    case Type::SingleRecurrence:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpressionReduced(sqctx), *sqctx);
      }
      ctx.setValueForSymbol(un, un1Symbol);
      ctx.setValueForSymbol(unm1, unSymbol);
      ctx.setValueForSymbol(vn, vn1Symbol);
      ctx.setValueForSymbol(vnm1, vnSymbol);
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)(n-1), ctx);
    }
    default:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpressionReduced(sqctx), *sqctx);
      }
      if (n == initialRank()+1) {
        return PoincareHelpers::ApproximateToScalar<T>(secondInitialConditionExpressionReduced(sqctx), *sqctx);
      }
      ctx.setValueForSymbol(unm1, un1Symbol);
      ctx.setValueForSymbol(unm2, unSymbol);
      ctx.setValueForSymbol(vnm1, vn1Symbol);
      ctx.setValueForSymbol(vnm2, vnSymbol);
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)(n-2), ctx);
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

Sequence::SequenceRecordData * Sequence::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<SequenceRecordData *>(const_cast<void *>(d.buffer));
}

/* Sequence Handle */

Poincare::Layout Sequence::SequenceHandle::name(Sequence * sequence) {
  if (m_name.isUninitialized()) {
    buildName(sequence);
  }
  return m_name;
}

void Sequence::SequenceHandle::updateNewDataWithExpression(Ion::Storage::Record * record, Expression & expressionToStore, void * expressionAddress, size_t newExpressionSize, size_t previousExpressionSize) {
  Ion::Storage::Record::Data newData = record->value();
  // Translate expressions located downstream
  size_t sizeBeforeExpression = (char *)expressionAddress -(char *)newData.buffer;
  size_t remainingSize = newData.size - sizeBeforeExpression - previousExpressionSize;
  memmove((char *)expressionAddress + newExpressionSize, (char *)expressionAddress + previousExpressionSize, remainingSize);
  // Copy the expression
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(), newExpressionSize);
  }
  // Update meta data
  updateMetaData(record, newExpressionSize);
}

/* Definition Handle*/

void * Sequence::DefinitionHandle::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(SequenceRecordData);
}

size_t Sequence::DefinitionHandle::expressionSize(const Ion::Storage::Record * record) const {
  Ion::Storage::Record::Data data = record->value();
  SequenceRecordData * dataBuffer = static_cast<const Sequence *>(record)->recordData();
  return data.size-sizeof(SequenceRecordData) - dataBuffer->firstInitialConditionSize() - dataBuffer->secondInitialConditionSize();
}

void Sequence::DefinitionHandle::buildName(Sequence * sequence) {
  char name = sequence->fullName()[0];
  if (sequence->type() == Type::Explicit) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n", 1, k_layoutFont), VerticalOffsetLayoutNode::Type::Subscript));
  } else if (sequence->type() == Type::SingleRecurrence) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+1", 3, k_layoutFont), VerticalOffsetLayoutNode::Type::Subscript));
  } else {
    assert(sequence->type() == Type::DoubleRecurrence);
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+2", 3, k_layoutFont), VerticalOffsetLayoutNode::Type::Subscript));
  }
}

/* First Initial Condition Handle*/

void * Sequence::FirstInitialConditionHandle::expressionAddress(const Ion::Storage::Record * record) const {
  Ion::Storage::Record::Data data = record->value();
  SequenceRecordData * dataBuffer = static_cast<const Sequence *>(record)->recordData();
  size_t offset = data.size - dataBuffer->firstInitialConditionSize() - dataBuffer->secondInitialConditionSize();
  return (char *)data.buffer+offset;
}

size_t Sequence::FirstInitialConditionHandle::expressionSize(const Ion::Storage::Record * record) const {
  return static_cast<const Sequence *>(record)->recordData()->firstInitialConditionSize();
}

void Sequence::FirstInitialConditionHandle::updateMetaData(const Ion::Storage::Record * record, size_t newSize) {
  static_cast<const Sequence *>(record)->recordData()->setFirstInitialConditionSize(newSize);
}

void Sequence::FirstInitialConditionHandle::buildName(Sequence * sequence) {
  assert(sequence->type() == Type::SingleRecurrence || sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(sequence->initialRank()).serialize(buffer, k_initialRankNumberOfDigits+1);
  Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer), k_layoutFont);
  m_name = HorizontalLayout::Builder(
      CodePointLayout::Builder(sequence->fullName()[0], k_layoutFont),
      VerticalOffsetLayout::Builder(indexLayout, VerticalOffsetLayoutNode::Type::Subscript));
}

/* Second Initial Condition Handle*/

void * Sequence::SecondInitialConditionHandle::expressionAddress(const Ion::Storage::Record * record) const {
  Ion::Storage::Record::Data data = record->value();
  SequenceRecordData * dataBuffer = static_cast<const Sequence *>(record)->recordData();
  size_t offset = data.size - dataBuffer->secondInitialConditionSize();
  return (char *)data.buffer+offset;
}

void Sequence::SecondInitialConditionHandle::updateMetaData(const Ion::Storage::Record * record, size_t newSize) {
  static_cast<const Sequence *>(record)->recordData()->setSecondInitialConditionSize(newSize);
}

size_t Sequence::SecondInitialConditionHandle::expressionSize(const Ion::Storage::Record * record) const {
  return static_cast<const Sequence *>(record)->recordData()->secondInitialConditionSize();
}

void Sequence::SecondInitialConditionHandle::buildName(Sequence * sequence) {
  assert(sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(sequence->initialRank()+1).serialize(buffer, k_initialRankNumberOfDigits+1);
  Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer), k_layoutFont);
  m_name = HorizontalLayout::Builder(
      CodePointLayout::Builder(sequence->fullName()[0], k_layoutFont),
      VerticalOffsetLayout::Builder(indexLayout, VerticalOffsetLayoutNode::Type::Subscript));
}

template double Sequence::templatedApproximateAtAbscissa<double>(double, SequenceContext*) const;
template float Sequence::templatedApproximateAtAbscissa<float>(float, SequenceContext*) const;
template double Sequence::approximateToNextRank<double>(int, SequenceContext*) const;
template float Sequence::approximateToNextRank<float>(int, SequenceContext*) const;

}
