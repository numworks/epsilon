#ifndef POINCARE_EXPRESSION_DIMENSION_H
#define POINCARE_EXPRESSION_DIMENSION_H

#include <poincare_junior/src/memory/tree.h>

#include "unit_representatives.h"

namespace PoincareJ {

struct MatrixDimension {
  uint8_t rows;
  uint8_t cols;
};

struct UnitDimension {
  Units::DimensionVector vector;
  // Only one representative is needed for now.
  const Units::Representative* representative;
};

struct Dimension {
  enum class Type {
    Scalar,
    Matrix,
    Unit,
    Boolean,
    Point,
  };

  Dimension(Type type = Type::Scalar) : type(type) {
    assert(type == Type::Scalar || type == Type::Boolean ||
           type == Type::Point);
  };
  Dimension(MatrixDimension iMatrix) : type(Type::Matrix), matrix(iMatrix){};
  Dimension(UnitDimension iUnit) : type(Type::Unit), unit(iUnit){};

  static Dimension Scalar() { return Dimension(Type::Scalar); }
  static Dimension Boolean() { return Dimension(Type::Boolean); }
  static Dimension Point() { return Dimension(Type::Point); }
  static Dimension Matrix(uint8_t rows, uint8_t cols) {
    return Dimension({.rows = rows, .cols = cols});
  }
  static Dimension Unit(Units::DimensionVector vector,
                        const Units::Representative* representative) {
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
  bool isBoolean() const { return type == Type::Boolean; }
  bool isPoint() const { return type == Type::Point; }
  bool isSquareMatrix() const {
    return isMatrix() && matrix.rows == matrix.cols;
  }
  bool isVector() const {
    return isMatrix() && (matrix.rows == 1 || matrix.cols == 1);
  }
  bool isAngleUnit() const {
    return isUnit() && unit.vector.angle != 0 && unit.vector.supportSize() == 1;
  }
  bool isSimpleAngleUnit() const {
    return isAngleUnit() && unit.vector.angle == 1;
  }
  bool hasNonKelvinTemperatureUnit() const {
    return isUnit() &&
           IsNonKelvinTemperatureRepresentative(unit.representative);
  }
  static bool IsNonKelvinTemperatureRepresentative(
      const Units::Representative* representative) {
    return representative == &Units::Temperature::representatives.celsius ||
           representative == &Units::Temperature::representatives.fahrenheit;
  }

  // Return -1 if tree isn't a list.
  static int GetListLength(const Tree* t);
  static bool DeepCheckListLength(const Tree* t);
  static Dimension GetDimension(const Tree* t);
  static bool DeepCheckDimensions(const Tree* t);

  Type type;
  union {
    MatrixDimension matrix;
    UnitDimension unit;
  };
};

}  // namespace PoincareJ

#endif
