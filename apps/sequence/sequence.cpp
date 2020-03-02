#include "sequence.h"
#include "cache_context.h"
#include "sequence_store.h"
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/sum.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/integer.h>
#include "../shared/poincare_helpers.h"
#include <string.h>
#include <apps/i18n.h>
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

I18n::Message Sequence::parameterMessageName() const {
  return I18n::Message::N;
}

void Sequence::tidy() {
  m_definition.tidyName();
  Function::tidy(); // m_definitionName.tidy()
  m_firstInitialCondition.tidy();
  m_firstInitialCondition.tidyName();
  m_secondInitialCondition.tidy();
  m_secondInitialCondition.tidyName();
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
      setContent("", nullptr); // No context needed here
      break;
    case Type::SingleRecurrence:
    {
      char ex[5] = "u(n)";
      ex[0] = fullName()[0];
      setContent(ex, nullptr); // No context needed here
      break;
    }
    case Type::DoubleRecurrence:
    {
      char ex[12] = "u(n+1)+u(n)";
      char name = fullName()[0];
      ex[0] = name;
      ex[7] = name;
      setContent(ex, nullptr); // No context needed here
      break;
    }
  }
  setFirstInitialConditionContent("", nullptr); // No context needed here
  setSecondInitialConditionContent("", nullptr); // No context needed here
}

void Sequence::setInitialRank(int rank) {
  recordData()->setInitialRank(rank);
  m_firstInitialCondition.tidyName();
  m_secondInitialCondition.tidyName();
}

Poincare::Layout Sequence::nameLayout() {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(fullName()[0], KDFont::SmallFont),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(symbol(), KDFont::SmallFont), VerticalOffsetLayoutNode::Position::Subscript)
    );
}

bool Sequence::isDefined() {
  RecordDataBuffer * data = recordData();
  switch (type()) {
    case Type::Explicit:
      return value().size > metaDataSize();
    case Type::SingleRecurrence:
      return data->initialConditionSize(0) > 0 && value().size > metaDataSize() + data->initialConditionSize(0);
    default:
      return data->initialConditionSize(0) > 0 && data->initialConditionSize(1) > 0 && value().size > metaDataSize() + data->initialConditionSize(0) + data->initialConditionSize(1);
  }
}

bool Sequence::isEmpty() {
  RecordDataBuffer * data = recordData();
  Type type = data->type();
  return Function::isEmpty() &&
    (type == Type::Explicit ||
      (data->initialConditionSize(0) == 0 &&
        (type == Type::SingleRecurrence || data->initialConditionSize(1) == 0)));
}

template<typename T>
T Sequence::templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const {
  T n = std::round(x);
  int sequenceIndex = SequenceStore::sequenceIndexForName(fullName()[0]);
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

  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownN[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknownN, bufferSize, UCodePointUnknown);

  CacheContext<T> ctx = CacheContext<T>(sqctx);
  // Hold values u(n), u(n-1), u(n-2), v(n), v(n-1), v(n-2)...
  T values[MaxNumberOfSequences][MaxRecurrenceDepth+1];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    for (int j = 0; j < MaxRecurrenceDepth+1; j++) {
      values[i][j] = sqctx->valueOfSequenceAtPreviousRank<T>(i, j);
    }
  }
  // Hold symbols u(n), u(n+1), v(n), v(n+1), w(n), w(n+1)
  Poincare::Symbol symbols[MaxNumberOfSequences][MaxRecurrenceDepth];
  char name[MaxRecurrenceDepth][7] = {"0(n)","0(n+1)"};
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    for (int j = 0; j < MaxRecurrenceDepth; j++) {
      name[j][0] = SequenceStore::k_sequenceNames[i][0];
      symbols[i][j] = Symbol::Builder(name[j], strlen(name[j]));
    }
  }

  switch (type()) {
    case Type::Explicit:
    {
      for (int i = 0; i < MaxNumberOfSequences; i++) {
        // Set in context u(n) = u(n) for all sequences
        ctx.setValueForSymbol(values[i][0], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)n, &ctx);
    }
    case Type::SingleRecurrence:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpressionReduced(sqctx), sqctx);
      }
      for (int i = 0; i < MaxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-1) and u(n+1) = u(n) for all sequences
        ctx.setValueForSymbol(values[i][0], symbols[i][1]);
        ctx.setValueForSymbol(values[i][1], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)(n-1), &ctx);
    }
    default:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateToScalar<T>(firstInitialConditionExpressionReduced(sqctx), sqctx);
      }
      if (n == initialRank()+1) {
        return PoincareHelpers::ApproximateToScalar<T>(secondInitialConditionExpressionReduced(sqctx), sqctx);
      }
      for (int i = 0; i < MaxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-2) and u(n+1) = u(n-1) for all sequences
        ctx.setValueForSymbol(values[i][1], symbols[i][1]);
        ctx.setValueForSymbol(values[i][2], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), unknownN, (T)(n-2), &ctx);
    }
  }
}

