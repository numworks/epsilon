#ifndef POINCARE_EXPRESSION_DIMENSION_H
#define POINCARE_EXPRESSION_DIMENSION_H

#include <poincare_junior/src/memory/tree.h>

#include "unit.h"

namespace PoincareJ {

struct MatrixDimension {
  uint8_t rows;
  uint8_t cols;
};

struct UnitDimension {
  DimensionVector vector;
  // Only one representative is needed for now.
  const UnitRepresentative* representative;
};

struct Dimension {
  enum class Type {
    Scalar,
    Matrix,
    Unit,
  };

  Dimension() : type(Type::Scalar){};
  Dimension(MatrixDimension iMatrix) : type(Type::Matrix), matrix(iMatrix){};
  Dimension(UnitDimension iUnit) : type(Type::Unit), unit(iUnit){};

  static Dimension Scalar() { return Dimension(); }
  static Dimension Matrix(uint8_t rows, uint8_t cols) {
    return Dimension({.rows = rows, .cols = cols});
  }
  static Dimension Unit(DimensionVector vector,
                        const UnitRepresentative* representative) {
    return Dimension({.vector = vector, .representative = representative});
  }
  static Dimension Unit(const Tree* unit);

  bool operator==(const Dimension& other) const;
  bool operator!=(const Dimension& other) const { return !(*this == other); };

  bool isSanitized() const {
    return !(isMatrix() && matrix.rows * matrix.cols == 0) &&
           !(isUnit() && unit.vector.isEmpty());
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
  bool hasNonKelvinTemperatureUnit() const {
    return type == Type::Unit &&
           IsNonKelvinTemperatureRepresentative(unit.representative);
  }
  static bool IsNonKelvinTemperatureRepresentative(
      const UnitRepresentative* representative) {
    return representative == Unit::k_temperatureRepresentatives +
                                 Unit::k_celsiusRepresentativeIndex ||
           representative == Unit::k_temperatureRepresentatives +
                                 Unit::k_fahrenheitRepresentativeIndex;
  }

  static Dimension GetDimension(const Tree* t);
  static bool DeepCheckDimensions(const Tree* t);

  Type type;
  union {
    MatrixDimension matrix;
    UnitDimension unit;
    // TODO lists
  };
};

}  // namespace PoincareJ

#endif
