#include "calculation_store.h"

#include <poincare/cas.h>
#include <poincare/helpers/store.h>
#include <poincare/helpers/symbol.h>
#include <poincare/k_tree.h>
#include <poincare/old/circuit_breaker_checkpoint.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>
#include <poincare/trigonometry.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static UserExpression enhancePushedExpression(UserExpression expression) {
  /* Add an angle unit in trigonometric functions if the user could have
   * forgotten to change the angle unit in the preferences.
   * Ex: If angleUnit = rad, cos(4)->cos(4rad)
   *     If angleUnit = deg, cos(π)->cos(π°)
   * */
  if (!Preferences::SharedPreferences()->examMode().forbidUnits()) {
    Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
        expression, Preferences::SharedPreferences()->angleUnit());
  }
  return expression;
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
  if ((mostRecentCalculation->displayOutput(context) ==
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
  } else if (input.recursivelyMatches(&NewExpression::isApproximate, context) &&
             mostRecentCalculation->equalSign(context) ==
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

UserExpression CalculationStore::replaceAnsInExpression(
    UserExpression expression, Context* context) const {
  JuniorSymbol ansSymbol = JuniorSymbol::Ans();
  UserExpression ansExpression = this->ansExpression(context);
  expression.replaceSymbolWithExpression(ansSymbol, ansExpression);
  return expression;
}

ExpiringPointer<Calculation> CalculationStore::push(
    Poincare::Layout inputLayout, Poincare::Context* context) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */
  Poincare::Preferences::ComplexFormat complexFormat =
      Poincare::Preferences::SharedPreferences()->complexFormat();
  m_inUsePreferences = *Preferences::SharedPreferences();
  char* cursor = endOfCalculations();
  Calculation* current;
  UserExpression inputExpression, exactOutputExpression,
      approximateOutputExpression;

  {
    CircuitBreakerCheckpoint checkpoint(
        Ion::CircuitBreaker::CheckpointType::Back);
    if (CircuitBreakerRun(checkpoint)) {
      /* Compute Ans now before the store is updated or the last calculation
       * deleted.
       * Setting Ans in the context makes it available during the parsing of the
       * input, namely to know if a rightwards arrow is a unit conversion or a
       * variable assignment. */
      PoolVariableContext ansContext = createAnsContext(context);

      /* Push a new, empty Calculation */
      current = reinterpret_cast<Calculation*>(cursor);
      pushEmptyCalculation(&cursor, &current);

      assert(cursor != k_pushErrorLocation);

      /* Push the input */
      inputExpression = UserExpression::Parse(inputLayout, &ansContext);
      inputExpression = replaceAnsInExpression(inputExpression, context);
      inputExpression = enhancePushedExpression(inputExpression);
      const size_t sizeOfExpression =
          pushExpressionTree(&cursor, inputExpression, &current);
      if (sizeOfExpression == k_pushErrorSize) {
        assert(cursor == k_pushErrorLocation);
        // leave the calculation undefined
        return current;
      }
      assert(sizeOfExpression == inputExpression.tree()->treeSize());
      current->m_inputTreeSize = sizeOfExpression;

      /* Parse and compute the expression */
      assert(!inputExpression.isUninitialized());
      // Update complexFormat with input expression
      complexFormat =
          Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
              complexFormat, inputExpression, context);

      Internal::ProjectionContext projContext = {
          .m_complexFormat = complexFormat,
          .m_angleUnit =
              Poincare::Preferences::SharedPreferences()->angleUnit(),
          .m_unitFormat =
              GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
          .m_symbolic = CAS::Enabled()
                            ? SymbolicComputation::ReplaceDefinedSymbols
                            : SymbolicComputation::ReplaceAllSymbols,
          .m_context = context};

      inputExpression.cloneAndSimplifyAndApproximate(
          &exactOutputExpression, &approximateOutputExpression, &projContext);
      assert(!exactOutputExpression.isUninitialized() &&
             !approximateOutputExpression.isUninitialized());

      /* Post-processing of store expression */
      exactOutputExpression = enhancePushedExpression(exactOutputExpression);
    } else {
      context->tidyDownstreamPoolFrom(checkpoint.endOfPoolBeforeCheckpoint());
      return nullptr;
    }
  }

  /* When an input contains a store, it is kept by the reduction in the
   * exact output and the actual store is performed here. The global
   * context will perform the store and ensure that no symbol is kept in
   * the definition of a variable.
   * This must be done after the checkpoint because it can delete
   * some memoized expressions in the Sequence store, which would alter the pool
   * above the checkpoint.
   *
   * Once this is done, replace the output with the stored expression. To do
   * so, retrieve the expression of the symbol after it is stored because it can
   * be different from the value in the store expression.
   * e.g. if f(x) = cos(x), the expression "f(x^2)->f(x)" will return
   * "cos(x^2)".
   * */
  if (exactOutputExpression.isStore()) {
    // TODO: factorize with StoreMenuController::parseAndStore
    // TODO: add circuit breaker?
    UserExpression value = StoreHelper::Value(exactOutputExpression);
    JuniorSymbolAbstract symbol = StoreHelper::Symbol(exactOutputExpression);
    UserExpression valueApprox =
        PoincareHelpers::ApproximateKeepingUnits<double>(value, context);
    if (symbol.isUserSymbol() &&
        CAS::ShouldOnlyDisplayApproximation(inputExpression, value, valueApprox,
                                            context)) {
      value = valueApprox;
    }
#if 0
    /* TODO_PCJ: restore assert
     * Handle case of functions (3*x->f(x)): there should be no symbol except x */
    assert(!value.recursivelyMatches(
        [](const NewExpression e) { return e.isUserSymbol(); }));
#endif
    if (StoreHelper::StoreValueForSymbol(context, value, symbol)) {
      exactOutputExpression = value;
      approximateOutputExpression = valueApprox;
      assert(!exactOutputExpression.isUninitialized() &&
             !approximateOutputExpression.isUninitialized());
    } else {
      exactOutputExpression = Undefined::Builder();
      approximateOutputExpression = Undefined::Builder();
    }
  }

  if (m_inUsePreferences.examMode().forbidUnits() &&
      approximateOutputExpression.hasUnit()) {
    approximateOutputExpression = Undefined::Builder();
    exactOutputExpression = Undefined::Builder();
  }

  /* Push exact output and approximate output.
   * If one is too big for the store, push undef instead. */
  for (int i = 0; i < Calculation::k_numberOfExpressions - 1; i++) {
    UserExpression e =
        i == 0 ? exactOutputExpression : approximateOutputExpression;
    const size_t sizeOfExpression = pushExpressionTree(&cursor, e, &current);
    if (sizeOfExpression == k_pushErrorSize) {
      assert(cursor == k_pushErrorLocation);
      // not enough space, leave undef
      continue;
    }
    assert((i == 0 &&
            sizeOfExpression == exactOutputExpression.tree()->treeSize()) ||
           (i == 1 && sizeOfExpression ==
                          approximateOutputExpression.tree()->treeSize()));

    (i == 0 ? current->m_exactOutputTreeSize
            : current->m_approximatedOutputTreeSize) = sizeOfExpression;
  }

  /* All data has been appended, store the pointer to the end of the
   * calculation. */
  assert(cursor < pointerArea() - sizeof(Calculation*));
  pointerArray()[-1] = cursor;
  Calculation* newCalculation =
      reinterpret_cast<Calculation*>(endOfCalculations());

  newCalculation->setComplexFormat(complexFormat);
  /* Now that the calculation is fully built, we can finally update
   * m_numberOfCalculations. As that is the only variable tracking the state
   * of the store, updating it only at the end of the push ensures that,
   * should an interruption occur, all the temporary states are silently
   * discarded and no ill-formed Calculation is stored. */
  m_numberOfCalculations++;
  return ExpiringPointer(newCalculation);
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
                                       JuniorSymbol::Ans());
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
    char* currentEndOfCalculations) const {
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

/* TODO:
 * We should replace pushEmptyCalculation and pushExpressionTree with a
 * single pushCalculation that would safely set the trees and their sizes.
 */

void CalculationStore::getEmptySpace(char** location, size_t neededSize,
                                     Calculation** current) {
  while (spaceForNewCalculations(*location) < neededSize) {
    if (numberOfCalculations() == 0) {
      *location = k_pushErrorLocation;
      return;
    }
    int deleted = deleteOldestCalculation(*location);
    *location -= deleted;
    *current = reinterpret_cast<Calculation*>(
        reinterpret_cast<char*>(*current) - deleted);
  }
}

size_t CalculationStore::pushEmptyCalculation(char** location,
                                              Calculation** current) {
  getEmptySpace(location, k_calculationMinimalSize, current);
  if (*location == k_pushErrorLocation) {
    return k_pushErrorSize;
  }
  new (*location) Calculation(
      Poincare::Preferences::SharedPreferences()->calculationPreferences());
  *location += sizeof(Calculation);
  return sizeof(Calculation);
}

size_t CalculationStore::pushExpressionTree(char** location, UserExpression e,
                                            Calculation** current) {
  size_t length = e.tree()->treeSize();
  getEmptySpace(location, length, current);
  if (*location == k_pushErrorLocation) {
    return k_pushErrorSize;
  }
  memcpy(*location, e.tree(), length);
  *location += length;
  return length;
}

}  // namespace Calculation
