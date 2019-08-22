#include "calculation_store.h"
#include "../shared/poincare_helpers.h"
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

CalculationStore::CalculationStore() :
  m_bufferEnd(m_buffer),
  m_numberOfCalculations(0),
  m_slidedBuffer(false),
  m_indexOfFirstMemoizedCalculationPointer(0)
{
  resetMemoizedModelsAfterCalculationIndex(-1);
}

ExpiringPointer<Calculation> CalculationStore::calculationAtIndex(int i) {
  assert(!m_slidedBuffer);
  assert(i >= 0 && i < m_numberOfCalculations);
  assert(m_indexOfFirstMemoizedCalculationPointer >= 0);
  if (i >= m_indexOfFirstMemoizedCalculationPointer && i < m_indexOfFirstMemoizedCalculationPointer + k_numberOfMemoizedCalculationPointers) {
    // The calculation is within the range of memoized calculations
    Calculation * c = m_memoizedCalculationPointers[i-m_indexOfFirstMemoizedCalculationPointer];
    if (c != nullptr) {
      // The pointer was memoized
      return ExpiringPointer<Calculation>(c);
    }
    c = bufferCalculationAtIndex(i);
    m_memoizedCalculationPointers[i-m_indexOfFirstMemoizedCalculationPointer] = c;
    return c;
  }
  // Slide the memoization buffer
  if (i >= m_indexOfFirstMemoizedCalculationPointer) {
    // Slide the memoization buffer to the left
    memmove(m_memoizedCalculationPointers, m_memoizedCalculationPointers+1, (k_numberOfMemoizedCalculationPointers - 1) * sizeof(Calculation *));
    m_memoizedCalculationPointers[k_numberOfMemoizedCalculationPointers - 1] = nullptr;
    m_indexOfFirstMemoizedCalculationPointer++;
  } else {
    // Slide the memoization buffer to the right
    memmove(m_memoizedCalculationPointers+1, m_memoizedCalculationPointers, (k_numberOfMemoizedCalculationPointers - 1) * sizeof(Calculation *));
    m_memoizedCalculationPointers[0] = nullptr;
    m_indexOfFirstMemoizedCalculationPointer--;
  }
  return calculationAtIndex(i);
}

ExpiringPointer<Calculation> CalculationStore::push(const char * text, Context * context) {
  /* Compute ans now, before the buffer is slided and before the calculation
   * might be deleted */
  Expression ans = ansExpression(context);

  // Prepare the buffer for the new calculation
  int minSize = Calculation::MinimalSize();
  assert(k_bufferSize > minSize);
  while (remainingBufferSize() < minSize) {
    deleteLastCalculation();
  }
  char * newCalculationsLocation = slideCalculationsToEndOfBuffer();
  char * nextSerializationLocation = m_buffer;

  // Add the beginning of the calculation
  {
    /* Copy the begining of the calculation. The calculation minimal size is
     * available, so this memmove will not overide anything. */
    Calculation newCalc = Calculation();
    size_t calcSize = sizeof(newCalc);
    memmove(nextSerializationLocation, &newCalc, calcSize);
    nextSerializationLocation += calcSize;
  }

  /* Add the input expression.
   * We do not store directly the text entered by the user because we do not
   * want to keep Ans symbol in the calculation store. */
  const char * inputSerialization = nextSerializationLocation;
  {
    Expression input = Expression::Parse(text).replaceSymbolWithExpression(Symbol::Ans(), ans);
    if (!serializeExpression(input, nextSerializationLocation, &newCalculationsLocation)) {
      /* If the input does not fit in the store (event if the current
       * calculation is the only calculation), just replace the calculation with
       * undef. */
      return emptyStoreAndPushUndef(context);
    }
    nextSerializationLocation += strlen(nextSerializationLocation) + 1;
  }

  // Compute and serialize the outputs
  {
    Expression outputs[] = {Expression(), Expression()};
    PoincareHelpers::ParseAndSimplifyAndApproximate(inputSerialization, &(outputs[0]), &(outputs[1]), context, false);
    for (int i = 0; i < 2; i++) {
      if (!serializeExpression(outputs[i], nextSerializationLocation, &newCalculationsLocation)) {
        /* If the exat/approximate output does not fit in the store (event if the
         * current calculation is the only calculation), replace the output with
         * undef if it fits, else replace the whole calcualtion with undef. */
        Expression undef = Undefined::Builder();
        if (!serializeExpression(undef, nextSerializationLocation, &newCalculationsLocation)) {
          return emptyStoreAndPushUndef(context);
        }
      }
      nextSerializationLocation += strlen(nextSerializationLocation) + 1;
    }
  }

  // Restore the other calculations
  size_t slideSize = m_buffer + k_bufferSize - newCalculationsLocation;
  memcpy(nextSerializationLocation, newCalculationsLocation, slideSize);
  m_slidedBuffer = false;
  m_numberOfCalculations++;
  m_bufferEnd+= nextSerializationLocation - m_buffer;

  // Clean the memoization
  resetMemoizedModelsAfterCalculationIndex(-1);

  return ExpiringPointer<Calculation>(reinterpret_cast<Calculation *>(m_buffer));
}

