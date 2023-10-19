#include "dimension.h"

#include "approximation.h"
#include "matrix.h"
#include "parametric.h"
#include "symbol.h"

namespace PoincareJ {

Dimension Dimension::Unit(const Tree* unit) {
  return Unit(Units::DimensionVector::FromBaseUnits(unit),
              Units::Unit::GetRepresentative(unit));
}

bool Dimension::DeepCheckDimensions(const Tree* t) {
  Dimension childDim[t->numberOfChildren()];
  bool hasUnitChild = false;
  bool hasNonKelvinChild = false;
  for (int i = 0; const Tree* child : t->children()) {
    if (!DeepCheckDimensions(child)) {
      return false;
    }
    childDim[i] = GetDimension(child);
    if (childDim[i].isUnit()) {
      // Cannot mix non-Kelvin temperature unit with any unit.
      // TODO: UnitConvert should be able to handle this.
      if (hasNonKelvinChild) {
        return false;
      }
      if (childDim[i].hasNonKelvinTemperatureUnit()) {
        if (hasUnitChild) {
          return false;
        }
        hasNonKelvinChild = true;
      }
      hasUnitChild = true;
    }
    assert(childDim[i].isSanitized());
    i++;
  }
  bool unitsAllowed = false;
  bool angleUnitsAllowed = false;
  switch (t->type()) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      assert(t->numberOfChildren() > 0);
      for (int i = 1; i < t->numberOfChildren(); i++) {
        if (childDim[0] != childDim[i]) {
          return false;
        }
      }
      return true;
    case BlockType::Division:
    case BlockType::Multiplication: {
      /* TODO: Forbid Complex * units. Units are already forbidden in complex
       * builtins. */
      assert(t->numberOfChildren() > 0);
      uint8_t cols = 0;
      Units::DimensionVector unitVector = Units::DimensionVector::Empty();
      for (int i = 0; i < t->numberOfChildren(); i++) {
        bool secondDivisionChild = (i == 1 && t->type() == BlockType::Division);
        Dimension next = childDim[i];
        if (next.isMatrix()) {
          // Matrix size must match. Forbid Matrices on denominator
          if ((cols && cols != next.matrix.rows) || secondDivisionChild) {
            return false;
          }
          cols = next.matrix.cols;
        } else if (next.isUnit()) {
          if (hasNonKelvinChild && secondDivisionChild) {
            // Cannot divide by non-Kelvin temperature unit
            assert(next.hasNonKelvinTemperatureUnit());
            return false;
          }
          unitVector.addAllCoefficients(next.unit.vector,
                                        secondDivisionChild ? -1 : 1);
        }
      }
      // Forbid units * matrices
      return unitVector.isEmpty() || cols == 0;
    }
    case BlockType::Power:
    case BlockType::PowerReal:
    case BlockType::PowerMatrix: {
      if (!childDim[1].isScalar()) {
        return false;
      }
      if (childDim[0].isMatrix()) {
        return childDim[0].isSquareMatrix();
      }
      if (!childDim[0].isUnit()) {
        return true;
      }
      if (hasNonKelvinChild) {
        assert(childDim[0].hasNonKelvinTemperatureUnit());
        // Powers of non-Kelvin temperature unit are forbidden
        return false;
      }
      const Tree* index = t->child(1);
      // TODO: Handle operations such as _m^(1+1) or _m^(-1*n) or _m^(1/2)
      return index->type().isRational() ||
             index->type() == BlockType::Decimal ||
             (index->type() == BlockType::Multiplication &&
              index->numberOfChildren() == 2 &&
              index->child(0)->type() == BlockType::MinusOne &&
              index->child(1)->type().isRational());
    }
    case BlockType::Sum:
    case BlockType::Product:
      return childDim[Parametric::k_variableIndex].isScalar() &&
             childDim[Parametric::k_lowerBoundIndex].isScalar() &&
             childDim[Parametric::k_upperBoundIndex].isScalar() &&
             (t->type() != BlockType::Product ||
              childDim[Parametric::k_integrandIndex].isScalar() ||
              childDim[Parametric::k_integrandIndex].isSquareMatrix());
    case BlockType::Dim:
    case BlockType::Ref:
    case BlockType::Rref:
    case BlockType::Transpose:
      return childDim[0].isMatrix();
    case BlockType::Det:
    case BlockType::Trace:
    case BlockType::Inverse:
      return childDim[0].isSquareMatrix();
    case BlockType::Identity:
      // TODO check for unknowns and display error message if not integral
      return childDim[0].isScalar() && t->child(0)->type().isInteger();
    case BlockType::Norm:
      return childDim[0].isVector();
    case BlockType::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]);
    case BlockType::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
             (childDim[0].matrix.rows == 3 || childDim[0].matrix.cols == 3);
    case BlockType::Abs:
    // case BlockType::SquareRoot: TODO: Handle _m^(1/2)
    case BlockType::UserFunction:
      unitsAllowed = true;
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
    case BlockType::Trig:
      angleUnitsAllowed = true;
    default:
      assert(t->type().isScalarOnly());
    case BlockType::Matrix:
      if (hasNonKelvinChild ||
          (hasUnitChild && !(unitsAllowed || angleUnitsAllowed))) {
        // Early escape. By default, non-Kelvin temperature unit are forbidden.
        return false;
      }
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (childDim[i].isScalar() ||
            (childDim[i].isUnit() &&
             (unitsAllowed ||
              (angleUnitsAllowed && childDim[i].isSimpleAngleUnit())))) {
          continue;
        }
        return false;
      }
      return true;
  }
}

