#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include <apps/constant.h>
#include <apps/shared/expiring_pointer.h>
#include <poincare/old/pool_variable_context.h>
#include <poincare/preferences.h>
#include <stddef.h>

#include "calculation.h"

namespace Calculation {

struct OutputExpressions {
  Poincare::UserExpression exact;
  Poincare::UserExpression approximate;
};

// clang-format off
/*
  To optimize the storage space, we use one big buffer for all calculations.
  The calculations are stored one after another while pointers to the end of each
  calculation are stored at the end of the buffer, in the opposite direction.
  By doing so, we can memoize every calculation entered while not limiting
  the number of calculation stored in the buffer.

  If the remaining space is too small for storing a new calculation, we
  delete the oldest one.

 Memory layout :
                                                                <- Available space for new calculations ->
+--------------------------------------------------------------------------------------------------------------------+
|               |               |               |               |                                        |  |  |  |  |
| Calculation 3 | Calculation 2 | Calculation 1 | Calculation O |                                        |p0|p1|p2|p3|
|     Oldest    |               |               |               |                                        |  |  |  |  |
+--------------------------------------------------------------------------------------------------------------------+
^               ^               ^               ^               ^                                        ^
m_buffer        p3              p2              p1              p0                                       a

a = pointerArea()

Each calculation in the buffer is composed of a Calculation instance directly
followed by some Poincare expressions, one for each element of the calculation
(in order: input, exact output, approximate output). See the m_trees member
variable of Calculation for more details on this memory layout.
*/
// clang-format on

class CalculationStore {
 public:
  CalculationStore(char* buffer, size_t bufferSize);

  /* A Calculation does not count toward the number while it is being built and
   * filled. */
  int numberOfCalculations() const { return m_numberOfCalculations; }
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int index) const;
  Poincare::UserExpression ansExpression(Poincare::Context* context) const;

  /* TODO: clarify the semantics: the internal value (tree) that "expression"
   * points to is modified, and "expression" is also returned. This can be
   * confusing as the expression is both returned and modified in place. */
  Poincare::UserExpression replaceAnsInExpression(
      Poincare::UserExpression expression, Poincare::Context* context) const;
  size_t bufferSize() const { return m_bufferSize; }
  size_t remainingBufferSize() const {
    return spaceForNewCalculations(endOfCalculations()) + sizeof(Calculation*);
  }

  Shared::ExpiringPointer<Calculation> push(Poincare::Layout input,
                                            Poincare::Context* context);
  void deleteCalculationAtIndex(int index) {
    privateDeleteCalculationAtIndex(index, endOfCalculations());
  }
  void deleteAll() { m_numberOfCalculations = 0; }
  bool preferencesHaveChanged();

  Poincare::PoolVariableContext createAnsContext(Poincare::Context* context);

 private:
  static constexpr char* k_pushErrorLocation = nullptr;
  static constexpr size_t k_pushErrorSize = 0;

  static constexpr size_t neededSizeForCalculation(size_t sizeOfExpressions) {
    /* See the "memory layout" section in the description of the
     * CalculationStore class for more details on how calculations are stored.
     */
    return sizeof(Calculation) + sizeOfExpressions + sizeof(Calculation*);
  }

  char* pointerArea() const {
    return m_buffer + m_bufferSize -
           m_numberOfCalculations * sizeof(Calculation*);
  }
  char** pointerArray() const {
    return reinterpret_cast<char**>(pointerArea());
  }
  char* endOfCalculations() const {
    return numberOfCalculations() == 0 ? m_buffer : endOfCalculationAtIndex(0);
  }
  char* endOfCalculationAtIndex(int index) const;
  /* Account for the size of an additional pointer at the end of the buffer. */
  size_t spaceForNewCalculations(const char* currentEndOfCalculations) const;

  size_t privateDeleteCalculationAtIndex(int index, char* shiftedMemoryEnd);
  size_t deleteOldestCalculation(char* endOfTemporaryData) {
    return privateDeleteCalculationAtIndex(numberOfCalculations() - 1,
                                           endOfTemporaryData);
  }

  /* Make space for calculation. Update the location if some older calculations
   * are cleared. */
  void getEmptySpace(char** location, size_t neededSize);

  struct CalculationElements {
    Poincare::UserExpression input;
    OutputExpressions outputs;
    Poincare::Preferences::ComplexFormat complexFormat;

    size_t size() const {
      return input.size() + outputs.exact.size() + outputs.approximate.size();
    }
  };

  Poincare::UserExpression parseInput(Poincare::Layout inputLayout,
                                      Poincare::Context* context);

  CalculationElements computeAndProcess(Poincare::Expression inputExpression,
                                        Poincare::Context* context);

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
   * Note: this method assumes that enough space was freed after "location" to
   * contain the new calculation. */
  Calculation* pushCalculation(const CalculationElements& calculationToPush,
                               char** location);

  char* const m_buffer;
  const size_t m_bufferSize;
  int m_numberOfCalculations;
  Poincare::Preferences m_inUsePreferences;
};

}  // namespace Calculation

#endif
