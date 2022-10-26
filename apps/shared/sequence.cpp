#include "sequence.h"
#include "sequence_cache_context.h"
#include "sequence_store.h"
#include "../shared/poincare_helpers.h"
#include <apps/i18n.h>
#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/sequence.h>
#include <poincare/serialization_helper.h>
#include <poincare/sum.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/zoom.h>
#include <string.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

I18n::Message Sequence::parameterMessageName() const {
  return I18n::Message::N;
}

int Sequence::nameWithArgument(char * buffer, size_t bufferSize) {
  int seqNameSize = name(buffer, bufferSize);
  assert(seqNameSize > 0);
  size_t result = seqNameSize;
  assert(result <= bufferSize);
  buffer[result++] = '(';
  assert(result <= bufferSize);
  assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
  result += UTF8Decoder::CodePointToChars(symbol(), buffer+result, bufferSize-result);
  assert(result <= bufferSize);
  result += strlcpy(buffer+result, ")", bufferSize-result);
  return result;
}

int Sequence::nameWithArgumentAndType(char * buffer, size_t bufferSize) {
  int result = nameWithArgument(buffer, bufferSize);
  assert(result >= 1);
  int offset = result - 1;
  switch (type())
  {
  case Type::SingleRecurrence:
    result += strlcpy(buffer+offset, "+1)", bufferSize-offset);
    break;
  case Type::DoubleRecurrence:
    result += strlcpy(buffer+offset, "+2)", bufferSize-offset);
    break;
  default:
    break;
  }
  return result;
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
  recordData()->setType(t);
  m_definition.tidyName();
  tidyDownstreamPoolFrom();
  /* Reset all contents */
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  switch (t) {
    case Type::Explicit:
      error = setContent("", nullptr); // No context needed here
      break;
    case Type::SingleRecurrence:
    {
      char ex[] = "u{n}";
      /* Maybe in the future sequence names will be longer and this
      * code won't be valid anymore. This assert is to ensure that this code is
      * changed if it's the case.
      * */
      assert(fullName()[1] == 0 || fullName()[1] == '.');
      ex[0] = fullName()[0];
      error = setContent(ex, nullptr); // No context needed here
      break;
    }
    case Type::DoubleRecurrence:
    {
      char ex[] = "u{n+1}+u{n}";
      /* Maybe in the future sequence names will be longer and this
      * code won't be valid anymore. This assert is to ensure that this code is
      * changed if it's the case.
      * */
      assert(fullName()[1] == 0 || fullName()[1] == '.');
      char name = fullName()[0];
      ex[0] = name;
      ex[7] = name;
      error = setContent(ex, nullptr); // No context needed here
      break;
    }
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  (void) error; // Silence compilation warning
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
      CodePointLayout::Builder(fullName()[0]),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(symbol()), VerticalOffsetLayoutNode::VerticalPosition::Subscript)
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

bool Sequence::isSimplyRecursive(Context * context) {
  constexpr size_t bufferSize = SequenceStore::k_maxSequenceNameLength + 1;
  char buffer[bufferSize];
  name(buffer, bufferSize);
  return type() == Shared::Sequence::Type::SingleRecurrence && !expressionClone().recursivelyMatches([](const Expression e, Context * context, void * arg) {
    if (e.type() == ExpressionNode::Type::Symbol) {
      const Poincare::Symbol symbol = static_cast<const Poincare::Symbol&>(e);
      return symbol.isSystemSymbol() ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
    }
    if (e.type() != ExpressionNode::Type::Sequence) {
      return TrinaryBoolean::Unknown;
    }
    const Poincare::Sequence seq = static_cast<const Poincare::Sequence&>(e);
    char * buffer = static_cast<char*>(arg);
    if (strcmp(seq.name(), buffer) != 0) {
      return TrinaryBoolean::True;
    }
    if (seq.childAtIndex(0).type() != ExpressionNode::Type::Symbol) {
      return TrinaryBoolean::True;
    }
    Expression child = seq.childAtIndex(0);
    const Poincare::Symbol symbol = static_cast<const Poincare::Symbol&>(child);
    return symbol.isSystemSymbol() ? TrinaryBoolean::False : TrinaryBoolean::True;
  }, context, ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, static_cast<void*>(buffer));
}

void Sequence::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  model()->tidyDownstreamPoolFrom(treePoolCursor);
  m_firstInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
  m_secondInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
}

