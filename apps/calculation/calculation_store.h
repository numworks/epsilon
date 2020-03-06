#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"
#include <apps/shared/expiring_pointer.h>
#include <poincare/print_float.h>

namespace Calculation {

/* To optimize the storage space, we use one big buffer for all calculations.
 *
 * The previous solution was to keep 10 calculations, each containing 3 buffers
 * (for input and outputs). To optimize the storage, we then wanted to put all
 * outputs in a cache where they could be deleted to add a new entry, and
 * recomputed on cache miss. However, the computation depends too much on the
 * state of the memory for this to be possible. For instance:
 * 6->a
 * a+1
 * Perform some big computations that remove a+1 from the cache
 * Delete a from the variable box.
 * Scroll up to display a+1 : a does not exist anymore so the outputs won't be
 * recomputed correctly.
 *
 * Now we do not cap the number of calculations and just delete the oldests to
 * create space for a new calculation. */

class CalculationStore {
public:
  CalculationStore();
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int i);
  Shared::ExpiringPointer<Calculation> push(const char * text, Poincare::Context * context);
  void deleteCalculationAtIndex(int i);
  void deleteAll();
  int numberOfCalculations() const { return m_numberOfCalculations; }
  Poincare::Expression ansExpression(Poincare::Context * context);
  void tidy();
private:
  static constexpr int k_maxNumberOfCalculations = 25;
  static constexpr int k_bufferSize = 10 * Calculation::k_numberOfExpressions * Constant::MaxSerializedExpressionSize;

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
  CalculationIterator end() const { return CalculationIterator(m_bufferEnd); }

  Calculation * bufferCalculationAtIndex(int i);
  int remainingBufferSize() const { assert(m_bufferEnd >= m_buffer); return k_bufferSize - (m_bufferEnd - m_buffer); }
  bool pushSerializeExpression(Poincare::Expression e, char * location, char * * newCalculationsLocation, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  char * slideCalculationsToEndOfBuffer(); // returns the new position of the calculations
  size_t deleteLastCalculation(const char * calculationsStart = nullptr);
  const char * lastCalculationPosition(const char * calculationsStart) const;
  Shared::ExpiringPointer<Calculation> emptyStoreAndPushUndef(Poincare::Context * context);

  char m_buffer[k_bufferSize];
  const char * m_bufferEnd;
  int m_numberOfCalculations;
  bool m_slidedBuffer;

  // Memoization
  static constexpr int k_numberOfMemoizedCalculationPointers = 10;
  void resetMemoizedModelsAfterCalculationIndex(int index);
  int m_indexOfFirstMemoizedCalculationPointer;
  mutable Calculation * m_memoizedCalculationPointers[k_numberOfMemoizedCalculationPointers];
};

}

#endif
