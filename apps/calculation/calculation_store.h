#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"
#include <apps/shared/expiring_pointer.h>
#include <poincare/print_float.h>

namespace Calculation {

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

m_calculationAreaEnd = p0
a = addressOfPointerToCalculation(0)
*/

class CalculationStore {
public:
  CalculationStore();
  CalculationStore(char * buffer, int size);
  typedef KDCoordinate (*HeightComputer)(Calculation * c, Poincare::Context * context, bool expanded);
  Shared::ExpiringPointer<Calculation> push(const char * text, Poincare::Context * context, HeightComputer heightComputer);

  Shared::ExpiringPointer<Calculation> calculationAtIndex(int i);
  void deleteCalculationAtIndex(int i);
  void deleteAll();
  int bufferSize() { return m_bufferSize; }
  int remainingBufferSize() const { assert(m_calculationAreaEnd >= m_buffer); return m_bufferSize - (m_calculationAreaEnd - m_buffer) - m_numberOfCalculations*sizeof(Calculation*); }
  int numberOfCalculations() const { return m_numberOfCalculations; }
  Poincare::Expression ansExpression(Poincare::Context * context);
  void recomputeHeights(HeightComputer heightComputer);
  bool preferencesMightHaveChanged(Poincare::Preferences * preferences);
  void recover();

private:
  class CalculationIterator {
  public:
    CalculationIterator(const char * c) : m_calculation(reinterpret_cast<Calculation *>(const_cast<char *>(c))) {}
    Calculation * operator*() { return m_calculation; }
    bool operator!=(const CalculationIterator& it) const { return (m_calculation != it.m_calculation); }
    CalculationIterator & operator++() {
      m_calculation = m_calculation->next();
      return *this;
    }
  protected:
    Calculation * m_calculation;
  };

  CalculationIterator begin() const { return CalculationIterator(m_buffer); }
  CalculationIterator end() const { return CalculationIterator(m_calculationAreaEnd); }

  bool pushSerializedExpression(Poincare::Expression e, char ** start, char * end, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  Shared::ExpiringPointer<Calculation> emptyStoreAndPushUndef(Poincare::Context * context, HeightComputer heightComputer);
  size_t deleteOldestCalculation();
  char * addressOfPointerToCalculationOfIndex(int i) { assert(i <= m_numberOfCalculations); return m_buffer + m_bufferSize - (m_numberOfCalculations - i)*sizeof(Calculation *);}
  void updateRecoveryData();

  // Memoization
  char * beginingOfMemoizationArea() {return addressOfPointerToCalculationOfIndex(0);};
  void recomputeMemoizedPointersAfterCalculationIndex(int index);

  char * const m_buffer;
  const int m_bufferSize;
  char * m_calculationAreaEnd;
  char * m_recoveryCalculationAreaEnd;
  int m_numberOfCalculations;
  int m_recoveryNumberOfCalculations;
  Poincare::Preferences m_inUsePreferences;
};

}

#endif
