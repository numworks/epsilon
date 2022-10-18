#include "calculation_store.h"
#include "../shared/poincare_helpers.h"
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/rational.h>
#include <poincare/store.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include "../exam_mode_configuration.h"
#include <assert.h>
#include <ion/circuit_breaker.h>
#include <apps/shared/utils.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

CalculationStore::CalculationStore(char * buffer, int size) :
  m_buffer(buffer),
  m_bufferSize(size),
  m_calculationAreaEnd(m_buffer),
  m_numberOfCalculations(0),
  m_inUsePreferences(*Poincare::Preferences::sharedPreferences())
{
  assert(m_buffer != nullptr);
  assert(m_bufferSize > 0);
}

// Returns an expiring pointer to the calculation of index i
ExpiringPointer<Calculation> CalculationStore::calculationAtIndex(int i) {
  assert(i >= 0 && i < m_numberOfCalculations);
  // m_buffer is the address of the oldest calculation in calculation store
  Calculation * c = (Calculation *) m_buffer;
  if (i != m_numberOfCalculations-1) {
    // The calculation we want is not the oldest one so we get its pointer
    c = *reinterpret_cast<Calculation**>(addressOfPointerToCalculationOfIndex(i+1));
  }
  return ExpiringPointer<Calculation>(c);
}