Expression Sequence::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  /* Here, we cannot just create the expression sum(u(n), start, end) because
   * the approximation of u(n) is not handled by Poincare (but only by
   * Sequence). */
  double result = 0.0;
  if (end-start > ExpressionNode::k_maxNumberOfSteps || start + 1.0 == start) {
    return Float<double>::Builder(NAN);
  }
  start = std::round(start);
  end = std::round(end);
  for (double i = start; i <= end; i = i + 1.0) {
    /* When |start| >> 1.0, start + 1.0 = start. In that case, quit the
     * infinite loop. */
    if (i == i-1.0 || i == i+1.0) {
      return Float<double>::Builder(NAN);
    }
    result += evaluateXYAtParameter(i, context).x2();
  }
  return Float<double>::Builder(result);
}

Sequence::RecordDataBuffer * Sequence::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

/* Sequence Model */

Poincare::Layout Sequence::SequenceModel::name(Sequence * sequence) {
  if (m_name.isUninitialized()) {
    buildName(sequence);
  }
  return m_name;
}

void Sequence::SequenceModel::updateNewDataWithExpression(Ion::Storage::Record * record, const Expression & expressionToStore, void * expressionAddress, size_t newExpressionSize, size_t previousExpressionSize) {
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

void * Sequence::DefinitionModel::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(RecordDataBuffer);
}

size_t Sequence::DefinitionModel::expressionSize(const Ion::Storage::Record * record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer * dataBuffer = static_cast<const Sequence *>(record)->recordData();
  return data.size-sizeof(RecordDataBuffer) - dataBuffer->initialConditionSize(0) - dataBuffer->initialConditionSize(1);
}

void Sequence::DefinitionModel::buildName(Sequence * sequence) {
  char name = sequence->fullName()[0];
  if (sequence->type() == Type::Explicit) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n", 1, k_layoutFont), VerticalOffsetLayoutNode::Position::Subscript));
  } else if (sequence->type() == Type::SingleRecurrence) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+1", 3, k_layoutFont), VerticalOffsetLayoutNode::Position::Subscript));
  } else {
    assert(sequence->type() == Type::DoubleRecurrence);
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name, k_layoutFont),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+2", 3, k_layoutFont), VerticalOffsetLayoutNode::Position::Subscript));
  }
}

/* Initial Condition Handle*/

void * Sequence::InitialConditionModel::expressionAddress(const Ion::Storage::Record * record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer * dataBuffer = static_cast<const Sequence *>(record)->recordData();
  size_t offset = conditionIndex() == 0 ? data.size - dataBuffer->initialConditionSize(0) - dataBuffer->initialConditionSize(1) : data.size - dataBuffer->initialConditionSize(1) ;
  return (char *)data.buffer+offset;
}

size_t Sequence::InitialConditionModel::expressionSize(const Ion::Storage::Record * record) const {
  return static_cast<const Sequence *>(record)->recordData()->initialConditionSize(conditionIndex());
}

void Sequence::InitialConditionModel::updateMetaData(const Ion::Storage::Record * record, size_t newSize) {
  static_cast<const Sequence *>(record)->recordData()->setInitialConditionSize(newSize, conditionIndex());
}

void Sequence::InitialConditionModel::buildName(Sequence * sequence) {
  assert((conditionIndex() == 0 && sequence->type() == Type::SingleRecurrence) || sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits+1];
  Integer(sequence->initialRank()+conditionIndex()).serialize(buffer, k_initialRankNumberOfDigits+1);
  Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer), k_layoutFont);
  m_name = HorizontalLayout::Builder(
      CodePointLayout::Builder(sequence->fullName()[0], k_layoutFont),
      VerticalOffsetLayout::Builder(indexLayout, VerticalOffsetLayoutNode::Position::Subscript));
}

template double Sequence::templatedApproximateAtAbscissa<double>(double, SequenceContext*) const;
template float Sequence::templatedApproximateAtAbscissa<float>(float, SequenceContext*) const;
template double Sequence::approximateToNextRank<double>(int, SequenceContext*) const;
template float Sequence::approximateToNextRank<float>(int, SequenceContext*) const;

}
