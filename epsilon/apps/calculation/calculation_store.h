#pragma once

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <ion/circuit_breaker.h>
#include <omg/expiring_pointer.h>
#include <omg/store.h>
#include <poincare/pool_variable_context.h>
#include <poincare/variable_store.h>
#include <stddef.h>

#include "calculation.h"

namespace Calculation {

struct OutputExpressions {
  Poincare::UserExpression exact;
  Poincare::UserExpression approximate;
};

/* To optimize the storage space, we use one big buffer for all calculations.
 * Refer to OMG::Store to see the actual memory layout.
 *
 * Each calculation in the buffer is composed of a Calculation instance directly
 * followed by some Poincare expressions, one for each element of the
 * calculation (in order: input, exact output, approximate output). See the
 * m_trees member variable of Calculation for more details on this memory
 * layout.
 */

class CalculationStore
    : OMG::Store<Ion::CircuitBreaker::lock, Ion::CircuitBreaker::unlock> {
 public:
  CalculationStore(char* buffer, size_t bufferSize);

  /* A Calculation does not count toward the number while it is being built and
   * filled. */
  int numberOfCalculations() const { return numberOfElements(); }
  OMG::ExpiringPointer<Calculation> calculationAtIndex(int index) const;
  Poincare::UserExpression ansExpression() const;

  void replaceAnsInExpression(Poincare::UserExpression& expression) const;

  OMG::ExpiringPointer<Calculation> push(Poincare::Layout input);
  void deleteCalculationAtIndex(int index);
  bool preferencesHaveChanged();

  Poincare::PoolVariableContext createAnsContext();

  using Store::deleteAll;
  using Store::maximumSize;
  using Store::remainingSize;

 private:
  constexpr static size_t neededSizeForCalculation(size_t sizeOfExpressions) {
    /* See the "memory layout" section in the description of the
     * CalculationStore class for more details on how calculations are stored.
     */
    return sizeof(Calculation) + sizeOfExpressions;
  }

  struct CalculationElements {
    Poincare::UserExpression input;
    OutputExpressions outputs;
    bool hasReductionFailure;
    Poincare::ComplexFormat complexFormat;

    size_t sizeOfTrees() const {
      return input.tree()->treeSize() + outputs.exact.tree()->treeSize() +
             outputs.approximate.tree()->treeSize();
    }
  };

  Poincare::UserExpression parseInput(Poincare::Layout inputLayout);

  CalculationElements computeAndProcess(
      Poincare::UserExpression inputExpression);

  /* Push an empty calculation at a certain location. Assumes there is enough
   * space to push an empty calculation. Returns a pointer to the new
   * Calculation. */
  Calculation* pushEmptyCalculation(char** location);

  /* Push helper method that takes the current location and updates it to the
   * end of the pushed content. Assumes there is enough space to push the
   * expression tree. Returns the size of the pushed content. */
  size_t pushExpressionTree(char** location, Poincare::UserExpression e);

  /* Push a Calculation with its expressions (input, exact and approximate
   * output), and updates the calculation buffer to hold this new calculation
   * (see the memory layout in the CalculationStore class description).
   * Note: this method assumes that enough space was freed to contain the new
   * calculation. */
  Calculation* pushCalculation(const CalculationElements& calculationToPush);

  struct HeightSensitivePreferences {
    uint8_t numberOfSignificantDigits;
    Poincare::Preferences::CombinatoricSymbols combinatoricSymbols;
    Poincare::Preferences::LogarithmBasePosition logarithmBasePosition;
    bool operator==(const HeightSensitivePreferences&) const = default;
  };

  HeightSensitivePreferences getCurrentPreferences() {
    return {
        GlobalPreferences::SharedGlobalPreferences()
            ->numberOfSignificantDigits(),
        GlobalPreferences::SharedGlobalPreferences()->combinatoricSymbols(),
        GlobalPreferences::SharedGlobalPreferences()->logarithmBasePosition(),
    };
  }

  HeightSensitivePreferences m_inUsePreferences;
};

}  // namespace Calculation
