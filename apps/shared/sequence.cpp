#include "sequence.h"

#include <apps/i18n.h>
#include <float.h>
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

#include "../shared/poincare_helpers.h"
#include "sequence_cache_context.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

I18n::Message Sequence::parameterMessageName() const {
  return I18n::Message::N;
}

int Sequence::nameWithArgumentAndType(char *buffer, size_t bufferSize) {
  int result = nameWithArgument(buffer, bufferSize);
  assert(result >= 1);
  int offset = result - 1;
  switch (type()) {
    case Type::SingleRecurrence:
      result += strlcpy(buffer + offset, "+1)", bufferSize - offset);
      break;
    case Type::DoubleRecurrence:
      result += strlcpy(buffer + offset, "+2)", bufferSize - offset);
      break;
    default:
      break;
  }
  return result;
}

void Sequence::setType(Type t) {
  if (t == type()) {
    return;
  }
  recordData()->setType(t);
  m_definition.tidyName();
  tidyDownstreamPoolFrom();
  /* Reset all contents */
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  switch (t) {
    case Type::Explicit:
      error = setContent("", nullptr);  // No context needed here
      break;
    case Type::SingleRecurrence: {
      char ex[] = "u\u0014{n\u0014}";
      /* Maybe in the future sequence names will be longer and this
       * code won't be valid anymore. This assert is to ensure that this code is
       * changed if it's the case.
       * */
      assert(fullName()[1] == 0 || fullName()[1] == '.');
      ex[0] = fullName()[0];
      error = setContent(ex, nullptr);  // No context needed here
      break;
    }
    case Type::DoubleRecurrence: {
      char ex[] = "u\u0014{n+1\u0014}+u\u0014{n\u0014}";
      /* Maybe in the future sequence names will be longer and this
       * code won't be valid anymore. This assert is to ensure that this code is
       * changed if it's the case.
       * */
      assert(fullName()[1] == 0 || fullName()[1] == '.');
      constexpr int k_uNameSecondIndex = 9;
      assert(ex[k_uNameSecondIndex] == 'u');
      char name = fullName()[0];
      ex[0] = name;
      ex[k_uNameSecondIndex] = name;
      error = setContent(ex, nullptr);  // No context needed here
      break;
    }
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  (void)error;                                    // Silence compilation warning
  setFirstInitialConditionContent("", nullptr);   // No context needed here
  setSecondInitialConditionContent("", nullptr);  // No context needed here
}

void Sequence::setInitialRank(int rank) {
  recordData()->setInitialRank(rank);
  m_firstInitialCondition.tidyName();
  m_secondInitialCondition.tidyName();
}

Poincare::Layout Sequence::nameLayout() {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(fullName()[0]),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder(symbol()),
          VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

bool Sequence::isDefined() {
  RecordDataBuffer *data = recordData();
  switch (type()) {
    case Type::Explicit:
      return value().size > metaDataSize();
    case Type::SingleRecurrence:
      return data->initialConditionSize(0) > 0 &&
             value().size > metaDataSize() + data->initialConditionSize(0);
    default:
      return data->initialConditionSize(0) > 0 &&
             data->initialConditionSize(1) > 0 &&
             value().size > metaDataSize() + data->initialConditionSize(0) +
                                data->initialConditionSize(1);
  }
}

bool Sequence::isEmpty() {
  RecordDataBuffer *data = recordData();
  Type type = data->type();
  return Function::isEmpty() &&
         (type == Type::Explicit || (data->initialConditionSize(0) == 0 &&
                                     (type == Type::SingleRecurrence ||
                                      data->initialConditionSize(1) == 0)));
}

bool Sequence::isSimplyRecursive(Context *context) {
  constexpr size_t bufferSize = SequenceStore::k_maxSequenceNameLength + 1;
  char buffer[bufferSize];
  name(buffer, bufferSize);
  return type() == Shared::Sequence::Type::SingleRecurrence &&
         !expressionClone().recursivelyMatches(
             [](const Expression e, Context *context, void *arg) {
               if (e.type() == ExpressionNode::Type::Symbol) {
                 const Poincare::Symbol symbol =
                     static_cast<const Poincare::Symbol &>(e);
                 return symbol.isSystemSymbol() ? TrinaryBoolean::True
                                                : TrinaryBoolean::Unknown;
               }
               if (e.type() != ExpressionNode::Type::Sequence) {
                 return TrinaryBoolean::Unknown;
               }
               const Poincare::Sequence seq =
                   static_cast<const Poincare::Sequence &>(e);
               char *buffer = static_cast<char *>(arg);
               if (strcmp(seq.name(), buffer) != 0) {
                 return TrinaryBoolean::True;
               }
               if (seq.childAtIndex(0).type() != ExpressionNode::Type::Symbol) {
                 return TrinaryBoolean::True;
               }
               Expression child = seq.childAtIndex(0);
               const Poincare::Symbol symbol =
                   static_cast<const Poincare::Symbol &>(child);
               return symbol.isSystemSymbol() ? TrinaryBoolean::False
                                              : TrinaryBoolean::True;
             },
             context,
             SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
             static_cast<void *>(buffer));
}

void Sequence::tidyDownstreamPoolFrom(char *treePoolCursor) const {
  model()->tidyDownstreamPoolFrom(treePoolCursor);
  m_firstInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
  m_secondInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
}

template <typename T>
T Sequence::templatedApproximateAtAbscissa(T x, SequenceContext *sqctx) const {
  T n = std::round(x);
  int sequenceIndex = SequenceStore::sequenceIndexForName(fullName()[0]);
  if (sqctx->iterateUntilRank<T>(n)) {
    return sqctx->valueOfCommonRankSequenceAtPreviousRank<T>(sequenceIndex, 0);
  }
  return NAN;
}

template <typename T>
T Sequence::valueAtRank(int n, SequenceContext *sqctx) {
  if (n < 0) {
    return NAN;
  }
  int sequenceIndex = SequenceStore::sequenceIndexForName(fullName()[0]);
  if (sqctx->independentSequenceRank<T>(sequenceIndex) > n ||
      sqctx->independentSequenceRank<T>(sequenceIndex) < 0) {
    // Reset cache indexes and cache values
    sqctx->setIndependentSequenceRank<T>(-1, sequenceIndex);
    for (int i = 0; i < SequenceStore::k_maxRecurrenceDepth + 1; i++) {
      sqctx->setIndependentSequenceValue<T>(NAN, sequenceIndex, i);
    }
  }
  while (sqctx->independentSequenceRank<T>(sequenceIndex) < n) {
    sqctx->stepSequenceAtIndex<T>(sequenceIndex);
  }
  /* In case we have sqctx->independentSequenceRank<T>(sequenceIndex) = n, we
   * can return the value */
  T value = sqctx->independentSequenceValue<T>(sequenceIndex, 0);
  return value;
}

template <typename T>
T Sequence::approximateToNextRank(SequenceContext *sqctx,
                                  int sequenceIndex) const {
  int n = sequenceIndex == -1
              ? sqctx->commonRank<T>()
              : sqctx->independentSequenceRank<T>(sequenceIndex);
  if (n < initialRank() || n < 0) {
    return NAN;
  }
  SequenceCacheContext<T> ctx = SequenceCacheContext<T>(sqctx, sequenceIndex);

  /* Hold values = {{u(n), u(n-1), u(n-2)}, {v(n), v(n-1), v(n-2)}, {w(n),
   * w(n-1), w(n-2)}}
   * WARNING : values must be ordered by name index (u then v then w) because
   * SequenceCacheContext needs it. */
  T values[SequenceStore::k_maxNumberOfSequences]
          [SequenceStore::k_maxRecurrenceDepth + 1];
  /* In case we step only one sequence (sequenceIndex != -1), we use independant
   * values stored in SequenceContext (m_independentRankValues). However, these
   * values might not be aligned on the same rank. Thus, we align them all at
   * the rank of the sequence we are stepping. */
  int independentRank = sqctx->independentSequenceRank<T>(sequenceIndex);
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    if (sequenceIndex != -1 &&
        sqctx->independentSequenceRank<T>(i) != independentRank) {
      int offset = independentRank - sqctx->independentSequenceRank<T>(i);
      if (offset != 0) {
        for (int j = SequenceStore::k_maxRecurrenceDepth; j >= 0; j--) {
          values[i][j] =
              j - offset < 0 || j - offset > SequenceStore::k_maxRecurrenceDepth
                  ? NAN
                  : sqctx->independentSequenceValue<T>(i, j - offset);
        }
      }
    } else {
      for (int j = 0; j < SequenceStore::k_maxRecurrenceDepth + 1; j++) {
        values[i][j] =
            sequenceIndex != -1
                ? sqctx->independentSequenceValue<T>(i, j)
                : sqctx->valueOfCommonRankSequenceAtPreviousRank<T>(i, j);
      }
    }
  }

  // Update angle unit and complex format
  Preferences preferences =
      Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(sqctx));

  T x;
  Poincare::Expression e;

  switch (type()) {
    case Type::Explicit: {
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n) for all sequences
        ctx.setValue(values[i][0], i, 0);
      }
      x = static_cast<T>(n);
      e = expressionReduced(sqctx);
      break;
    }
    case Type::SingleRecurrence: {
      if (n == initialRank()) {
        x = static_cast<T>(NAN);
        e = firstInitialConditionExpressionReduced(sqctx);
        break;
      }
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-1) and u(n+1) = u(n) for all sequences
        ctx.setValue(values[i][0], i, 1);
        ctx.setValue(values[i][1], i, 0);
      }
      x = static_cast<T>(n - 1);
      e = expressionReduced(sqctx);
      break;
    }
    default: {
      assert(type() == Type::DoubleRecurrence);
      if (n == initialRank()) {
        x = static_cast<T>(NAN);
        e = firstInitialConditionExpressionReduced(sqctx);
        break;
      }
      if (n == initialRank() + 1) {
        x = static_cast<T>(NAN);
        e = secondInitialConditionExpressionReduced(sqctx);
        break;
      }
      for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
        // Set in context u(n) = u(n-2) and u(n+1) = u(n-1) for all sequences
        ctx.setValue(values[i][1], i, 1);
        ctx.setValue(values[i][2], i, 0);
      }
      x = static_cast<T>(n - 2);
      e = expressionReduced(sqctx);
      break;
    }
  }

  return PoincareHelpers::ApproximateWithValueForSymbol(
      e, k_unknownName, x, &ctx, &preferences, false);
}

