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

// Public

CalculationStore::CalculationStore(char *buffer, size_t bufferSize)
    : m_buffer(buffer),
      m_bufferSize(bufferSize),
      m_numberOfCalculations(0),
      m_inUsePreferences(*Preferences::sharedPreferences) {}

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
  Expression exactOutput = mostRecentCalculation->exactOutput();
  Expression input = mostRecentCalculation->input();
  if (exactOutput.isUninitialized() || input.isUninitialized()) {
    return defaultAns;
  }
  if (input.recursivelyMatches(Expression::IsApproximate, context)) {
    Expression approximate = mostRecentCalculation->approximateOutput(
        Calculation::NumberOfSignificantDigits::Maximal);
    if (approximate.isUninitialized()) {
      return defaultAns;
    }
    return approximate;
  }
  /* Special case: If exact output was hidden, it should not be accessible using
   * ans, unless it is equal to an approximation that is neither undefined nor
   * nonreal. */
  const char *exactOutputText = mostRecentCalculation->exactOutputText();
  const char *approximateOutputText =
      mostRecentCalculation->approximateOutputText(
          Calculation::NumberOfSignificantDigits::UserDefined);
  if (mostRecentCalculation->displayOutput(context) ==
          Calculation::DisplayOutput::ApproximateOnly &&
      (strcmp(approximateOutputText, exactOutputText) != 0 ||
       exactOutput.type() == ExpressionNode::Type::Nonreal ||
       exactOutput.type() == ExpressionNode::Type::Undefined)) {
    return input;
  }
  return exactOutput;
}

ExpiringPointer<Calculation> CalculationStore::push(
    const char *text, Poincare::Context *context,
    HeightComputer heightComputer) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */
  constexpr int maxNumberOfDigits =
      PrintFloat::k_numberOfStoredSignificantDigits;

  m_inUsePreferences = *Preferences::sharedPreferences;
  char *cursor = endOfCalculations();

  CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    /* Compute Ans now before the store is updated or the last calculation
     * deleted. */
    const Expression ans = ansExpression(context);

    // Push a new, empty Calculation
    cursor = pushEmptyCalculation(cursor);
    assert(cursor != k_pushError);

    // Push the input
    Expression inputExpression = Expression::Parse(text, context);
    inputExpression = EnhanceUserInput(inputExpression, ans);
    cursor =
        pushSerializedExpression(cursor, inputExpression, maxNumberOfDigits);
    if (cursor == k_pushError) {
      return errorPushUndefined(heightComputer);
    }
    /* Recompute the location of the input text in case a calculation was
     * deleted. */
    char *const inputText = endOfCalculations() + sizeof(Calculation);

    // Parse and compute the expression
    Expression exactOutputExpression, approximateOutputExpression;
    PoincareHelpers::ParseAndSimplifyAndApproximate(
        inputText, &inputExpression, &exactOutputExpression,
        &approximateOutputExpression, context);

    // Post-processing of some corner case expressions
    if (exactOutputExpression.type() == ExpressionNode::Type::Store) {
      /* When a input contains a store it is kept by the reduction in the
       * exact output and the actual store is performed here. The global
       * context will perform the store and ensure that no symbol is kept in
       * the definition of a variable.
       * Once this is done we replace the output with the stored expression
       * and approximate it to mimic the behaviour of normal computations. */
      Store storeExpression = reinterpret_cast<Store &>(exactOutputExpression);
      Expression exactStoredExpression = storeExpression.childAtIndex(0);
      Expression approximateStoredExpression =
          PoincareHelpers::ApproximateKeepingUnits<double>(
              exactStoredExpression, context);
      if (storeExpression.childAtIndex(1).type() ==
              ExpressionNode::Type::Symbol &&
          ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
              inputExpression, exactStoredExpression, context)) {
        storeExpression.replaceChildAtIndexInPlace(0,
                                                   approximateStoredExpression);
      }
      storeExpression.storeValueForSymbol(context);
      exactOutputExpression =
          context->expressionForSymbolAbstract(storeExpression.symbol(), false);
      if (exactOutputExpression.isUninitialized()) {
        exactOutputExpression = Undefined::Builder();
      }
      approximateOutputExpression = approximateStoredExpression;
    }
    if (m_inUsePreferences.examMode().forbidUnits() &&
        approximateOutputExpression.hasUnit()) {
      approximateOutputExpression = Undefined::Builder();
    }

    /* Push the outputs: exact output, and approximate output with maximum
     * number of significant digits and displayed number of digits.
     * If one is too big for the store, push undef instead. */
    for (int i = 0; i < Calculation::k_numberOfExpressions - 1; i++) {
      Expression e =
          i == 0 ? exactOutputExpression : approximateOutputExpression;
      int digits = i == Calculation::k_numberOfExpressions - 2
                       ? m_inUsePreferences.numberOfSignificantDigits()
                       : maxNumberOfDigits;

      char *nextCursor = pushSerializedExpression(cursor, e, digits);
      if (nextCursor == k_pushError) {
        nextCursor = pushUndefined(cursor);
        if (nextCursor == k_pushError) {
          return errorPushUndefined(heightComputer);
        }
      }
      cursor = nextCursor;
    }

    /* All data has been appended, store the pointer to the end of the
     * calculation. */
    assert(cursor < pointerArea() - sizeof(Calculation *));
    pointerArray()[-1] = cursor;

    // Compute the calculation heights
    Calculation *newCalculation =
        reinterpret_cast<Calculation *>(endOfCalculations());
    SetCalculationHeights(newCalculation, heightComputer, context);

    /* Now that the calculation is fully built, we can finally update
     * m_numberOfCalculations. As that is the only variable tracking the state
     * of the store, updating it only at the end of the push ensures that,
     * should an interruption occur, all the temporary states are silently
     * discarded and no ill-formed Calculation is stored. */
    m_numberOfCalculations++;
    return ExpiringPointer(newCalculation);
  } else {
    context->tidyDownstreamPoolFrom();
    return nullptr;
  }
}

