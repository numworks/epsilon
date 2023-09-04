#ifndef POINCARE_EXPRESSION_DIMENSION_H
#define POINCARE_EXPRESSION_DIMENSION_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

// TODO: Move this class with Unit implementation
struct DimensionVector {
 public:
  constexpr static uint8_t k_numberOfBaseUnits = 8;
  // Operators
  bool operator==(const DimensionVector&) const = default;
  bool operator!=(const DimensionVector&) const = default;
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
  constexpr bool isEmpty() const { return supportSize() == 0; }
  // Return a unit's DimensionVector.
  static DimensionVector FromUnit(const Tree* unit) {
    return {.time = 1};  // TODO
  }
  constexpr static DimensionVector Empty() { return {}; }
  void addAllCoefficients(const DimensionVector other, int8_t factor) {
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      setCoefficientAtIndex(
          i, coefficientAtIndex(i) + other.coefficientAtIndex(i) * factor);
      assert(other.coefficientAtIndex(i) <= INT8_MAX / factor);
      assert(other.coefficientAtIndex(i) >= INT8_MIN / factor);
      assert(static_cast<int32_t>(coefficientAtIndex(i)) +
                 static_cast<int32_t>(other.coefficientAtIndex(i) * factor) <=
             static_cast<int32_t>(INT8_MAX));
      assert(static_cast<int32_t>(coefficientAtIndex(i)) +
                 static_cast<int32_t>(other.coefficientAtIndex(i) * factor) >=
             static_cast<int32_t>(INT8_MIN));
    }
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
  void setCoefficientAtIndex(int8_t coefficient, uint8_t i) {
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

  int8_t time = 0;
  int8_t distance = 0;
  int8_t angle = 0;
  int8_t mass = 0;
  int8_t current = 0;
  int8_t temperature = 0;
  int8_t amountOfSubstance = 0;
  int8_t luminousIntensity = 0;
};
static_assert(sizeof(DimensionVector) ==
              sizeof(uint8_t) * DimensionVector::k_numberOfBaseUnits);
static_assert(DimensionVector::Empty().isEmpty());

struct RowColVector {
  uint8_t rows;
  uint8_t cols;
};

struct Dimension {
  enum class Type {
    Scalar,
    Matrix,
    Unit,
  };

  Dimension() : type(Type::Scalar){};
  Dimension(RowColVector iMatrix) : type(Type::Matrix), matrix(iMatrix){};
  Dimension(DimensionVector iUnit) : type(Type::Unit), unit(iUnit){};

  static Dimension Scalar() { return Dimension(); }
  static Dimension Matrix(uint8_t rows, uint8_t cols) {
    return Dimension({.rows = rows, .cols = cols});
  }
  static Dimension Unit(DimensionVector vector) { return Dimension(vector); }
  static Dimension Unit(const Tree* unit) {
    return Dimension(DimensionVector::FromUnit(unit));
  }

  bool operator==(const Dimension& other) const;
  bool operator!=(const Dimension& other) const { return !(*this == other); };

  bool isSanitized() const {
    return !(isMatrix() && matrix.rows * matrix.cols == 0) &&
           !(isUnit() && unit.isEmpty());
  }

  bool isScalar() const { return type == Type::Scalar; }
  bool isMatrix() const { return type == Type::Matrix; }
  bool isUnit() const { return type == Type::Unit; }
  bool isSquareMatrix() const {
    return type == Type::Matrix && matrix.rows == matrix.cols;
  }
  bool isVector() const {
    return type == Type::Matrix && (matrix.rows == 1 || matrix.cols == 1);
  }

  static Dimension GetDimension(const Tree* t);
  static bool DeepCheckDimensions(const Tree* t);

  Type type;
  union {
    RowColVector matrix;
    DimensionVector unit;
    // TODO lists
  };
};

}  // namespace PoincareJ

#endif