Expression Sequence::sumBetweenBounds(double start, double end,
                                      Poincare::Context *context) const {
  /* Here, we cannot just create the expression sum(u(n), start, end) because
   * the approximation of u(n) is not handled by Poincare (but only by
   * Sequence). */
  double result = 0.0;
  if (end - start > ExpressionNode::k_maxNumberOfSteps ||
      start + 1.0 == start) {
    return Float<double>::Builder(NAN);
  }
  start = std::round(start);
  end = std::round(end);
  for (double i = start; i <= end; i = i + 1.0) {
    /* When |start| >> 1.0, start + 1.0 = start. In that case, quit the
     * infinite loop. */
    if (i == i - 1.0 || i == i + 1.0) {
      return Float<double>::Builder(NAN);
    }
    result += evaluateXYAtParameter(i, context).x2();
  }
  return Float<double>::Builder(result);
}

Sequence::RecordDataBuffer *Sequence::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

/* Sequence Model */

Poincare::Layout Sequence::SequenceModel::name(Sequence *sequence) {
  if (m_name.isUninitialized()) {
    buildName(sequence);
  }
  return m_name;
}

void Sequence::SequenceModel::tidyName(char *treePoolCursor) const {
  if (treePoolCursor == nullptr || m_name.isDownstreamOf(treePoolCursor)) {
    m_name = Poincare::Layout();
  }
}

