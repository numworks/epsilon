#include "calculation_store.h"

#include <apps/exam_mode_manager.h>
#include <apps/shared/global_context.h>
#include <poincare/cas.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/helpers/store.h>
#include <poincare/helpers/symbol.h>
#include <poincare/helpers/trigonometry.h>
#include <poincare/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>
#include <poincare/variable_store.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static void enhancePushedExpression(UserExpression& expression) {
  assert(!expression.isUninitialized());
  /* Add an angle unit in trigonometric functions if the user could have
   * forgotten to change the angle unit in the preferences.
   * Ex: If angleUnit = rad, cos(4) is enhanced to cos(4rad)
   *     If angleUnit = deg, cos(π) is enhanced to cos(π°)
   *     If angleUnit = *, 2->rad is enhanced to 2*->rad
   * */
  if (!ExamModeManager::ExamMode().forbidUnits()) {
    Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
        expression, MathPreferences::SharedPreferences()->angleUnit());
  }
}

// Public

CalculationStore::CalculationStore(char* buffer, size_t bufferSize)
    : Store<Ion::CircuitBreaker::lock, Ion::CircuitBreaker::unlock>(buffer,
                                                                    bufferSize),
      m_inUsePreferences(*MathPreferences::SharedPreferences()) {}

OMG::ExpiringPointer<Calculation> CalculationStore::calculationAtIndex(
    int index) const {
  assert(0 <= index && index <= numberOfCalculations() - 1);
  return OMG::ExpiringPointer(
      reinterpret_cast<Calculation*>(elementAtIndex(index)));
}

UserExpression CalculationStore::ansExpression(Context* context) const {
  const UserExpression defaultAns = UserExpression::Builder(0_e);
  if (numberOfCalculations() == 0) {
    return defaultAns;
  }
  OMG::ExpiringPointer<Calculation> mostRecentCalculation =
      calculationAtIndex(0);
  UserExpression input = mostRecentCalculation->input();
  UserExpression exactOutput = mostRecentCalculation->exactOutput();
  UserExpression approxOutput = mostRecentCalculation->approximateOutput();
  UserExpression ansExpr;
  if ((mostRecentCalculation->displayOutput() ==
           Calculation::DisplayOutput::ApproximateOnly &&
       !exactOutput.isIdenticalTo(approxOutput)) ||
      exactOutput.isUndefined()) {
    /* Case 1.
     * If exact output was hidden, it should not be accessible using Ans.
     * Return input instead so that no precision is lost.
     * Except if the exact output is equal to its approximation and is neither
     * Nonreal nor Undefined, in which case the exact output can be used as Ans
     * since it's exactly the approx (this happens mainly with units).
     * */
    ansExpr = input;
    if (ansExpr.isStore()) {
      /* Case 1.1 If the input is a store expression, keep only the first child
       * of the input in Ans because the whole store can't be used in a
       * calculation. */
      ansExpr = ansExpr.cloneChildAtIndex(0);
    }
  } else if (input.recursivelyMatches(&Expression::isApproximate, context) &&
             mostRecentCalculation->equalSign() ==
                 Calculation::EqualSign::Equal) {
    /* Case 2.
     * If the user used a decimal in the input and the exact output is equal to
     * the approximation, prefer using the approximation to keep the decimal
     * form. */
    ansExpr = approxOutput;
  } else {
    /* Case 3.
     * Default to the exact output.*/
    ansExpr = exactOutput;
  }
  assert(ansExpr.isUninitialized() || !ansExpr.isStore());
  return ansExpr.isUninitialized() ? defaultAns : ansExpr;
}

void CalculationStore::replaceAnsInExpression(UserExpression& expression,
                                              Context* context) const {
  UserExpression ansSymbol = SymbolHelper::Ans();
  UserExpression ansExpression = this->ansExpression(context);
  expression.replaceSymbolWithExpression(ansSymbol, ansExpression);
}

static bool compute(Poincare::UserExpression inputExpression,
                    Poincare::UserExpression& exactOutputExpression,
                    Poincare::UserExpression& approximateOutputExpression,
                    Poincare::Preferences::ComplexFormat& complexFormat,
                    Poincare::Context* context) {
  assert(!inputExpression.isUninitialized());
  // Update complexFormat with input expression
  complexFormat =
      Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
          complexFormat, inputExpression, context);

  Internal::ProjectionContext projContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_symbolic = CAS::Enabled() ? SymbolicComputation::ReplaceDefinedSymbols
                                   : SymbolicComputation::ReplaceAllSymbols,
      .m_context = context};

  return inputExpression.cloneAndSimplifyAndApproximate(
      &exactOutputExpression, &approximateOutputExpression, projContext);
}