void CalculationStore::deleteCalculationAtIndex(int i) {
  assert(i >= 0 && i < m_numberOfCalculations);
  assert(!m_slidedBuffer);
  ExpiringPointer<Calculation> calcI = calculationAtIndex(i);
  char * nextCalc = reinterpret_cast<char *>(calcI->next());
  assert(m_bufferEnd >= nextCalc);
  size_t slidingSize = m_bufferEnd - nextCalc;
  memmove((char *)(calcI.pointer()), nextCalc, slidingSize);
  m_bufferEnd -= (nextCalc - (char *)(calcI.pointer()));
  m_numberOfCalculations--;
  resetMemoizedModelsAfterCalculationIndex(i);
}

void CalculationStore::deleteAll() {
  /* We might call deleteAll because the app closed due to a pool allocation
   * failure, so we cannot assert that m_slidedBuffer is false. */
  m_slidedBuffer = false;
  m_bufferEnd = m_buffer;
  m_numberOfCalculations = 0;
  resetMemoizedModelsAfterCalculationIndex(-1);
}

void CalculationStore::tidy() {
  if (m_slidedBuffer) {
    deleteAll();
    return;
  }
  resetMemoizedModelsAfterCalculationIndex(-1);
  for (Calculation * c : *this) {
    c->tidy();
  }
}

Expression CalculationStore::ansExpression(Context * context) {
  if (numberOfCalculations() == 0) {
    return Rational::Builder(0);
  }
  ExpiringPointer<Calculation> mostRecentCalculation = calculationAtIndex(0);
  /* Special case: the exact output is a Store/Equal expression.
   * Store/Equal expression can only be at the root of an expression.
   * To avoid turning 'ans->A' in '2->A->A' or '2=A->A' (which cannot be
   * parsed), ans is replaced by the approximation output when any Store or
   * Equal expression appears. */
  bool exactOuptutInvolvesStoreEqual = mostRecentCalculation->exactOutput().recursivelyMatches([](const Expression e, Context * context) {
          return e.type() == ExpressionNode::Type::Store || e.type() == ExpressionNode::Type::Equal;
        }, context, false);
  if (mostRecentCalculation->input().recursivelyMatches(Expression::IsApproximate, context) || exactOuptutInvolvesStoreEqual) {
    return mostRecentCalculation->approximateOutput(context);
  }
  return mostRecentCalculation->exactOutput();
}

Calculation * CalculationStore::bufferCalculationAtIndex(int i) {
  int currentIndex = 0;
  for (Calculation * c : *this) {
    if (currentIndex == i) {
      return c;
    }
    currentIndex++;
  }
  assert(false);
  return nullptr;
}