void Sequence::SequenceModel::tidyDownstreamPoolFrom(
    char *treePoolCursor) const {
  tidyName(treePoolCursor);
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

void Sequence::SequenceModel::updateNewDataWithExpression(
    Ion::Storage::Record *record, const Expression &expressionToStore,
    void *expressionAddress, size_t newExpressionSize,
    size_t previousExpressionSize) {
  Ion::Storage::Record::Data newData = record->value();
  // Translate expressions located downstream
  size_t sizeBeforeExpression =
      (char *)expressionAddress - (char *)newData.buffer;
  size_t remainingSize =
      newData.size - sizeBeforeExpression - previousExpressionSize;
  memmove((char *)expressionAddress + newExpressionSize,
          (char *)expressionAddress + previousExpressionSize, remainingSize);
  // Copy the expression
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(),
            newExpressionSize);
  }
  // Update meta data
  updateMetaData(record, newExpressionSize);
}

/* Definition Handle*/

void *Sequence::DefinitionModel::expressionAddress(
    const Ion::Storage::Record *record) const {
  return (char *)record->value().buffer + sizeof(RecordDataBuffer);
}

size_t Sequence::DefinitionModel::expressionSize(
    const Ion::Storage::Record *record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer *dataBuffer =
      static_cast<const Sequence *>(record)->recordData();
  return data.size - sizeof(RecordDataBuffer) -
         dataBuffer->initialConditionSize(0) -
         dataBuffer->initialConditionSize(1);
}