// Pushes an expression in the store
ExpiringPointer<Calculation> CalculationStore::push(const char * text, Context * context, HeightComputer heightComputer) {
  /* TODO: we could refine this UserCircuitBreaker. When interrupted during
   * simplification, we could still try to display the approximate result? When
   * interrupted during approximation, we could at least display the exact
   * result. If we do so, don't forget to force the Calculation sign to be
   * approximative to avoid long computation to determine it.
   */

  // Store a safe state to get back on in case of interruption.
  char * addressOfCalculation = m_calculationAreaEnd;
  int totalOlderCalculations = m_numberOfCalculations;

  UserCircuitBreakerCheckpoint checkpoint;
  if (CircuitBreakerRun(checkpoint)) {
    /* Compute ans now, before the buffer is updated and before the calculation
     * might be deleted */
    Expression ans = ansExpression(context);

    /* Prepare the buffer for the new calculation.
     * The minimal size to store the new calculation is the minimal size of a
     * calculation plus the pointer to its end */
    constexpr int minimalSize = Calculation::k_minimalSize + sizeof(Calculation *);
    assert(m_bufferSize > minimalSize);
    while (remainingBufferSize() < minimalSize) {
      /* If there is no more space to store a calculation, we delete the oldest
       * one. */
      Ion::CircuitBreaker::lock();
      deleteOldestCalculation();
      addressOfCalculation = m_calculationAreaEnd;
      totalOlderCalculations = m_numberOfCalculations;
      Ion::CircuitBreaker::unlock();
    }

    size_t calcSize;
    // Add the beginning of the calculation
    {
      /* Copy the begining of the calculation. The calculation minimal size is
       * available, so this memmove will not overide anything. */
      Calculation newCalc = Calculation();
      calcSize = sizeof(newCalc);
      memcpy(m_calculationAreaEnd, &newCalc, calcSize);
      // Update the end of the calculation storage area
      m_calculationAreaEnd += calcSize;
      // Storing the pointer of the end of the new calculation
      memcpy(beginingOfMemoizationArea()-sizeof(Calculation*), &m_calculationAreaEnd, sizeof(m_calculationAreaEnd));
    }
    // The new calculation is now stored
    m_numberOfCalculations++;
    // Getting the addresses of the begining of the free space
    char * beginingOfFreeSpace = m_calculationAreaEnd;
    // Getting the addresses of the end of the free space
    char * endOfFreeSpace = beginingOfMemoizationArea();

    // Add the input expression
    {
      /* We do not store directly the text entered by the user because we do not
       * want to keep Ans symbol in the calculation store. */
      Expression input = Expression::Parse(text, context).replaceSymbolWithExpression(Symbol::Ans(), ans);
      Ion::CircuitBreaker::lock();
      char * previousAreaEnd = m_calculationAreaEnd;
      if (!pushSerializedExpression(input, &beginingOfFreeSpace, endOfFreeSpace)) {
        assert(m_numberOfCalculations == 1);
        addressOfCalculation = m_buffer;
        totalOlderCalculations = 0;
        /* Silent static analyzer warning: these variables are used in case of
         * circuit breaker interruption (longjmp) but the compiler analyzer gets
         * lost in such cases. */
        (void)addressOfCalculation;
        (void)totalOlderCalculations;
        Ion::CircuitBreaker::unlock();
        /* If the input does not fit in the store (event if the current
         * calculation is the only calculation), just replace the calculation
         * with undef. */
        return emptyStoreAndPushUndef(context, heightComputer);
      }
      if (m_calculationAreaEnd != previousAreaEnd) {
        // Calculations have been deleted
        assert(m_calculationAreaEnd < previousAreaEnd);
        addressOfCalculation -= previousAreaEnd - m_calculationAreaEnd;
        totalOlderCalculations = m_numberOfCalculations - 1;
      }
      Ion::CircuitBreaker::unlock();

      // Update the end of the calculation storage area
      m_calculationAreaEnd = beginingOfFreeSpace;
      // Update the pointer of the end of the new calculation
      memcpy(beginingOfMemoizationArea(), &m_calculationAreaEnd, sizeof(m_calculationAreaEnd));
    }

    // Compute and serialize the outputs
    /* The serialized outputs are:
     * - the exact output
     * - the approximate output with the maximal number of significant digits
     * - the approximate output with the displayed number of significant digits
     */
    {
      // Outputs hold exact output, approximate output and its duplicate
      constexpr static int numberOfOutputs = Calculation::k_numberOfExpressions - 1;
      Expression input;
      Expression outputs[numberOfOutputs] = {Expression(), Expression(), Expression()};
      PoincareHelpers::ParseAndSimplifyAndApproximate(addressOfCalculation + calcSize, &input, &(outputs[0]), &(outputs[1]), context);
      if (outputs[0].type() == ExpressionNode::Type::Store) {
        /* When a input contains a store it is kept by the reduction in the
         * exact output and the actual store is performed here. The global
         * context will perform the store and ensure that no symbol is kept in
         * the definition of a variable.
         * Once this is done we replace the output with the stored expression
         * and approximate it to mimic the behaviour of normal computations.
         */
        Store store = static_cast<Store&>(outputs[0]);
        bool isVariable = store.childAtIndex(1).type() == Poincare::ExpressionNode::Type::Symbol;
        Expression exactValue = store.childAtIndex(0);
        Expression approximatedValue = PoincareHelpers::ApproximateKeepingUnits<double>(exactValue, context);
        if (isVariable && Utils::ShouldOnlyDisplayApproximation(input, exactValue, context)) {
          store.replaceChildAtIndexInPlace(0, approximatedValue);
        }
        outputs[0] = store.storeValueForSymbol(context);
        outputs[1] = approximatedValue;
      }
      if (ExamModeConfiguration::unitsAreForbidden() && outputs[1].hasUnit()) {
        /* Hide results with units on units if required by the exam mode
         * configuration. */
        outputs[1] = Undefined::Builder();
      }
      outputs[2] = outputs[1];
      int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits;
      for (int i = 0; i < numberOfOutputs; i++) {
        if (i == numberOfOutputs - 1) {
          numberOfSignificantDigits = Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits();
        }
        char * previousAreaEnd = m_calculationAreaEnd;
        Ion::CircuitBreaker::lock();
        if (!pushSerializedExpression(outputs[i], &beginingOfFreeSpace, endOfFreeSpace, numberOfSignificantDigits)) {
          assert(m_numberOfCalculations == 1);
          addressOfCalculation = m_buffer;
          totalOlderCalculations = 0;
          Ion::CircuitBreaker::unlock();
          /* If the exact/approximate output does not fit in the store (even if
           * the current calculation is the only one), replace the output with
           * undef if it fits, else replace the whole calculation with undef. */
          Expression undef = Undefined::Builder();
          if (!pushSerializedExpression(undef, &beginingOfFreeSpace, endOfFreeSpace)) {
            assert(m_numberOfCalculations == 1);
            // Replace the whole calculation with undef.
            return emptyStoreAndPushUndef(context, heightComputer);
          }
        }
        if (m_calculationAreaEnd != previousAreaEnd) {
          // Calculations have been deleted
          assert(m_calculationAreaEnd < previousAreaEnd);
          addressOfCalculation -= previousAreaEnd - m_calculationAreaEnd;
          totalOlderCalculations = m_numberOfCalculations - 1;
        }
        Ion::CircuitBreaker::unlock();
        // Update the end of the calculation storage area
        m_calculationAreaEnd = beginingOfFreeSpace;
        // Update the pointer of the end of the new calculation
        memcpy(beginingOfMemoizationArea(), &m_calculationAreaEnd, sizeof(m_calculationAreaEnd));
      }
    }

    // Retrieve pointer to inserted Calculation.
    assert(addressOfCalculation == ((m_numberOfCalculations == 1) ? m_buffer : *reinterpret_cast<char **>(addressOfPointerToCalculationOfIndex(1))));
    ExpiringPointer<Calculation> calculation = ExpiringPointer<Calculation>(reinterpret_cast<Calculation *>(addressOfCalculation));

    /* Heights are computed now to make sure that the display output is decided
     * accordingly to the remaining size in the Poincare pool. Once it is, it
     * can't change anymore: the calculation heights are fixed which ensures that
     * scrolling computation is right. */
    calculation->setHeights(
        heightComputer(calculation.pointer(), context, false),
        heightComputer(calculation.pointer(), context, true));

    /* Silent static analyzer warning: these variables are used in case of
     * circuit breaker interruption (longjmp) but the compiler analyzer gets
     * lost in such cases. */
    (void)addressOfCalculation;
    (void)totalOlderCalculations;
    return calculation;
  } else {
    // Restore Calculation store in a safe state.
    Ion::CircuitBreaker::lock();
    m_calculationAreaEnd = addressOfCalculation;
    m_numberOfCalculations = totalOlderCalculations;
    Ion::CircuitBreaker::unlock();
    context->tidyDownstreamPoolFrom();
    return nullptr;
  }
}

