#include "calculation_store.h"

#include <apps/shared/global_context.h>
#include <poincare/cas.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/helpers/store.h>
#include <poincare/helpers/symbol.h>
#include <poincare/helpers/trigonometry.h>
#include <poincare/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

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
  if (!Preferences::SharedPreferences()->examMode().forbidUnits()) {
    Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
        expression, Preferences::SharedPreferences()->angleUnit());
  }
}

// Public

CalculationStore::CalculationStore(char* buffer, size_t bufferSize)
    : m_buffer(buffer),
      m_bufferSize(bufferSize),
      m_numberOfCalculations(0),
      m_inUsePreferences(*Preferences::SharedPreferences()) {}

ExpiringPointer<Calculation> CalculationStore::calculationAtIndex(
    int index) const {
  assert(0 <= index && index <= numberOfCalculations() - 1);
  Calculation* ptr = reinterpret_cast<Calculation*>(
      index == numberOfCalculations() - 1 ? m_buffer
                                          : endOfCalculationAtIndex(index + 1));
  return ExpiringPointer(ptr);
}

UserExpression CalculationStore::ansExpression(Context* context) const {
  const UserExpression defaultAns = UserExpression::Builder(0_e);
  if (numberOfCalculations() == 0) {
    return defaultAns;
  }
  ExpiringPointer<Calculation> mostRecentCalculation = calculationAtIndex(0);
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
     * the approximation, prefer using the approximation too keep the decimal
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

static void compute(Poincare::Expression inputExpression,
                    Poincare::Expression& exactOutputExpression,
                    Poincare::Expression& approximateOutputExpression,
                    Poincare::Preferences::ComplexFormat& complexFormat,
                    Poincare::Context* context) {
  assert(!inputExpression.isUninitialized());
  // Update complexFormat with input expression
  complexFormat =
      Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
          complexFormat, inputExpression, context);

  Internal::ProjectionContext projContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = Poincare::Preferences::SharedPreferences()->angleUnit(),
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_symbolic = CAS::Enabled() ? SymbolicComputation::ReplaceDefinedSymbols
                                   : SymbolicComputation::ReplaceAllSymbols,
      .m_context = context};

  inputExpression.cloneAndSimplifyAndApproximate(
      &exactOutputExpression, &approximateOutputExpression, &projContext);
}

static OutputExpressions computeInterruptible(
    Poincare::Expression inputExpression,
    Poincare::Preferences::ComplexFormat& complexFormat,
    Poincare::Context* context) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */
  OutputExpressions outputs;
  CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    compute(inputExpression, outputs.exact, outputs.approximate, complexFormat,
            context);
  } else {
    GlobalContext::s_sequenceStore->tidyDownstreamPoolFrom(
        checkpoint.endOfPoolBeforeCheckpoint());
    // If the output computation is interrupted, return undef
    /* TODO: split into two Checkpoints, one for the exact computation and one
     * for the approximate computation */
    outputs = {Undefined::Builder(), Undefined::Builder()};
  }

  assert(!outputs.exact.isUninitialized() &&
         !outputs.approximate.isUninitialized());
  return outputs;
}

static void processStore(OutputExpressions& outputs,
                         Poincare::UserExpression input,
                         Poincare::Context* context) {
  /* The global context performs the store and ensures that no symbol is kept in
   * the definition of a variable.
   * Once this is done, the output is replaced with the stored expression. To do
   * so, the expression of the symbol is retrieved after it is stored because it
   * can be different from the value in the store expression.
   * e.g. if f(x) = cos(x), the expression "f(x^2)->f(x)" will return
   * "cos(x^2)". */

  // TODO: factorize with StoreMenuController::parseAndStore
  // TODO: add circuit breaker?
  UserExpression value = StoreHelper::Value(outputs.exact);
  UserExpression symbol = StoreHelper::Symbol(outputs.exact);
  UserExpression valueApprox =
      PoincareHelpers::Approximate<double>(value, context);
  if (symbol.isUserSymbol() &&
      CAS::ShouldOnlyDisplayApproximation(input, value, valueApprox, context)) {
    value = valueApprox;
  }
#if 0
/* TODO_PCJ: restore assert
* Handle case of functions (3*x->f(x)): there should be no symbol except x */
assert(!value.recursivelyMatches(
[](const Expression e) { return e.isUserSymbol(); }));
#endif
  if (StoreHelper::StoreValueForSymbol(context, value, symbol)) {
    outputs.exact = value;
    outputs.approximate = valueApprox;
    assert(!outputs.exact.isUninitialized() &&
           !outputs.approximate.isUninitialized());
  } else {
    outputs.exact = Undefined::Builder();
    outputs.approximate = Undefined::Builder();
  }
}

