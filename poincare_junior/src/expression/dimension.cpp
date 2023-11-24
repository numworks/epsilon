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

bool Dimension::DeepCheckListLength(const Tree* t) {
  // TODO complexity should be linear
  int childLength[t->numberOfChildren()];
  for (int i = 0; const Tree* child : t->children()) {
    if (!DeepCheckListLength(child)) {
      return false;
    }
    childLength[i] = GetListLength(child);
    i++;
  }
  if (t->isListToScalar() && childLength[0] == 0) {
    // ListToScalar operators expect a list as first argument
    return false;
  }
  if (t->isSampleStdDev() && childLength[0] < 2) {
    // SampleStdDev needs a list of length >= 2
    return false;
  }
  if (t->isList()) {
    for (int i = 0; i < t->numberOfChildren(); i++) {
      if (childLength[i++] > 0) {
        // List of lists are forbidden
        return false;
      }
    }
    return true;
  }
  int thisLength = 0;
  for (int i = 0; i < t->numberOfChildren(); i++) {
    if (childLength[i] == 0) {
      continue;
    }
    if (thisLength > 0 && childLength[i] != thisLength) {
      // Children lists should have the same dimension
      return false;
    }
    thisLength = childLength[i];
  }
  if (thisLength > 0 && GetDimension(t).isMatrix()) {
    // List of matrices are forbidden
    return false;
  }
  return true;
}

int Dimension::GetListLength(const Tree* t) {
  switch (t->type()) {
    case BlockType::Mean:
    case BlockType::StdDev:
    case BlockType::Median:
    case BlockType::Variance:
    case BlockType::SampleStdDev:
    case BlockType::Minimum:
    case BlockType::Maximum:
    case BlockType::ListSum:
    case BlockType::ListProduct:
      assert(GetListLength(t->child(0)));
      return 0;
    case BlockType::ListAccess:
      assert(GetListLength(t->child(0)) && GetListLength(t->child(1)) == 1);
      return 0;
    case BlockType::ListSort:
      assert(GetListLength(t->child(0)));
      return GetListLength(t->child(0));
    case BlockType::List:
      // all children should be scalars
      return t->numberOfChildren();
    case BlockType::ListSequence:
      return Approximation::To<float>(t->child(1));
    case BlockType::RandIntNoRep:
      return Approximation::To<float>(t->child(2));
    default: {
      // TODO sort lists first to optimize GetListLength ?
      for (const Tree* child : t->children()) {
        int childListDim = GetListLength(child);
        if (childListDim) {
          return childListDim;
        }
      }
      return 0;
    }
  }
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
        bool secondDivisionChild = (i == 1 && t->isDivision());
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
      return index->isRational() || index->isDecimal() ||
             (index->isMultiplication() && index->numberOfChildren() == 2 &&
              index->child(0)->isMinusOne() && index->child(1)->isRational());
    }
    case BlockType::Sum:
    case BlockType::Product:
      return childDim[Parametric::k_variableIndex].isScalar() &&
             childDim[Parametric::k_lowerBoundIndex].isScalar() &&
             childDim[Parametric::k_upperBoundIndex].isScalar() &&
             (!t->isProduct() ||
              childDim[Parametric::k_integrandIndex].isScalar() ||
              childDim[Parametric::k_integrandIndex].isSquareMatrix());

    // Matrices
    case BlockType::Dim:
      return childDim[0].isMatrix() ||
             (childDim[0].isScalar() && GetListLength(t->child(0)) > 0);
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
      return childDim[0].isScalar() && t->child(0)->isInteger();
    case BlockType::Norm:
      return childDim[0].isVector();
    case BlockType::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]);
    case BlockType::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
             (childDim[0].matrix.rows == 3 || childDim[0].matrix.cols == 3);

    case BlockType::Round:
      return (childDim[0].isScalar() || childDim[0].isUnit()) &&
             childDim[1].isScalar();
    case BlockType::Abs:
    case BlockType::Floor:
    case BlockType::Ceiling:
    case BlockType::Sign:
    // case BlockType::SquareRoot: TODO: Handle _m^(1/2)
    case BlockType::UserFunction:
      unitsAllowed = true;
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
    case BlockType::Trig:
      angleUnitsAllowed = true;
    default:
      assert(t->isScalarOnly());
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
      const Units::Representative* representative = nullptr;
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
        secondDivisionChild = (t->isDivision());
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
    case BlockType::Floor:
    case BlockType::Ceiling:
    case BlockType::Round:
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
      return GetDimension(t->nextNode()).isMatrix() ? Matrix(1, 2) : Scalar();
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
