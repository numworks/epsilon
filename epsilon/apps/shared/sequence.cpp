#include "sequence.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <float.h>
#include <omg/print.h>
#include <omg/utf8_helper.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <string.h>

#include <cmath>

#include "global_store.h"
#include "sequence_context.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

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
  char name[SymbolHelper::k_maxNameSize];
  nameWithoutExtension(name, SymbolHelper::k_maxNameSize);
  switch (t) {
    case Type::Explicit:
      error = setExpressionContent(UserExpression());
      setFirstInitialConditionContent(Layout());
      setSecondInitialConditionContent(Layout());
      break;
    case Type::SingleRecurrence: {
      error = setExpressionContent(SymbolHelper::BuildSequence(
          name, UserExpression::Builder(KUnknownSymbol)));
      setFirstInitialConditionContent("1"_l);
      setSecondInitialConditionContent(Layout());
      break;
    }
    case Type::DoubleRecurrence: {
      error = setExpressionContent(UserExpression::Create(
          KAdd(KA, KB),
          {.KA = SymbolHelper::BuildSequence(
               name, UserExpression::Builder(KAdd(KUnknownSymbol, 1_e))),
           .KB = SymbolHelper::BuildSequence(
               name, UserExpression::Builder(KUnknownSymbol))}));
      setFirstInitialConditionContent("1"_l);
      setSecondInitialConditionContent("1"_l);
      break;
    }
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  (void)error;  // Silence compilation warning
}

void Sequence::setInitialRank(int rank) {
  recordData()->setInitialRank(rank);
  m_firstInitialCondition.tidyName();
  m_secondInitialCondition.tidyName();
}

void Sequence::setRecursiveNotation(RecursiveNotation notation) {
  recordData()->setRecursiveNotation(notation);
  m_definition.tidyName();
}

Layout Sequence::aggregatedLayout() {
  Layout result;
  switch (type()) {
    case Type::Explicit:
      result = Layout::Create(KSequence1L(KA, KB),
                              {.KA = definitionName(), .KB = layout()});
      break;
    case Type::SingleRecurrence:
      result = Layout::Create(KSequence2L(KA, KB, KC, KD),
                              {.KA = definitionName(),
                               .KB = layout(),
                               .KC = firstInitialConditionName(),
                               .KD = firstInitialConditionLayout()});
      break;
    case Type::DoubleRecurrence:
      result = Layout::Create(KSequence3L(KA, KB, KC, KD, KE, KF),
                              {.KA = definitionName(),
                               .KB = layout(),
                               .KC = firstInitialConditionName(),
                               .KD = firstInitialConditionLayout(),
                               .KE = secondInitialConditionName(),
                               .KF = secondInitialConditionLayout()});
      break;
  }
  SequenceHelper::SetFirstRank(result, initialRank());
  return result;
}