bool CalculationStore::serializeExpression(Expression e, char * location, char * * newCalculationsLocation) {
  assert(m_slidedBuffer);
  return pushExpression(
      [](char * location, size_t locationSize, void * e) {
        return PoincareHelpers::Serialize(*(Expression *)e, location, locationSize) < (int)locationSize-1;
      }, &e, location, newCalculationsLocation);
}

char * CalculationStore::slideCalculationsToEndOfBuffer() {
  int calculationsSize = m_bufferEnd - m_buffer;
  char * calculationsNewPosition = m_buffer + k_bufferSize - calculationsSize;
  memmove(calculationsNewPosition, m_buffer, calculationsSize);
  m_slidedBuffer = true;
  return calculationsNewPosition;
}

size_t CalculationStore::deleteLastCalculation(const char * calculationsStart) {
  assert(m_numberOfCalculations > 0);
  size_t result;
  if (!m_slidedBuffer) {
    assert(calculationsStart == nullptr);
    const char * previousBufferEnd = m_bufferEnd;
    m_bufferEnd = lastCalculationPosition(m_buffer);
    assert(previousBufferEnd > m_bufferEnd);
    result = previousBufferEnd - m_bufferEnd;
  } else {
    assert(calculationsStart != nullptr);
    const char * lastCalc = lastCalculationPosition(calculationsStart);
    assert(*lastCalc == 0);
    result = m_buffer + k_bufferSize - lastCalc;
    memmove(const_cast<char *>(calculationsStart + result), calculationsStart, m_buffer + k_bufferSize - calculationsStart - result);
  }
  m_numberOfCalculations--;
  resetMemoizedModelsAfterCalculationIndex(-1);
  return result;
}

const char * CalculationStore::lastCalculationPosition(const char * calculationsStart) const {
  assert(calculationsStart >= m_buffer && calculationsStart < m_buffer + k_bufferSize);
  Calculation * c = reinterpret_cast<Calculation *>(const_cast<char *>(calculationsStart));
  int calculationIndex = 0;
  while (calculationIndex < m_numberOfCalculations - 1) {
    c = c->next();
  }
  return reinterpret_cast<const char *>(c);
}

bool CalculationStore::pushExpression(ValueCreator valueCreator, Expression * expression, char * location, char * * newCalculationsLocation) {
  assert(*newCalculationsLocation <= m_buffer + k_bufferSize);
  bool expressionIsPushed = false;
  while (true) {
    expressionIsPushed = valueCreator(location, *newCalculationsLocation - location, expression);
    if (expressionIsPushed || *newCalculationsLocation >= m_buffer + k_bufferSize) {
      break;
    }
    *newCalculationsLocation = *newCalculationsLocation + deleteLastCalculation();
    assert(*newCalculationsLocation <= m_buffer + k_bufferSize);
  }
  return expressionIsPushed;
}

Shared::ExpiringPointer<Calculation> CalculationStore::emptyStoreAndPushUndef(Context * context) {
  /* We end up here as a result of a failed calculation push. The store
   * attributes are not necessarily clean, so we need to reset them. */
  m_slidedBuffer = false;
  deleteAll();
  return push(Undefined::Name(), context);
}

void CalculationStore::resetMemoizedModelsAfterCalculationIndex(int index) {
  if (index < m_indexOfFirstMemoizedCalculationPointer) {
    memset(&m_memoizedCalculationPointers, 0, k_numberOfMemoizedCalculationPointers * sizeof(Calculation *));
    return;
  }
  if (index >= m_indexOfFirstMemoizedCalculationPointer + k_numberOfMemoizedCalculationPointers) {
    return;
  }
  for (int i = index - m_indexOfFirstMemoizedCalculationPointer; i < k_numberOfMemoizedCalculationPointers; i++) {
    m_memoizedCalculationPointers[i] = nullptr;
  }
}

}
