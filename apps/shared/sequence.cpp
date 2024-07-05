#include "sequence.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <float.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/numeric/zoom.h>
#include <poincare/old/addition.h>
#include <poincare/old/based_integer.h>
#include <poincare/old/rational.h>
#include <poincare/old/sequence.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sum.h>
#include <poincare/src/expression/sequence.h>
#include <string.h>

#include <cmath>

#include "global_context.h"
#include "sequence_context.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

I18n::Message Sequence::parameterMessageName() const {
  return I18n::Message::N;
}

size_t Sequence::nameWithArgumentAndType(char* buffer, size_t bufferSize) {
  size_t result = nameWithArgument(buffer, bufferSize);
  assert(result >= 1);
  size_t offset = result - 1;
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
      error = setExpressionContent(UserExpression());
      break;
    case Type::SingleRecurrence: {
      error = setExpressionContent(Poincare::Sequence::Builder(
          fullName(), 1, UserExpression::Builder(KUnknownSymbol)));
      break;
    }
    case Type::DoubleRecurrence: {
      error = setExpressionContent(Poincare::UserExpression::Create(
          KAdd(KA, KB),
          {.KA = Poincare::Sequence::Builder(
               fullName(), 1,
               UserExpression::Builder(KAdd(KUnknownSymbol, 1_e))),
           .KB = Poincare::Sequence::Builder(
               fullName(), 1, UserExpression::Builder(KUnknownSymbol))}));
      break;
    }
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  (void)error;  // Silence compilation warning
  // No context needed here
  setFirstInitialConditionContent(Poincare::Layout(), nullptr);
  setSecondInitialConditionContent(Poincare::Layout(), nullptr);
}

void Sequence::setInitialRank(int rank) {
  recordData()->setInitialRank(rank);
  m_firstInitialCondition.tidyName();
  m_secondInitialCondition.tidyName();
}

Layout Sequence::nameLayout() {
  return Layout::Create(KA ^ KSubscriptL(KB),
                        {.KA = Layout::CodePoint(fullName()[0]),
                         .KB = Layout::CodePoint(symbol())});
}

bool Sequence::isDefined() const {
  RecordDataBuffer* data = recordData();
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

bool Sequence::isEmpty() const {
  RecordDataBuffer* data = recordData();
  Type type = data->type();
  return Function::isEmpty() &&
         (type == Type::Explicit || (data->initialConditionSize(0) == 0 &&
                                     (type == Type::SingleRecurrence ||
                                      data->initialConditionSize(1) == 0)));
}

bool Sequence::isSuitableForCobweb(Context* context) const {
  return type() == Type::SingleRecurrence &&
         !std::isnan(approximateAtRank(
             initialRank(),
             reinterpret_cast<SequenceContext*>(context)->cache())) &&
         !mainExpressionContainsForbiddenTerms(context, true, false, false);
}

bool Sequence::mainExpressionContainsForbiddenTerms(
    Context* context, bool recursionIsAllowed, bool systemSymbolIsAllowed,
    bool otherSequencesAreAllowed) const {
  constexpr size_t bufferSize = SequenceStore::k_maxSequenceNameLength + 1;
  char buffer[bufferSize];
  name(buffer, bufferSize);
  // TODO_PCJ: used SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
  return Poincare::Internal::Sequence::MainExpressionContainsForbiddenTerms(
      expressionClone().tree(), buffer, type(), initialRank(),
      recursionIsAllowed, systemSymbolIsAllowed, otherSequencesAreAllowed);
}

void Sequence::tidyDownstreamPoolFrom(PoolObject* treePoolCursor) const {
  model()->tidyDownstreamPoolFrom(treePoolCursor);
  m_firstInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
  m_secondInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
}

template <typename T>
T Sequence::privateEvaluateYAtX(T x, Context* context) const {
  // Round behaviour changes platform-wise if std::isnan(x)
  assert(!std::isnan(x));
  int n = std::round(x);
  return static_cast<T>(approximateAtRank(
      n, reinterpret_cast<SequenceContext*>(context)->cache()));
}

double Sequence::approximateAtRank(int rank,
                                   Internal::SequenceCache* sqctx) const {
  int sequenceIndex = SequenceStore::SequenceIndexForName(fullName()[0]);
  if (!isDefined() || rank < initialRank() ||
      (rank >= firstNonInitialRank() &&
       sqctx->sequenceIsNotComputable(sequenceIndex))) {
    return NAN;
  }
  sqctx->stepUntilRank(sequenceIndex, rank);
  return sqctx->storedValueOfSequenceAtRank(sequenceIndex, rank);
}

double Sequence::approximateAtContextRank(SequenceContext* sqctx, int rank,
                                          bool intermediateComputation) const {
  if (rank < initialRank()) {
    return NAN;
  }
  double x;
  SystemExpression e;
  if (rank >= firstNonInitialRank()) {
    x = static_cast<double>(rank - order());
    e = expressionReduced(sqctx);
  } else {
    assert(type() != Type::Explicit);
    x = static_cast<double>(NAN);
    if (rank == initialRank()) {
      e = firstInitialConditionExpressionReduced(sqctx);
    } else {
      assert(type() == Type::DoubleRecurrence);
      e = secondInitialConditionExpressionReduced(sqctx);
    }
  }
  return e.approximateToScalarWithValue(x);
}

UserExpression Sequence::sumBetweenBounds(double start, double end,
                                          Context* context) const {
  /* Here, we cannot just create the expression sum(u(n), start, end) because
   * the approximation of u(n) is not handled by Poincare (but only by
   * Sequence). */
  double result = 0.0;
  if (end - start > ExpressionNode::k_maxNumberOfSteps ||
      start + 1.0 == start) {
    return NewExpression::Builder<double>(NAN);
  }
  start = std::round(start);
  end = std::round(end);
  for (double i = start; i <= end; i = i + 1.0) {
    /* When |start| >> 1.0, start + 1.0 = start. In that case, quit the
     * infinite loop. */
    if (i == i - 1.0 || i == i + 1.0) {
      return NewExpression::Builder<double>(NAN);
    }
    result += evaluateXYAtParameter(i, context).y();
  }
  return NewExpression::Builder<double>(result);
}

Sequence::RecordDataBuffer* Sequence::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer*>(const_cast<void*>(d.buffer));
}