void CalculationStore::recomputeHeightsIfPreferencesHaveChanged(
    Poincare::Preferences *preferences, HeightComputer heightComputer) {
  // Track settings that might invalidate HistoryCells heights
  if (m_inUsePreferences.combinatoricSymbols() ==
          preferences->combinatoricSymbols() &&
      m_inUsePreferences.numberOfSignificantDigits() ==
          preferences->numberOfSignificantDigits() &&
      m_inUsePreferences.logarithmBasePosition() ==
          preferences->logarithmBasePosition()) {
    return;
  }
  m_inUsePreferences = *preferences;
  for (int i = 0; i < numberOfCalculations(); i++) {
    /* The void context is used since there is no reasons for the
     * heightComputer to resolve symbols */
    SetCalculationHeights(calculationAtIndex(i).pointer(), heightComputer,
                          nullptr);
  }
}

// Private

char *CalculationStore::endOfCalculationAtIndex(int index) const {
  assert(0 <= index && index < numberOfCalculations());
  char *res = pointerArray()[index];
  /* Make sure the calculation pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
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

ExpiringPointer<Calculation> CalculationStore::errorPushUndefined(
    HeightComputer heightComputer) {
  assert(numberOfCalculations() == 0);
  char *cursor = pushUndefined(m_buffer);
  assert(m_buffer < cursor &&
         cursor <= m_buffer + m_bufferSize - sizeof(Calculation *));
  *(pointerArray() - 1) = cursor;
  Calculation *ptr = reinterpret_cast<Calculation *>(m_buffer);
  /* The void context is used since there is no reasons for the
   * heightComputer to resolve symbols */
  SetCalculationHeights(ptr, heightComputer, nullptr);
  m_numberOfCalculations = 1;
  return ExpiringPointer<Calculation>(ptr);
}

char *CalculationStore::pushEmptyCalculation(char *location) {
  while (spaceForNewCalculations(location) < Calculation::k_minimalSize) {
    if (numberOfCalculations() == 0) {
      return k_pushError;
    }
    location -= deleteOldestCalculation(location);
  }
  new (location) Calculation();
  return location + sizeof(Calculation);
}

char *CalculationStore::pushSerializedExpression(
    char *location, Expression e, int numberOfSignificantDigits) {
  while (true) {
    int availableSize = spaceForNewCalculations(location);
    int length = availableSize > 0
                     ? PoincareHelpers::Serialize(e, location, availableSize,
                                                  numberOfSignificantDigits)
                     : 0;
    if (length + 1 < availableSize) {
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

Expression CalculationStore::EnhanceUserInput(Expression inputExpression,
                                              Expression ansExpression) {
  // Replace Ans
  inputExpression =
      inputExpression.replaceSymbolWithExpression(Symbol::Ans(), ansExpression);
  /* Add an angle unit in trigonometric functions if the user could have
   * forgotten to change the angle unit in the preferences.
   * Ex: If angleUnit = rad, cos(4)->cos(4rad)
   *     If angleUnit = deg, cos(π)->cos(π°)
   * */
  inputExpression = Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
      inputExpression, Preferences::sharedPreferences->angleUnit());
  return inputExpression;
}

}  // namespace Calculation
