#ifndef POINCARE_EXPRESSION_DIMENSION_VECTOR_H
#define POINCARE_EXPRESSION_DIMENSION_VECTOR_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <array>

namespace Poincare::Internal {

class Tree;

namespace Units {

struct SIVector {
  constexpr static uint8_t k_numberOfBaseUnits = 8;
  // Operators
  bool operator==(const SIVector&) const = default;
  bool operator!=(const SIVector&) const = default;
  // SupportSize is defined as the number of distinct base units.
  constexpr size_t supportSize() const {
    size_t supportSize = 0;
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      if (coefficientAtIndex(i) == 0) {
        continue;
      }
      supportSize++;
    }
    return supportSize;
  }
  constexpr bool isSI() const {
    size_t numberOfOnes = 0;
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      if (coefficientAtIndex(i) == 1) {
        numberOfOnes++;
      } else if (coefficientAtIndex(i) != 0) {
        return false;
      }
    }
    return numberOfOnes == 1;
  }
  constexpr bool isEmpty() const { return supportSize() == 0; }
  constexpr static SIVector Empty() { return {}; }

  static SIVector FromBaseUnits(const Tree* baseUnits);
  // Push SI units matching the vector
  Tree* toBaseUnits() const;

  constexpr void addAllCoefficients(const SIVector other, int8_t factor) {
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      setCoefficientAtIndex(
          i, coefficientAtIndex(i) + other.coefficientAtIndex(i) * factor);
    }
  }

  constexpr void setCoefficientAtIndex(uint8_t i, int8_t coefficient) {
    assert(i < k_numberOfBaseUnits);
    int8_t* coefficientsAddresses[] = {&time,
                                       &distance,
                                       &angle,
                                       &mass,
                                       &current,
                                       &temperature,
                                       &amountOfSubstance,
                                       &luminousIntensity};
    static_assert(std::size(coefficientsAddresses) == k_numberOfBaseUnits);
    *(coefficientsAddresses[i]) = coefficient;
  }

  constexpr void setCoefficientAtIndex(uint8_t i, int coefficient) {
    assert(coefficient <= INT8_MAX && coefficient >= INT8_MIN);
    setCoefficientAtIndex(i, static_cast<int8_t>(coefficient));
  }

  constexpr int8_t coefficientAtIndex(uint8_t i) const {
    assert(i < k_numberOfBaseUnits);
    const int8_t coefficients[] = {time,
                                   distance,
                                   angle,
                                   mass,
                                   current,
                                   temperature,
                                   amountOfSubstance,
                                   luminousIntensity};
    static_assert(std::size(coefficients) == k_numberOfBaseUnits);
    return coefficients[i];
  }

  int8_t time = 0;
  int8_t distance = 0;
  int8_t angle = 0;
  int8_t mass = 0;
  int8_t current = 0;
  int8_t temperature = 0;
  int8_t amountOfSubstance = 0;
  int8_t luminousIntensity = 0;
};
static_assert(sizeof(SIVector) ==
              sizeof(uint8_t) * SIVector::k_numberOfBaseUnits);
static_assert(SIVector::Empty().isEmpty());

}  // namespace Units
}  // namespace Poincare::Internal

#endif