template<typename T>
T Sequence::templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const {
  T n = std::round(x);
  int sequenceIndex = SequenceStore::sequenceIndexForName(fullName()[0]);
  if (sqctx->iterateUntilRank<T>(n)) {
    return sqctx->valueOfCommonRankSequenceAtPreviousRank<T>(sequenceIndex, 0);
  }
  return NAN;
}

template<typename T>
T Sequence::valueAtRank(int n, SequenceContext *sqctx) {
  if (n < 0) {
    return NAN;
  }
  int sequenceIndex = SequenceStore::sequenceIndexForName(fullName()[0]);
  if (sqctx->independentSequenceRank<T>(sequenceIndex) > n || sqctx->independentSequenceRank<T>(sequenceIndex) < 0) {
    // Reset cache indexes and cache values
    sqctx->setIndependentSequenceRank<T>(-1, sequenceIndex);
    for (int i = 0 ; i < SequenceStore::k_maxRecurrenceDepth+1; i++) {
      sqctx->setIndependentSequenceValue<T>(NAN, sequenceIndex, i);
    }
  }
  while(sqctx->independentSequenceRank<T>(sequenceIndex) < n) {
    sqctx->stepSequenceAtIndex<T>(sequenceIndex);
  }
  /* In case we have sqctx->independentSequenceRank<T>(sequenceIndex) = n, we can return the
   * value */
  T value = sqctx->independentSequenceValue<T>(sequenceIndex, 0);
  return value;
}