Dimension Dimension::GetDimension(const Tree* t) {
  switch (t->type()) {
    case BlockType::Division:
    case BlockType::Multiplication: {
      uint8_t rows = 0;
      uint8_t cols = 0;
      const Units::UnitRepresentative* representative = nullptr;
      Units::DimensionVector unitVector = Units::DimensionVector::Empty();
      bool secondDivisionChild = false;
      for (const Tree* child : t->children()) {
        Dimension dim = GetDimension(child);
        if (dim.isMatrix()) {
          if (rows == 0) {
            rows = dim.matrix.rows;
          }
          cols = dim.matrix.cols;
        } else if (dim.isUnit()) {
          unitVector.addAllCoefficients(dim.unit.vector,
                                        secondDivisionChild ? -1 : 1);
          representative = dim.unit.representative;
        }
        secondDivisionChild = (t->type() == BlockType::Division);
      }
      // If other than a non-kelvin temperature, representative doesn't matter.
      return rows > 0
                 ? Matrix(rows, cols)
                 : (unitVector.isEmpty() ? Scalar()
                                         : Unit(unitVector, representative));
    }
    case BlockType::Sum:
    case BlockType::Product:
      return GetDimension(t->child(Parametric::k_integrandIndex));
    case BlockType::PowerMatrix:
    case BlockType::PowerReal:
    case BlockType::Power: {
      Dimension dim = GetDimension(t->nextNode());
      if (dim.isUnit()) {
        float index = Approximation::To<float>(t->child(1));
        // TODO: Handle/forbid index > int8_t
        assert(!std::isnan(index) &&
               std::fabs(index) < static_cast<float>(INT8_MAX));
        Units::DimensionVector unitVector = Units::DimensionVector::Empty();
        unitVector.addAllCoefficients(dim.unit.vector,
                                      static_cast<int8_t>(index));
        return Unit(unitVector, dim.unit.representative);
      }
    }
    case BlockType::Abs:
    case BlockType::SquareRoot:
    case BlockType::UserFunction:
    case BlockType::Addition:
    case BlockType::Subtraction:
    case BlockType::Cross:
    case BlockType::Inverse:
    case BlockType::Ref:
    case BlockType::Rref:
      return GetDimension(t->nextNode());
    case BlockType::Matrix:
      return Matrix(Matrix::NumberOfRows(t), Matrix::NumberOfColumns(t));
    case BlockType::Dim:
      return Matrix(1, 2);
    case BlockType::Transpose: {
      Dimension dim = GetDimension(t->nextNode());
      return Matrix(dim.matrix.cols, dim.matrix.rows);
    }
    case BlockType::Identity: {
      int n = Approximation::To<float>(t->child(0));
      return Matrix(n, n);
    }
    case BlockType::Unit:
      return Dimension::Unit(t);
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      // Note: Angle units could be returned here.
    default:
      return Scalar();
  }
}

bool Dimension::operator==(const Dimension& other) const {
  if (type != other.type) {
    return false;
  }
  if (type == Type::Matrix) {
    return matrix.rows == other.matrix.rows && matrix.cols == other.matrix.cols;
  }
  if (type == Type::Unit) {
    return unit.vector == other.unit.vector &&
           unit.representative == other.unit.representative;
  }
  return true;
}

}  // namespace PoincareJ