void Sequence::DefinitionModel::buildName(Sequence *sequence) {
  char name = sequence->fullName()[0];
  if (sequence->type() == Type::Explicit) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(
            LayoutHelper::String("n", 1),
            VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  } else if (sequence->type() == Type::SingleRecurrence) {
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(
            LayoutHelper::String("n+1", 3),
            VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  } else {
    assert(sequence->type() == Type::DoubleRecurrence);
    m_name = HorizontalLayout::Builder(
        CodePointLayout::Builder(name),
        VerticalOffsetLayout::Builder(
            LayoutHelper::String("n+2", 3),
            VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  }
}

/* Initial Condition Handle*/

void *Sequence::InitialConditionModel::expressionAddress(
    const Ion::Storage::Record *record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer *dataBuffer =
      static_cast<const Sequence *>(record)->recordData();
  size_t offset = conditionIndex() == 0
                      ? data.size - dataBuffer->initialConditionSize(0) -
                            dataBuffer->initialConditionSize(1)
                      : data.size - dataBuffer->initialConditionSize(1);
  return (char *)data.buffer + offset;
}

size_t Sequence::InitialConditionModel::expressionSize(
    const Ion::Storage::Record *record) const {
  return static_cast<const Sequence *>(record)
      ->recordData()
      ->initialConditionSize(conditionIndex());
}

void Sequence::InitialConditionModel::updateMetaData(
    const Ion::Storage::Record *record, size_t newSize) {
  static_cast<const Sequence *>(record)->recordData()->setInitialConditionSize(
      newSize, conditionIndex());
}

void Sequence::InitialConditionModel::buildName(Sequence *sequence) {
  assert(
      (conditionIndex() == 0 && sequence->type() == Type::SingleRecurrence) ||
      sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits + 1];
  Integer(sequence->initialRank() + conditionIndex())
      .serialize(buffer, k_initialRankNumberOfDigits + 1);
  Layout indexLayout = LayoutHelper::String(buffer, strlen(buffer));
  m_name = HorizontalLayout::Builder(
      CodePointLayout::Builder(sequence->fullName()[0]),
      VerticalOffsetLayout::Builder(
          indexLayout, VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

template double Sequence::templatedApproximateAtAbscissa<double>(
    double, SequenceContext *) const;
template float Sequence::templatedApproximateAtAbscissa<float>(
    float, SequenceContext *) const;
template double Sequence::approximateToNextRank<double>(SequenceContext *,
                                                        int) const;
template float Sequence::approximateToNextRank<float>(SequenceContext *,
                                                      int) const;
template double Sequence::valueAtRank<double>(int, SequenceContext *);
template float Sequence::valueAtRank<float>(int, SequenceContext *);

}  // namespace Shared