static void postProcessOutputs(OutputExpressions& outputs,
                               Poincare::Expression inputExpression,
                               bool unitsForbidden,
                               Poincare::Context* context) {
  /* TODO: the two following operations should be performed in a
   * CircuitBreakerCheckpoint to handle the "Back" interruption properly,
   * although it is very unlikely to happen because these operations are fast.
   * However, having a CircuitBreakerCheckpoint here causes unexpected and
   * unexplained problems that should be investigated in more details.
   */
  if (unitsForbidden && outputs.approximate.hasUnit()) {
    outputs = {Undefined::Builder(), Undefined::Builder()};
  }
  enhancePushedExpression(outputs.exact);

  /* When an input contains a store, it is kept by the reduction in the exact
   * output and the actual store is performed here.
   * This must be done outside of a checkpoint because it can delete some
   * memoized expressions in the Sequence store, which would alter the pool
   * above the checkpoint. */
  // TODO: improve the safety of the store operation
  if (outputs.exact.isStore()) {
    processStore(outputs, inputExpression, context);
  }
}

Poincare::UserExpression CalculationStore::parseInput(
    Poincare::Layout inputLayout, Poincare::Context* context) {
  m_inUsePreferences = *Preferences::SharedPreferences();

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
        UserExpression::Parse(inputLayout, &ansContext);
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
    Poincare::Expression inputExpression, Poincare::Context* context) {
  Poincare::Preferences::ComplexFormat complexFormat =
      Poincare::Preferences::SharedPreferences()->complexFormat();
  OutputExpressions outputs =
      computeInterruptible(inputExpression, complexFormat, context);

  postProcessOutputs(outputs, inputExpression,
                     m_inUsePreferences.examMode().forbidUnits(), context);

  return {inputExpression, outputs, complexFormat};
}

ExpiringPointer<Calculation> CalculationStore::push(
    Poincare::Layout inputLayout, Poincare::Context* context) {
  Poincare::UserExpression inputExpression = parseInput(inputLayout, context);
  if (inputExpression.isUninitialized()) {
    /* If parsing was interrupted (which is unlikely to happen), do not update
     * the calculation store */
    return nullptr;
  }

  CalculationElements calculationToPush =
      computeAndProcess(inputExpression, context);

  size_t neededSize =
      neededSizeForCalculation(calculationToPush.sizeOfExpressions());
  if (neededSize > m_bufferSize) {
    /* The calculation is too big to hold on the buffer, even if all previous
     * calculations were deleted. Replace its outputs by undefined, it should
     * now fit on the calculation buffer. */
    calculationToPush.outputs.exact = Undefined::Builder();
    calculationToPush.outputs.approximate = Undefined::Builder();
    neededSize =
        neededSizeForCalculation(calculationToPush.sizeOfExpressions());
    if (neededSize > m_bufferSize) {
      /* If the calculation with undefined outputs is still too big, it means
       * that the input expression was too big, which is very unlikely to happen
       * in a real usecase. In that hypothetical case, do not update the
       * calculation store at all */
      return nullptr;
    }
  }

  // Free space for the new calculation and move cursor if needed
  char* cursor = endOfCalculations();
  getEmptySpace(&cursor, neededSize);

  Calculation* pushedCalculation = pushCalculation(calculationToPush, &cursor);
  assert(pushedCalculation);
  return ExpiringPointer(pushedCalculation);
}

bool CalculationStore::preferencesHaveChanged() {
  // Track settings that might invalidate HistoryCells heights
  Preferences* preferences = Preferences::SharedPreferences();
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
  PoolVariableContext ansContext(SymbolHelper::AnsMainAlias(), context);
  ansContext.setExpressionForUserNamed(ansExpression(context),
                                       SymbolHelper::Ans());
  return ansContext;
}

// Private