// Delete the calculation of index i
void CalculationStore::deleteCalculationAtIndex(int i) {
  assert(i >= 0 && i < m_numberOfCalculations);
  if (i == 0) {
    ExpiringPointer<Calculation> lastCalculationPointer = calculationAtIndex(0);
    Ion::CircuitBreaker::lock();
    m_calculationAreaEnd = (char *)(lastCalculationPointer.pointer());
    m_numberOfCalculations--;
    Ion::CircuitBreaker::unlock();
    return;
  }
  char * calcI = (char *)calculationAtIndex(i).pointer();
  char * nextCalc = (char *) calculationAtIndex(i-1).pointer();
  assert(m_calculationAreaEnd >= nextCalc);
  size_t slidingSize = m_calculationAreaEnd - nextCalc;
  Ion::CircuitBreaker::lock();
  // Slide the i-1 most recent calculations right after the i+1'th
  memmove(calcI, nextCalc, slidingSize);
  m_calculationAreaEnd -= nextCalc - calcI;
  // Recompute pointer to calculations after the i'th
  recomputeMemoizedPointersAfterCalculationIndex(i);
  m_numberOfCalculations--;
  Ion::CircuitBreaker::unlock();
}

// Delete the oldest calculation in the store and returns the amount of space freed by the operation
size_t CalculationStore::deleteOldestCalculation() {
  char * oldBufferEnd = m_calculationAreaEnd;
  deleteCalculationAtIndex(numberOfCalculations()-1);
  char * newBufferEnd = m_calculationAreaEnd;
  return oldBufferEnd - newBufferEnd;
}

