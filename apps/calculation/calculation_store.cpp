#include "calculation_store.h"

#include <apps/shared/expression_display_permissions.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/rational.h>
#include <poincare/store.h>
#include <poincare/symbol.h>
#include <poincare/trigonometry.h>
#include <poincare/undefined.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static Expression enhancePushedExpression(Expression expression) {
  /* Add an angle unit in trigonometric functions if the user could have
   * forgotten to change the angle unit in the preferences.
   * Ex: If angleUnit = rad, cos(4)->cos(4rad)
   *     If angleUnit = deg, cos(π)->cos(π°)
   * */
  if (!Preferences::SharedPreferences()->examMode().forbidUnits()) {
    expression = Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
        expression, Preferences::SharedPreferences()->angleUnit());
  }
  return expression;
}

// Public

CalculationStore::CalculationStore(char *buffer, size_t bufferSize)
    : m_buffer(buffer),
      m_bufferSize(bufferSize),
      m_numberOfCalculations(0),
      m_inUsePreferences(*Preferences::SharedPreferences()) {}

ExpiringPointer<Calculation> CalculationStore::calculationAtIndex(
    int index) const {
  assert(0 <= index && index <= numberOfCalculations() - 1);
  Calculation *ptr = reinterpret_cast<Calculation *>(
      index == numberOfCalculations() - 1 ? m_buffer
                                          : endOfCalculationAtIndex(index + 1));
  return ExpiringPointer(ptr);
}