struct CalculationResult {
  OutputExpressions outputs;
  bool hasReductionFailure;
};

static CalculationResult computeInterruptible(
    Poincare::UserExpression inputExpression,
    Poincare::Preferences::ComplexFormat& complexFormat,
    Poincare::Context* context) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */
  OutputExpressions outputs;
  bool hasReductionFailure = false;
  CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    hasReductionFailure = compute(inputExpression, outputs.exact,
                                  outputs.approximate, complexFormat, context);
  } else {
    GlobalContext::s_sequenceStore->tidyDownstreamPoolFrom(
        checkpoint.endOfPoolBeforeCheckpoint());
    // If the output computation is interrupted, return undef
    outputs = {Expression::Undefined(), Expression::Undefined()};
    hasReductionFailure = true;
  }

  assert(!outputs.exact.isUninitialized() &&
         !outputs.approximate.isUninitialized());
  return CalculationResult{outputs, hasReductionFailure};
}

static void processStore(OutputExpressions& outputs,
                         Poincare::UserExpression input,
                         Poincare::VariableStore* variableStore) {
  /* The global variable store performs the store and ensures that no symbol is
   * kept in the definition of a variable.
   * Once this is done, the output is replaced with the stored expression. To do
   * so, the expression of the symbol is retrieved after it is stored because it
   * can be different from the value in the store expression. e.g. if f(x) =
   * cos(x), the expression "f(x^2)->f(x)" will return "cos(x^2)". */

  // TODO: factorize with StoreMenuController::parseAndStore
  // TODO: add circuit breaker?
  UserExpression value = StoreHelper::Value(outputs.exact);
  UserExpression symbol = StoreHelper::Symbol(outputs.exact);
  UserExpression valueApprox =
      PoincareHelpers::ApproximateUser<double>(value, variableStore);
  if (symbol.isUserSymbol() && CAS::ShouldOnlyDisplayApproximation(
                                   input, value, valueApprox, variableStore)) {
    value = valueApprox;
  }
#if 0
/* TODO_PCJ: restore assert
* Handle case of functions (3*x->f(x)): there should be no symbol except x */
assert(!value.recursivelyMatches(
[](const Expression e) { return e.isUserSymbol(); }));
#endif
  if (StoreHelper::StoreValueForSymbol(variableStore, value, symbol)) {
    outputs.exact = value;
    outputs.approximate = valueApprox;
    assert(!outputs.exact.isUninitialized() &&
           !outputs.approximate.isUninitialized());
  } else {
    outputs.exact = Expression::Undefined();
    outputs.approximate = Expression::Undefined();
  }
}

static void postProcessOutputs(OutputExpressions& outputs,
                               Poincare::UserExpression inputExpression,
                               bool unitsForbidden,
                               Poincare::VariableStore* variableStore) {
  /* TODO: the two following operations should be performed in a
   * CircuitBreakerCheckpoint to handle the "Back" interruption properly,
   * although it is very unlikely to happen because these operations are fast.
   * However, having a CircuitBreakerCheckpoint here causes unexpected and
   * unexplained problems that should be investigated in more details.
   */
  if (unitsForbidden && outputs.approximate.hasUnit()) {
    outputs = {Expression::Undefined(), Expression::Undefined()};
  }
  enhancePushedExpression(outputs.exact);

  /* When an input contains a store, it is kept by the reduction in the exact
   * output and the actual store is performed here.
   * This must be done outside of a checkpoint because it can delete some
   * memoized expressions in the Sequence store, which would alter the pool
   * above the checkpoint. */
  // TODO: improve the safety of the store operation
  if (outputs.exact.isStore()) {
    processStore(outputs, inputExpression, variableStore);
  }
}

Poincare::UserExpression CalculationStore::parseInput(
    Poincare::Layout inputLayout, Poincare::Context* context) {
  m_inUsePreferences = *MathPreferences::SharedPreferences();

  CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    /* Compute Ans now before the store is updated or the last calculation
     * deleted.
     * Setting Ans in the context makes it available during the parsing of the
     * input, namely to know if a rightwards arrow is a unit conversion or a
     * variable assignment. */
    PoolVariableContext ansContext = createAnsContext(context);
    UserExpression inputExpression =
        UserExpression::Parse(inputLayout, ansContext);
    replaceAnsInExpression(inputExpression, context);
    enhancePushedExpression(inputExpression);
    return inputExpression;
  } else {
    GlobalContext::s_sequenceStore->tidyDownstreamPoolFrom(
        checkpoint.endOfPoolBeforeCheckpoint());
    return Poincare::UserExpression();
  }
}