/* Sequence Model */

Layout Sequence::SequenceModel::name(Sequence* sequence) {
  if (m_name.isUninitialized()) {
    buildName(sequence);
  }
  return m_name;
}

void Sequence::SequenceModel::tidyName(PoolObject* treePoolCursor) const {
  if (treePoolCursor == nullptr || m_name.isDownstreamOf(treePoolCursor)) {
    m_name = Layout();
  }
}

void Sequence::SequenceModel::tidyDownstreamPoolFrom(
    PoolObject* treePoolCursor) const {
  tidyName(treePoolCursor);
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

void Sequence::SequenceModel::updateNewDataWithExpression(
    Ion::Storage::Record* record, const UserExpression& expressionToStore,
    void* expressionAddress, size_t newExpressionSize,
    size_t previousExpressionSize) {
  Ion::Storage::Record::Data newData = record->value();
  // Translate expressions located downstream
  size_t sizeBeforeExpression =
      (char*)expressionAddress - (char*)newData.buffer;
  size_t remainingSize =
      newData.size - sizeBeforeExpression - previousExpressionSize;
  memmove((char*)expressionAddress + newExpressionSize,
          (char*)expressionAddress + previousExpressionSize, remainingSize);
  // Copy the expression
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(),
            newExpressionSize);
  }
  // Update meta data
  updateMetaData(record, newExpressionSize);
}

void Sequence::SequenceModel::setStorageChangeFlag() const {
  GlobalContext::sequenceStore->setStorageChangeFlag(true);
}

/* Definition Handle*/

void* Sequence::DefinitionModel::expressionAddress(
    const Ion::Storage::Record* record) const {
  return (char*)record->value().buffer + sizeof(RecordDataBuffer);
}

size_t Sequence::DefinitionModel::expressionSize(
    const Ion::Storage::Record* record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer* dataBuffer =
      static_cast<const Sequence*>(record)->recordData();
  return data.size - sizeof(RecordDataBuffer) -
         dataBuffer->initialConditionSize(0) -
         dataBuffer->initialConditionSize(1);
}

void Sequence::DefinitionModel::buildName(Sequence* sequence) {
  const char* index;
  if (sequence->type() == Type::Explicit) {
    index = "n";
  } else if (sequence->type() == Type::SingleRecurrence) {
    index = "n+1";
  } else {
    assert(sequence->type() == Type::DoubleRecurrence);
    index = "n+2";
  }
  m_name = Layout::Create(KA ^ KSubscriptL(KB),
                          {.KA = Layout::CodePoint(sequence->fullName()[0]),
                           .KB = Layout::String(index)});
}

/* Initial Condition Handle*/

void* Sequence::InitialConditionModel::expressionAddress(
    const Ion::Storage::Record* record) const {
  Ion::Storage::Record::Data data = record->value();
  RecordDataBuffer* dataBuffer =
      static_cast<const Sequence*>(record)->recordData();
  size_t offset = conditionIndex() == 0
                      ? data.size - dataBuffer->initialConditionSize(0) -
                            dataBuffer->initialConditionSize(1)
                      : data.size - dataBuffer->initialConditionSize(1);
  return (char*)data.buffer + offset;
}

size_t Sequence::InitialConditionModel::expressionSize(
    const Ion::Storage::Record* record) const {
  return static_cast<const Sequence*>(record)
      ->recordData()
      ->initialConditionSize(conditionIndex());
}

void Sequence::InitialConditionModel::updateMetaData(
    const Ion::Storage::Record* record, size_t newSize) {
  static_cast<const Sequence*>(record)->recordData()->setInitialConditionSize(
      newSize, conditionIndex());
}

void Sequence::InitialConditionModel::buildName(Sequence* sequence) {
  assert(
      (conditionIndex() == 0 && sequence->type() == Type::SingleRecurrence) ||
      sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits + 1];
  Integer(sequence->initialRank() + conditionIndex())
      .serialize(buffer, k_initialRankNumberOfDigits + 1);
  m_name = Layout::Create(KA ^ KSubscriptL(KB),
                          {.KA = Layout::CodePoint(sequence->fullName()[0]),
                           .KB = Layout::String(buffer)});
}

template double Sequence::privateEvaluateYAtX<double>(double, Context*) const;
template float Sequence::privateEvaluateYAtX<float>(float, Context*) const;

}  // namespace Shared