char* CalculationStore::endOfCalculationAtIndex(int index) const {
  assert(0 <= index && index < numberOfCalculations());
  char* res = pointerArray()[index];
  /* Make sure the calculation pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
}

size_t CalculationStore::spaceForNewCalculations(
    const char* currentEndOfCalculations) const {
  // Be careful with size_t: negative values are not handled
  return currentEndOfCalculations + sizeof(Calculation*) < pointerArea()
             ? (pointerArea() - currentEndOfCalculations) - sizeof(Calculation*)
             : 0;
}

size_t CalculationStore::privateDeleteCalculationAtIndex(
    int index, char* shiftedMemoryEnd) {
  char* deletionStart = index == numberOfCalculations() - 1
                            ? m_buffer
                            : endOfCalculationAtIndex(index + 1);
  char* deletionEnd = endOfCalculationAtIndex(index);
  size_t deletedSize = deletionEnd - deletionStart;
  size_t shiftedMemorySize = shiftedMemoryEnd - deletionEnd;

  Ion::CircuitBreaker::lock();
  memmove(deletionStart, deletionEnd, shiftedMemorySize);

  for (int i = index - 1; i >= 0; i--) {
    pointerArray()[i + 1] = pointerArray()[i] - deletedSize;
  }
  m_numberOfCalculations--;
  Ion::CircuitBreaker::unlock();

  return deletedSize;
}

void CalculationStore::getEmptySpace(char** location, size_t neededSize) {
  while (spaceForNewCalculations(*location) < neededSize) {
    if (numberOfCalculations() == 0) {
      *location = k_pushErrorLocation;
      return;
    }
    int deleted = deleteOldestCalculation(*location);
    *location -= deleted;
  }
}

Calculation* CalculationStore::pushEmptyCalculation(char** location) {
  assert(*location != k_pushErrorLocation);
  Calculation* newCalculation = reinterpret_cast<Calculation*>(*location);
  assert(spaceForNewCalculations(*location) >= sizeof(Calculation));
  new (*location) Calculation(
      Poincare::Preferences::SharedPreferences()->calculationPreferences());
  *location += sizeof(Calculation);
  assert(*location != k_pushErrorLocation);
  return newCalculation;
}

size_t CalculationStore::pushExpressionTree(char** location, UserExpression e) {
  assert(*location != k_pushErrorLocation);
  size_t length = e.tree()->treeSize();
  assert(spaceForNewCalculations(*location) >= length);
  memcpy(*location, e.tree(), length);
  *location += length;
  return length;
}

Calculation* CalculationStore::pushCalculation(
    const CalculationElements& calculationToPush, char** location) {
  assert(*location >= m_buffer &&
         *location <
             pointerArea() - neededSizeForCalculation(
                                 calculationToPush.sizeOfExpressions()));

  // Push an empty Calculation instance (takes sizeof(Calculation))
  Calculation* newCalculation = pushEmptyCalculation(location);
  // Set the complex format
  newCalculation->setComplexFormat(calculationToPush.complexFormat);

  // Push the input and output expressions after the Calculation
  assert(!calculationToPush.input.isUninitialized() &&
         !calculationToPush.outputs.exact.isUninitialized() &&
         !calculationToPush.outputs.approximate.isUninitialized());
  newCalculation->m_inputTreeSize =
      pushExpressionTree(location, calculationToPush.input);
  newCalculation->m_exactOutputTreeSize =
      pushExpressionTree(location, calculationToPush.outputs.exact);
  newCalculation->m_approximatedOutputTreeSize =
      pushExpressionTree(location, calculationToPush.outputs.approximate);

  /* Write the pointer to the new calculation at pointerArea() (takes
   * sizeof(Calculation*)) */
  assert(*location < pointerArea() - sizeof(Calculation*));
  pointerArray()[-1] = *location;
  /* Now that the calculation is fully built, we can finally update
   * m_numberOfCalculations. As that is the only variable tracking the state
   * of the store, updating it only at the end of the push ensures that,
   * should an interruption occur, all the temporary states are silently
   * discarded and no ill-formed Calculation is stored. */
  m_numberOfCalculations++;
  assert(calculationAtIndex(0).pointer() == newCalculation);

  return newCalculation;
}

}  // namespace Calculation