CalculationStore::CalculationElements CalculationStore::computeAndProcess(
    Poincare::UserExpression inputExpression,
    Poincare::VariableStore* variableStore) {
  Poincare::Preferences::ComplexFormat complexFormat =
      MathPreferences::SharedPreferences()->complexFormat();
  CalculationResult calculationResult =
      computeInterruptible(inputExpression, complexFormat, variableStore);

  postProcessOutputs(calculationResult.outputs, inputExpression,
                     ExamModeManager::ExamMode().forbidUnits(), variableStore);

  return CalculationElements{inputExpression, calculationResult.outputs,
                             calculationResult.hasReductionFailure,
                             complexFormat};
}

OMG::ExpiringPointer<Calculation> CalculationStore::push(
    Poincare::Layout inputLayout, Poincare::VariableStore* variableStore) {
  Poincare::UserExpression inputExpression =
      parseInput(inputLayout, variableStore);
  if (inputExpression.isUninitialized()) {
    /* If parsing was interrupted (which is unlikely to happen), do not update
     * the calculation store */
    return nullptr;
  }

  CalculationElements calculationToPush =
      computeAndProcess(inputExpression, variableStore);

  size_t neededSize = neededSizeForCalculation(calculationToPush.sizeOfTrees());
  if (neededSize > maximumSize()) {
    /* The calculation is too big to hold on the buffer, even if all previous
     * calculations were deleted. Replace its outputs by undefined, it should
     * now fit on the calculation buffer. */
    calculationToPush.outputs.exact = Expression::Undefined();
    calculationToPush.outputs.approximate = Expression::Undefined();
    neededSize = neededSizeForCalculation(calculationToPush.sizeOfTrees());
    if (neededSize > maximumSize()) {
      /* If the calculation with undefined outputs is still too big, it means
       * that the input expression was too big, which is very unlikely to happen
       * in a real usecase. */
      ExceptionCheckpoint::Raise();
    }
  }

  // Free space for the new calculation
  makeRoomForElement(neededSize);
  Calculation* pushedCalculation = pushCalculation(calculationToPush);
  assert(pushedCalculation);
  return OMG::ExpiringPointer(pushedCalculation);
}

bool CalculationStore::preferencesHaveChanged() {
  // Track settings that might invalidate HistoryCells heights
  const MathPreferences* preferences = MathPreferences::SharedPreferences();
  if (m_inUsePreferences.combinatoricSymbols() ==
          preferences->combinatoricSymbols() &&
      m_inUsePreferences.numberOfSignificantDigits() ==
          preferences->numberOfSignificantDigits() &&
      m_inUsePreferences.logarithmBasePosition() ==
          preferences->logarithmBasePosition()) {
    return false;
  }
  m_inUsePreferences = *preferences;
  return true;
}

PoolVariableContext CalculationStore::createAnsContext(Context* context) {
  return PoolVariableContext(SymbolHelper::AnsMainAlias(),
                             ansExpression(context), context);
}

// Private

Calculation* CalculationStore::pushEmptyCalculation(char** location) {
  Calculation* newCalculation = reinterpret_cast<Calculation*>(*location);
  assert(spaceForNewElements(*location) >= sizeof(Calculation));
  new (*location) Calculation(
      MathPreferences::SharedPreferences()->calculationPreferences());
  *location += sizeof(Calculation);
  return newCalculation;
}

size_t CalculationStore::pushExpressionTree(char** location, UserExpression e) {
  size_t length = e.tree()->treeSize();
  assert(spaceForNewElements(*location) >= length);
  memcpy(*location, e.tree(), length);
  *location += length;
  return length;
}

Calculation* CalculationStore::pushCalculation(
    const CalculationElements& calculationToPush) {
  char* start = endOfElements();
  char* cursor = start;
  assert(neededSizeForCalculation(calculationToPush.sizeOfTrees()) <=
         spaceForNewElements(cursor));

  // Push an empty Calculation instance (takes sizeof(Calculation))
  Calculation* newCalculation = pushEmptyCalculation(&cursor);
  // Set the calculation properties
  newCalculation->setComplexFormat(calculationToPush.complexFormat);
  newCalculation->setReductionFailure(calculationToPush.hasReductionFailure);

  // Push the input and output expressions after the Calculation
  assert(!calculationToPush.input.isUninitialized() &&
         !calculationToPush.outputs.exact.isUninitialized() &&
         !calculationToPush.outputs.approximate.isUninitialized());
  newCalculation->m_inputTreeSize =
      pushExpressionTree(&cursor, calculationToPush.input);
  newCalculation->m_exactOutputTreeSize =
      pushExpressionTree(&cursor, calculationToPush.outputs.exact);
  newCalculation->m_approximatedOutputTreeSize =
      pushExpressionTree(&cursor, calculationToPush.outputs.approximate);

  registerElement(start, cursor - start);
  return newCalculation;
}

}  // namespace Calculation
