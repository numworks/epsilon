#ifndef POINCARE_EXPRESSION_DIMENSION_H
#define POINCARE_EXPRESSION_DIMENSION_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>

#include "unit_representatives.h"

namespace Poincare::Internal {

struct MatrixDimension {
  uint8_t rows;
  uint8_t cols;
};

struct UnitDimension {
  Units::SIVector vector;
  // Only one representative is needed for now.
  const Units::Representative* representative;
};

struct Dimension {
  enum class DimensionType {
    Scalar,
    Matrix,
    Unit,
    Boolean,
    Point,
  };

  constexpr Dimension(DimensionType type = DimensionType::Scalar)
      : type(type), matrix({0, 0}) {
    assert(type == DimensionType::Scalar || type == DimensionType::Boolean ||
           type == DimensionType::Point);
  };
  Dimension(MatrixDimension iMatrix)
      : type(DimensionType::Matrix), matrix(iMatrix){};
  Dimension(UnitDimension iUnit) : type(DimensionType::Unit), unit(iUnit){};

  static Dimension Scalar() { return Dimension(DimensionType::Scalar); }
  static Dimension Boolean() { return Dimension(DimensionType::Boolean); }
  static Dimension Point() { return Dimension(DimensionType::Point); }
  static Dimension Matrix(uint8_t rows, uint8_t cols) {
    return Dimension({.rows = rows, .cols = cols});
  }
  static Dimension Unit(Units::SIVector vector,
                        const Units::Representative* representative) {
    assert(!vector.isEmpty());
    return Dimension({.vector = vector, .representative = representative});
  }
  static Dimension Unit(const Tree* unit);

  bool operator==(const Dimension& other) const;
  bool operator!=(const Dimension& other) const { return !(*this == other); };

  bool isSanitized() const {
    return !(isMatrix() && matrix.rows * matrix.cols == 0) &&
           !(isUnit() && unit.vector.isEmpty());
  }

  bool isScalar() const { return type == DimensionType::Scalar; }
  bool isMatrix() const { return type == DimensionType::Matrix; }
  bool isUnit() const { return type == DimensionType::Unit; }
  bool isBoolean() const { return type == DimensionType::Boolean; }
  bool isPoint() const { return type == DimensionType::Point; }
  bool isScalarOrUnit() const {
    return type == DimensionType::Scalar || type == DimensionType::Unit;
  }

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
  bool isSimpleRadianAngleUnit() const {
    return isSimpleAngleUnit() &&
           unit.representative == &Units::Angle::representatives.radian;
  }
  bool hasNonKelvinTemperatureUnit() const {
    return isUnit() && Units::Unit::IsNonKelvinTemperature(unit.representative);
  }

  constexpr static int k_nonListListLength = -1;
  // Return k_nonListListLength if tree isn't a list.
  static int ListLength(const Tree* e, Poincare::Context* ctx = nullptr);
  static bool IsList(const Tree* e, Poincare::Context* ctx = nullptr) {
    return ListLength(e, ctx) != k_nonListListLength;
  }
  static Dimension Get(const Tree* e, Poincare::Context* ctx = nullptr);
  static bool DeepCheck(const Tree* e, Poincare::Context* ctx = nullptr) {
    return DeepCheckDimensions(e, ctx) && DeepCheckListLength(e, ctx);
  }

  static void ReplaceTreeWithDimensionedType(Tree* e, Type type);

  DimensionType type;
  union {
    MatrixDimension matrix;
    UnitDimension unit;
  };

 private:
  static bool DeepCheckDimensions(const Tree* e,
                                  Poincare::Context* ctx = nullptr);
  static bool DeepCheckListLength(const Tree* e,
                                  Poincare::Context* ctx = nullptr);
};

}  // namespace Poincare::Internal

#endif