// Delete all calculations
void CalculationStore::deleteAll() {
  Ion::CircuitBreaker::lock();
  m_calculationAreaEnd = m_buffer;
  m_numberOfCalculations = 0;
  Ion::CircuitBreaker::unlock();
}

// Replace "Ans" by its expression
Expression CalculationStore::ansExpression(Context * context) {
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
    Expression approximate = mostRecentCalculation->approximateOutput(Calculation::NumberOfSignificantDigits::Maximal);
    if (approximate.isUninitialized()) {
      return defaultAns;
    }
    return approximate;
  }
  /* Special case: If exact output was hidden, it should not be accessible using
   * ans, unless it is equal to an approximation that is neither undefined nor
   * nonreal. */
  const char * exactOutputText = mostRecentCalculation->exactOutputText();
  const char * approximateOutputText = mostRecentCalculation->approximateOutputText(Calculation::NumberOfSignificantDigits::UserDefined);
  if (mostRecentCalculation->displayOutput(context) == Calculation::DisplayOutput::ApproximateOnly && (strcmp(approximateOutputText, exactOutputText) != 0 ||
      exactOutput.type() == ExpressionNode::Type::Nonreal || exactOutput.type() == ExpressionNode::Type::Undefined)) {
    return input;
  }
  return exactOutput;
}

void CalculationStore::recomputeHeights(HeightComputer heightComputer) {
  for (Calculation * calculation : *this) {
    calculation->setHeights(
      /* The void context is used since there is no reasons for the
       * heightComputer to resolve symbols */
      heightComputer(calculation, nullptr, false),
      heightComputer(calculation, nullptr, true));
  }
}

bool CalculationStore::preferencesMightHaveChanged(Poincare::Preferences * preferences) {
  // Track settings that might invalidate HistoryCells heights
  if (m_inUsePreferences.combinatoricSymbols() == preferences->combinatoricSymbols()
    && m_inUsePreferences.numberOfSignificantDigits() == preferences->numberOfSignificantDigits()) {
    return false;
  }
  m_inUsePreferences = *preferences;
  return true;
}

// Push converted expression in the buffer
bool CalculationStore::pushSerializedExpression(Expression e, char ** start, char * end, int numberOfSignificantDigits) {
  assert(end <= addressOfPointerToCalculationOfIndex(0) && *start < end && end - *start < m_bufferSize);
  /* Delete oldest calculations until the expression fits or until only this
   * calculation remains. Return true if the expression could fit. */
  while (true) {
    assert(*start > m_buffer);
    int availableSize = end - *start;
    int serializationSize = PoincareHelpers::Serialize(e, *start, availableSize, numberOfSignificantDigits);
    if (serializationSize < availableSize - 1) {
      assert(*(*start + serializationSize) == 0);
      *start += serializationSize + 1;
      return true;
    }
    if (m_numberOfCalculations == 1) {
      break;
    }
    *start -= deleteOldestCalculation();
  }
  return false;
}


Shared::ExpiringPointer<Calculation> CalculationStore::emptyStoreAndPushUndef(Context * context, HeightComputer heightComputer) {
  /* We end up here as a result of a failed calculation push. The store
   * attributes are not necessarily clean, so we need to reset them. */
  deleteAll();
  return push(Undefined::Name(), context, heightComputer);
}

// Recompute memoized pointers to the calculations after index i
void CalculationStore::recomputeMemoizedPointersAfterCalculationIndex(int index) {
  assert(index < m_numberOfCalculations);
  // Clear pointer and recompute new ones
  Calculation * c = calculationAtIndex(index).pointer();
  Calculation * nextCalc;
  while (index != 0) {
    nextCalc = c->next();
    memcpy(addressOfPointerToCalculationOfIndex(index), &nextCalc, sizeof(Calculation *));
    c = nextCalc;
    index--;
  }
}

}