Expression CalculationStore::ansExpression(Context *context) const {
  const Expression defaultAns = Rational::Builder(0);
  if (numberOfCalculations() == 0) {
    return defaultAns;
  }
  ExpiringPointer<Calculation> mostRecentCalculation = calculationAtIndex(0);
  Expression input = mostRecentCalculation->input();
  Expression exactOutput = mostRecentCalculation->exactOutput();
  Expression approxOutput = mostRecentCalculation->approximateOutput(
      Calculation::NumberOfSignificantDigits::Maximal);
  Expression ansExpr;
  if (mostRecentCalculation->displayOutput(context) ==
          Calculation::DisplayOutput::ApproximateOnly &&
      (strcmp(mostRecentCalculation->approximateOutputText(
                  Calculation::NumberOfSignificantDigits::Maximal),
              mostRecentCalculation->exactOutputText()) != 0 ||
       exactOutput.isUndefined())) {
    /* Case 1.
     * If exact output was hidden, is   should not be accessible using Ans.
     * Return input instead so that no precision is lost.
     * Except if the exact output is equal to its approximation and is neither
     * Nonreal nor Undefined, in which case the exact output can be used as Ans
     * since it's exactly the approx (this happens mainly with units).
     * */
    ansExpr = input;
    if (ansExpr.type() == ExpressionNode::Type::Store) {
      /* Case 1.1 If the input is a store expression, keep only the first child
       * of the input in Ans because the whole store can't be used in a
       * calculation. */
      ansExpr = ansExpr.childAtIndex(0);
    }
  } else if (input.recursivelyMatches(Expression::IsApproximate, context) &&
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
  assert(ansExpr.isUninitialized() ||
         ansExpr.type() != ExpressionNode::Type::Store);
  return ansExpr.isUninitialized() ? defaultAns : ansExpr;
}

Expression CalculationStore::replaceAnsInExpression(Expression expression,
                                                    Context *context) const {
  Symbol ansSymbol = Symbol::Ans();
  Expression ansExpression = this->ansExpression(context);
  return expression.replaceSymbolWithExpression(ansSymbol, ansExpression);
}

ExpiringPointer<Calculation> CalculationStore::push(
    const char *text, Poincare::Context *context) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */
  m_inUsePreferences = *Preferences::SharedPreferences();
  char *cursor = endOfCalculations();
  Expression exactOutputExpression, approximateOutputExpression,
      storeExpression;

  {
    CircuitBreakerCheckpoint checkpoint(
        Ion::CircuitBreaker::CheckpointType::Back);
    if (CircuitBreakerRun(checkpoint)) {
      /* Compute Ans now before the store is updated or the last calculation
       * deleted.
       * Setting Ans in the context makes it available during the parsing of the
       * input, namely to know if a rightwards arrow is a unit conversion or a
       * variable assignment. */
      VariableContext ansContext = createAnsContext(context);

      // Push a new, empty Calculation
      cursor = pushEmptyCalculation(
          cursor,
          Poincare::Preferences::SharedPreferences()->calculationPreferences());
      assert(cursor != k_pushError);

      // Push the input
      Expression inputExpression = Expression::Parse(text, &ansContext);
      inputExpression = replaceAnsInExpression(inputExpression, context);
      inputExpression = enhancePushedExpression(inputExpression);
      cursor = pushSerializedExpression(
          cursor, inputExpression, PrintFloat::k_maxNumberOfSignificantDigits);
      if (cursor == k_pushError) {
        return errorPushUndefined();
      }
      /* Recompute the location of the input text in case a calculation was
       * deleted. */
      char *const inputText = endOfCalculations() + sizeof(Calculation);

      // Parse and compute the expression
      inputExpression = Expression::Parse(inputText, context, false);
      assert(!inputExpression.isUninitialized());
      PoincareHelpers::CloneAndSimplifyAndApproximate(
          inputExpression, &exactOutputExpression, &approximateOutputExpression,
          context,
          {.symbolicComputation = SymbolicComputation::
               ReplaceAllSymbolsWithDefinitionsOrUndefined});
      assert(!exactOutputExpression.isUninitialized() &&
             !approximateOutputExpression.isUninitialized());

      // Post-processing of store expression
      exactOutputExpression = enhancePushedExpression(exactOutputExpression);
      if (exactOutputExpression.type() == ExpressionNode::Type::Store) {
        storeExpression = exactOutputExpression;
#if 0  // TODO_PCJ
        Expression exactStoredExpression =
            static_cast<Store &>(storeExpression).value();
        approximateOutputExpression =
            PoincareHelpers::ApproximateKeepingUnits<double>(
                exactStoredExpression, context);
        if (static_cast<Store &>(storeExpression).symbol().type() ==
                ExpressionNode::Type::Symbol &&
            ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
                inputExpression, exactStoredExpression,
                approximateOutputExpression, context)) {
          storeExpression.replaceChildAtIndexInPlace(
              0, approximateOutputExpression);
        }
        assert(static_cast<Store &>(storeExpression).symbol().type() !=
                   ExpressionNode::Type::Symbol ||
               !static_cast<Store &>(storeExpression)
                    .value()
                    .deepIsSymbolic(
                        nullptr, SymbolicComputation::DoNotReplaceAnySymbol));
#endif
      }
    } else {
      context->tidyDownstreamPoolFrom(checkpoint.endOfPoolBeforeCheckpoint());
      return nullptr;
    }
  }

  /* When a input contains a store, it is kept by the reduction in the
   * exact output and the actual store is performed here. The global
   * context will perform the store and ensure that no symbol is kept in
   * the definition of a variable.
   * This must be done after the checkpoint because it can delete
   * some memoized expressions in the Sequence store, which would alter the pool
   * above the checkpoint.
   *
   * Once this is done, replace the output with the stored expression. To do
   * so, retrieve the expression of the symbol after it is stored because it can
   * be different from the value in the storeExpression.
   * e.g. if f(x) = cos(x), the expression "f(x^2)->f(x)" will return
   * "cos(x^2)".
   * */
  if (!storeExpression.isUninitialized()) {
    assert(storeExpression.type() == ExpressionNode::Type::Store);
    if (static_cast<Store &>(storeExpression).storeValueForSymbol(context)) {
      exactOutputExpression = context->expressionForSymbolAbstract(
          static_cast<Store &>(storeExpression).symbol(), false);
      assert(!exactOutputExpression.isUninitialized());
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

  /* Push the outputs: exact output, and approximate output with maximum
   * number of significant digits and displayed number of digits.
   * If one is too big for the store, push undef instead. */
  for (int i = 0; i < Calculation::k_numberOfExpressions - 1; i++) {
    Expression e = i == 0 ? exactOutputExpression : approximateOutputExpression;
    int digits = i == Calculation::k_numberOfExpressions - 2
                     ? m_inUsePreferences.numberOfSignificantDigits()
                     : PrintFloat::k_maxNumberOfSignificantDigits;

    char *nextCursor = pushSerializedExpression(cursor, e, digits);
    if (nextCursor == k_pushError) {
      nextCursor = pushUndefined(cursor);
      if (nextCursor == k_pushError) {
        return errorPushUndefined();
      }
    }
    cursor = nextCursor;
  }

  /* All data has been appended, store the pointer to the end of the
   * calculation. */
  assert(cursor < pointerArea() - sizeof(Calculation *));
  pointerArray()[-1] = cursor;
  Calculation *newCalculation =
      reinterpret_cast<Calculation *>(endOfCalculations());

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
  Preferences *preferences = Preferences::SharedPreferences();
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

VariableContext CalculationStore::createAnsContext(Context *context) {
  VariableContext ansContext(Symbol::k_ansAliases.mainAlias(), context);
  ansContext.setExpressionForSymbolAbstract(ansExpression(context),
                                            Symbol::Ans());
  return ansContext;
}

// Private

char *CalculationStore::endOfCalculationAtIndex(int index) const {
  assert(0 <= index && index < numberOfCalculations());
  char *res = pointerArray()[index];
  /* Make sure the calculation pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
}

size_t CalculationStore::spaceForNewCalculations(
    char *currentEndOfCalculations) const {
  // Be careful with size_t: negative values are not handled
  return currentEndOfCalculations + sizeof(Calculation *) < pointerArea()
             ? (pointerArea() - currentEndOfCalculations) -
                   sizeof(Calculation *)
             : 0;
}

size_t CalculationStore::privateDeleteCalculationAtIndex(
    int index, char *shiftedMemoryEnd) {
  char *deletionStart = index == numberOfCalculations() - 1
                            ? m_buffer
                            : endOfCalculationAtIndex(index + 1);
  char *deletionEnd = endOfCalculationAtIndex(index);
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

ExpiringPointer<Calculation> CalculationStore::errorPushUndefined() {
  assert(numberOfCalculations() == 0);
  char *cursor = pushUndefined(m_buffer);
  assert(m_buffer < cursor &&
         cursor <= m_buffer + m_bufferSize - sizeof(Calculation *));
  *(pointerArray() - 1) = cursor;
  Calculation *calculation = reinterpret_cast<Calculation *>(m_buffer);
  m_numberOfCalculations = 1;
  return ExpiringPointer(calculation);
}

char *CalculationStore::pushEmptyCalculation(
    char *location,
    Poincare::Preferences::CalculationPreferences calculationPreferences) {
  while (spaceForNewCalculations(location) < k_calculationMinimalSize) {
    if (numberOfCalculations() == 0) {
      return k_pushError;
    }
    location -= deleteOldestCalculation(location);
  }
  new (location) Calculation(calculationPreferences);
  return location + sizeof(Calculation);
}

char *CalculationStore::pushSerializedExpression(
    char *location, Expression e, int numberOfSignificantDigits) {
  while (true) {
    size_t availableSize = spaceForNewCalculations(location);
    size_t length = availableSize > 0
                        ? PoincareHelpers::Serialize(e, location, availableSize,
                                                     numberOfSignificantDigits)
                        : 0;
    constexpr size_t k_maxCharSizeCodePoint = 4;
    if (length + k_maxCharSizeCodePoint < availableSize) {
      /* TODO: this is a hack to check that the serialization went well with the
       * available size. In most cases the serialization stops before writting 1
       * code point. This is a dirty hack and it doesn't cover the general case.
       * Serialization should return a bool indicating if it completed or not.
       * But this will change with poincare junior. */
      assert(location[length] == '\0');
      return location + length + 1;
    }
    if (numberOfCalculations() == 0) {
      return k_pushError;
    }
    location -= deleteOldestCalculation(location);
  }
  assert(false);
}

char *CalculationStore::pushUndefined(char *location) {
  return pushSerializedExpression(
      location, Undefined::Builder(),
      m_inUsePreferences.numberOfSignificantDigits());
}

}  // namespace Calculation