template<typename T>
T Sequence::approximateToNextRank(int n, SequenceContext * sqctx, int sequenceIndex) const {
  if (n < initialRank() || n < 0) {
    return NAN;
  }

  SequenceCacheContext<T> ctx = SequenceCacheContext<T>(sqctx, sequenceIndex);
  // Hold values u(n), u(n-1), u(n-2), v(n), v(n-1), v(n-2)...
  T values[SequenceStore::k_maxNumberOfSequences][SequenceStore::k_maxRecurrenceDepth+1];

  /* In case we step only one sequence to the next step, the data stored in
   * values is not necessarily u(n), u(n-1).... Indeed, since the indexes are
   * independent, if the index for u is 3 but the one for v is 5, value will
   * hold u(3), u(2), u(1) | v(5), v(4), v(3). Therefore, the calculation will
   * be wrong if they relay on a symbol such as u(n). To prevent this, we align
   * all the values around the index of the sequence we are stepping. */
  int independentRank = sqctx->independentSequenceRank<T>(sequenceIndex);
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    if (sequenceIndex != -1 && sqctx->independentSequenceRank<T>(i) != independentRank) {
      int offset = independentRank - sqctx->independentSequenceRank<T>(i);
      if (offset != 0) {
        for (int j = SequenceStore::k_maxRecurrenceDepth; j >= 0; j--) {
            values[i][j] = j-offset < 0 || j-offset > SequenceStore::k_maxRecurrenceDepth ? NAN : sqctx->independentSequenceValue<T>(i, j-offset);
        }
      }
    } else {
      for (int j = 0; j < SequenceStore::k_maxRecurrenceDepth+1; j++) {
        values[i][j] = sequenceIndex != -1 ? sqctx->independentSequenceValue<T>(i, j) : sqctx->valueOfCommonRankSequenceAtPreviousRank<T>(i, j);
      }
    }
  }
  // Hold symbols u(n), u(n+1), v(n), v(n+1), w(n), w(n+1)
  Poincare::Symbol symbols[SequenceStore::k_maxNumberOfSequences][SequenceStore::k_maxRecurrenceDepth];
  char name[SequenceStore::k_maxRecurrenceDepth][7] = {"0(n)","0(n+1)"};
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    for (int j = 0; j < SequenceStore::k_maxRecurrenceDepth; j++) {
      name[j][0] = SequenceStore::k_sequenceNames[i][0];
      symbols[i][j] = Symbol::Builder(name[j], strlen(name[j]));
    }
  }
  // Update angle unit and complex format
  Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormatAndAngleUnit(complexFormat(sqctx), angleUnit(sqctx));
  switch (type()) {
    case Type::Explicit:
    {
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n) for all sequences
        ctx.setValueForSymbol(values[i][0], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), k_unknownName, static_cast<T>(n), &ctx, &preferences, false);
    }
    case Type::SingleRecurrence:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateWithValueForSymbol(firstInitialConditionExpressionReduced(sqctx), k_unknownName, static_cast<T>(NAN), &ctx, &preferences, false);
      }
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-1) and u(n+1) = u(n) for all sequences
        ctx.setValueForSymbol(values[i][0], symbols[i][1]);
        ctx.setValueForSymbol(values[i][1], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), k_unknownName, static_cast<T>(n-1), &ctx, &preferences, false);
    }
    default:
    {
      if (n == initialRank()) {
        return PoincareHelpers::ApproximateWithValueForSymbol(firstInitialConditionExpressionReduced(sqctx), k_unknownName, static_cast<T>(NAN), &ctx, &preferences, false);
      }
      if (n == initialRank()+1) {
        return PoincareHelpers::ApproximateWithValueForSymbol(secondInitialConditionExpressionReduced(sqctx), k_unknownName, static_cast<T>(NAN), &ctx, &preferences, false);
      }
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-2) and u(n+1) = u(n-1) for all sequences
        ctx.setValueForSymbol(values[i][1], symbols[i][1]);
        ctx.setValueForSymbol(values[i][2], symbols[i][0]);
      }
      return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(sqctx), k_unknownName, static_cast<T>(n-2), &ctx, &preferences, false);
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

void Sequence::xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context *) const {
  *xMin = static_cast<float>(initialRank());
  *xMax = *xMin + Zoom::k_defaultHalfRange;
  *yMinIntrinsic = FLT_MAX;
  *yMaxIntrinsic = -FLT_MAX;
}

void Sequence::yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context, bool optimizeRange) const {
  protectedFullRangeForDisplay(xMin, xMax, 1.f, yMin, yMax, context, false);
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

void Sequence::SequenceModel::tidyName(char * treePoolCursor) const {
  if (treePoolCursor == nullptr || m_name.isDownstreamOf(treePoolCursor)) {
    m_name = Poincare::Layout();
  }
}

void Sequence::SequenceModel::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  tidyName(treePoolCursor);
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
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
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n", 1), VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  } else if (sequence->type() == Type::SingleRecurrence) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+1", 3), VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  } else {
    assert(sequence->type() == Type::DoubleRecurrence);
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(LayoutHelper::String("n+2", 3), VerticalOffsetLayoutNode::VerticalPosition::Subscript));
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
  Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer));
  m_name = HorizontalLayout::Builder(
      CodePointLayout::Builder(sequence->fullName()[0]),
      VerticalOffsetLayout::Builder(indexLayout, VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

template double Sequence::templatedApproximateAtAbscissa<double>(double, SequenceContext*) const;
template float Sequence::templatedApproximateAtAbscissa<float>(float, SequenceContext*) const;
template double Sequence::approximateToNextRank<double>(int, SequenceContext*, int) const;
template float Sequence::approximateToNextRank<float>(int, SequenceContext*, int) const;
template double Sequence::valueAtRank<double>(int, SequenceContext *);
template float Sequence::valueAtRank<float>(int, SequenceContext *);

}
