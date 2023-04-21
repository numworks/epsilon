#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include <apps/shared/expiring_pointer.h>
#include <poincare/preferences.h>
#include <stddef.h>

#include "calculation.h"

namespace Calculation {
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
*/
// clang-format on

class CalculationStore {
 public:
  using HeightComputer = KDCoordinate (*)(Calculation *, Poincare::Context *,
                                          bool);

  CalculationStore(char *buffer, size_t bufferSize);

  /* A Calculation does not count toward the number while it is being built and
   * filled. */
  int numberOfCalculations() const { return m_numberOfCalculations; }
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int index) const;
  Poincare::Expression ansExpression(Poincare::Context *context) const;
  size_t bufferSize() const { return m_bufferSize; }
  size_t remainingBufferSize() const {
    return spaceForNewCalculations(endOfCalculations()) + sizeof(Calculation *);
  }

  Shared::ExpiringPointer<Calculation> push(const char *text,
                                            Poincare::Context *context,
                                            HeightComputer heightComputer);
  void deleteCalculationAtIndex(int index) {
    privateDeleteCalculationAtIndex(index, endOfCalculations());
  }
  void deleteAll() { m_numberOfCalculations = 0; }
  void recomputeHeightsIfPreferencesHaveChanged(
      Poincare::Preferences *preferences, HeightComputer heightComputer);

 private:
  static constexpr char *k_pushError = nullptr;

  static void SetCalculationHeights(Calculation *calculation,
                                    HeightComputer heightComputer,
                                    Poincare::Context *context) {
    calculation->setHeights(heightComputer(calculation, context, false),
                            heightComputer(calculation, context, true));
  }

  char *pointerArea() const {
    return m_buffer + m_bufferSize -
           m_numberOfCalculations * sizeof(Calculation *);
  }
  char **pointerArray() const {
    return reinterpret_cast<char **>(pointerArea());
  }
  char *endOfCalculations() const {
    return numberOfCalculations() == 0 ? m_buffer : endOfCalculationAtIndex(0);
  }
  char *endOfCalculationAtIndex(int index) const;
  /* Account for the size of an additional pointer at the end of the buffer. */
  size_t spaceForNewCalculations(char *currentEndOfCalculations) const {
    return (pointerArea() - currentEndOfCalculations) - sizeof(Calculation *);
  }

  size_t privateDeleteCalculationAtIndex(int index, char *shiftedMemoryEnd);
  size_t deleteOldestCalculation(char *endOfTemporaryData) {
    return privateDeleteCalculationAtIndex(numberOfCalculations() - 1,
                                           endOfTemporaryData);
  }
  Shared::ExpiringPointer<Calculation> errorPushUndefined(
      HeightComputer heightComputer);

  /* Push helper methods return a pointer to the end of the pushed content, or
   * k_pushError if the content was not pushed. */
  char *pushEmptyCalculation(char *location);
  char *pushSerializedExpression(char *location, Poincare::Expression e,
                                 int numberOfSignificantDigits);
  char *pushUndefined(char *location);

  char *const m_buffer;
  const size_t m_bufferSize;
  int m_numberOfCalculations;
  Poincare::Preferences m_inUsePreferences;
};

}  // namespace Calculation

#endif