Ion::Storage::Record::ErrorStatus Sequence::setLayoutsForAggregated(
    Layout l, const SymbolContext& symbolContext) {
  if (SequenceHelper::IsSequenceInsideRack(l)) {
    Ion::Storage::Record::ErrorStatus error =
        setContent(SequenceHelper::ExtractExpressionAtRow(l, 0), symbolContext);
    if (type() == Type::Explicit ||
        error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    error = setFirstInitialConditionContent(
        SequenceHelper::ExtractExpressionAtRow(l, 1), symbolContext);
    if (type() == Type::SingleRecurrence ||
        error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    return setSecondInitialConditionContent(
        SequenceHelper::ExtractExpressionAtRow(l, 2), symbolContext);
  } else {
    // Handle layout as main expression
    return setContent(l, symbolContext);
  }
}

Layout Sequence::nameLayout() const {
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

bool Sequence::isSuitableForCobweb() const {
  return type() == Type::SingleRecurrence &&
         !std::isnan(approximateAtRank(
             initialRank(),
             GlobalContextAccessor::SequenceContext().cache())) &&
         !mainExpressionContainsForbiddenTerms(true, false, false);
}

bool Sequence::mainExpressionContainsForbiddenTerms(
    bool recursionIsAllowed, bool systemSymbolIsAllowed,
    bool otherSequencesAreAllowed) const {
  constexpr size_t bufferSize = SequenceStore::k_maxSequenceNameLength + 1;
  char buffer[bufferSize];
  name(buffer, bufferSize);
  // TODO_PCJ: used SymbolicComputation::ReplaceDefinedSymbols
  return SequenceHelper::MainExpressionContainsForbiddenTerms(
      expressionClone(), GlobalContextAccessor::SequenceContext(), buffer,
      type(), initialRank(), recursionIsAllowed, systemSymbolIsAllowed,
      otherSequencesAreAllowed);
}

void Sequence::tidyDownstreamPoolFrom(const PoolObject* treePoolCursor) const {
  model()->tidyDownstreamPoolFrom(treePoolCursor);
  m_firstInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
  m_secondInitialCondition.tidyDownstreamPoolFrom(treePoolCursor);
}

template <typename T>
T Sequence::privateEvaluateYAtX(T x) const {
  // Round behaviour changes platform-wise if std::isnan(x)
  assert(!std::isnan(x));
  int n = std::round(x);
  return static_cast<T>(
      approximateAtRank(n, GlobalContextAccessor::SequenceContext().cache()));
}

double Sequence::approximateAtRank(int rank, SequenceCache* sqctx) const {
  int sequenceIndex = SequenceStore::SequenceIndexForName(fullName()[0]);
  if (!isDefined() || rank < initialRank() ||
      (rank >= firstNonInitialRank() &&
       sqctx->sequenceIsNotComputable(sequenceIndex))) {
    return NAN;
  }
  sqctx->stepUntilRank(sequenceIndex, rank);
  return sqctx->storedValueOfSequenceAtRank(sequenceIndex, rank);
}

double Sequence::approximateAtContextRank(int rank,
                                          bool intermediateComputation) const {
  if (rank < initialRank()) {
    return NAN;
  }
  if (rank >= firstNonInitialRank()) {
    // TODO: Prepared function of expressionReduced could be memoized.
    return expressionReduced()
        .getPreparedFunction(Function::k_unknownName)
        .approximateToRealScalarWithValue(static_cast<double>(rank - order()));
  }
  assert(type() != Type::Explicit);
  SystemExpression e;
  if (rank == initialRank()) {
    e = firstInitialConditionExpressionReduced();
  } else {
    assert(type() == Type::DoubleRecurrence);
    e = secondInitialConditionExpressionReduced();
  }
  return e.approximateSystemToRealScalar<double>();
}

double Sequence::sumBetweenBoundsValue(double start, double end) const {
  double result = 0.0;
  if (end - start > k_maxNumberOfSteps || start + 1.0 == start) {
    return NAN;
  }
  start = std::round(start);
  end = std::round(end);
  for (double i = start; i <= end; i = i + 1.0) {
    /* When |start| >> 1.0, start + 1.0 = start. In that case, quit the
     * infinite loop. */
    if (i == i - 1.0 || i == i + 1.0) {
      return NAN;
    }
    result += evaluateXYAtParameter(i).y();
  }
  return result;
}

SystemExpression Sequence::sumBetweenBounds(double start, double end) const {
  return SystemExpression::Builder<double>(sumBetweenBoundsValue(start, end));
}

Sequence::RecordDataBuffer* Sequence::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer*>(const_cast<void*>(d.buffer));
}

/* Sequence Model */

Layout Sequence::SequenceModel::name(const Sequence* sequence) const {
  if (m_name.isUninitialized()) {
    buildName(sequence);
  }
  return m_name;
}

void Sequence::SequenceModel::tidyName(const PoolObject* treePoolCursor) const {
  if (treePoolCursor == nullptr || m_name.isDownstreamOf(treePoolCursor)) {
    m_name = Layout();
  }
}

void Sequence::SequenceModel::tidyDownstreamPoolFrom(
    const PoolObject* treePoolCursor) const {
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
  GlobalContextAccessor::SequenceStore().setStorageChangeFlag(true);
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

Poincare::Layout Sequence::DefinitionName(CodePoint name, Type type,
                                          RecursiveNotation notation) {
  const char* index;
  if (type == Type::Explicit || notation == RecursiveNotation::Shifted) {
    index = "n";
  } else if (type == Type::SingleRecurrence) {
    index = "n+1";
  } else {
    assert(type == Type::DoubleRecurrence);
    index = "n+2";
  }
  return Layout::Create(KA ^ KSubscriptL(KB), {.KA = Layout::CodePoint(name),
                                               .KB = Layout::String(index)});
}

void Sequence::DefinitionModel::buildName(const Sequence* sequence) const {
  m_name = DefinitionName(sequence->fullName()[0], sequence->type(),
                          sequence->recursiveNotation());
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

void Sequence::InitialConditionModel::buildName(
    const Sequence* sequence) const {
  assert(
      (conditionIndex() == 0 && sequence->type() == Type::SingleRecurrence) ||
      sequence->type() == Type::DoubleRecurrence);
  char buffer[k_initialRankNumberOfDigits + 1];
  int size = OMG::Print::IntLeft(sequence->initialRank() + conditionIndex(),
                                 buffer, k_initialRankNumberOfDigits + 1);
  buffer[size] = 0;
  m_name = Layout::Create(KA ^ KSubscriptL(KB),
                          {.KA = Layout::CodePoint(sequence->fullName()[0]),
                           .KB = Layout::String(buffer)});
}

template double Sequence::privateEvaluateYAtX<double>(double) const;
template float Sequence::privateEvaluateYAtX<float>(float) const;

}  // namespace Shared
