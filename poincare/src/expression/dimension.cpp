#include "dimension.h"

#include <algorithm>

#include "approximation.h"
#include "dependency.h"
#include "integer.h"
#include "matrix.h"
#include "parametric.h"
#include "physical_constant.h"
#include "symbol.h"
#include "variables.h"

namespace Poincare::Internal {

// TODO_PCJ: Use context with userNamed trees

Dimension Dimension::Unit(const Tree* unit) {
  return Unit(Units::DimensionVector::FromBaseUnits(unit),
              Units::Unit::GetRepresentative(unit));
}

bool Dimension::DeepCheckListLength(const Tree* t, Poincare::Context* ctx) {
  using Type = Internal::Type;
  // TODO complexity should be linear
  int childLength[t->numberOfChildren()];
  for (IndexedChild<const Tree*> child : t->indexedChildren()) {
    if (!DeepCheckListLength(child, ctx)) {
      return false;
    }
    childLength[child.index] = GetListLength(child, ctx);
  }
  switch (t->type()) {
    case Type::SampleStdDev:
      // SampleStdDev needs a list of length >= 2
      return childLength[0] >= 2 && (childLength[1] == k_nonListListLength ||
                                     childLength[0] == childLength[1]);
    case Type::Mean:
    case Type::StdDev:
    case Type::Median:
    case Type::Variance:
      // At least 1 child is needed.
      return childLength[0] >= 1 && (childLength[1] == k_nonListListLength ||
                                     childLength[0] == childLength[1]);
    case Type::Min:
    case Type::Max:
      // At least 1 child is needed.
      return childLength[0] >= 1;
    case Type::ListSum:
    case Type::ListProduct:
    case Type::ListSort:
      return childLength[0] >= 0;
    case Type::ListElement:
      return childLength[0] >= 0 && childLength[1] == k_nonListListLength;
    case Type::ListSlice:
      return childLength[0] >= 0 && childLength[1] == k_nonListListLength &&
             childLength[2] == k_nonListListLength;
    case Type::Set:
    case Type::List: {
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (childLength[i++] >= 0) {
          // List of lists are forbidden
          return false;
        }
      }
      return true;
    }
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        return DeepCheckListLength(definition, ctx);
      }
      return true;
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        Tree* clone = t->clone();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        bool result = DeepCheckListLength(clone, ctx);
        clone->removeTree();
        return result;
      }
      return true;
    }
    default: {
      assert(!t->isListToScalar());
      int thisLength = k_unknownListLength;
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (childLength[i] == k_nonListListLength) {
          continue;
        }
        if (thisLength >= 0 && childLength[i] != thisLength) {
          // Children lists should have the same length
          return false;
        }
        thisLength = childLength[i];
      }
      if (thisLength >= 0) {
        // Lists are forbidden
        if (t->isListSequence() || t->isRandIntNoRep()) {
          return false;
        }
        Dimension dim = GetDimension(t, ctx);
        if (dim.isMatrix() || dim.isUnit()) {
          return false;
        }
      }
    }
  }
  return true;
}

int Dimension::GetListLength(const Tree* t, Poincare::Context* ctx) {
  switch (t->type()) {
    case Type::Mean:
    case Type::StdDev:
    case Type::Median:
    case Type::Variance:
    case Type::SampleStdDev:
    case Type::Min:
    case Type::Max:
    case Type::ListSum:
    case Type::ListProduct:
    case Type::Dim:
    case Type::ListElement:
      return k_nonListListLength;
    case Type::ListSort:
      return GetListLength(t->child(0), ctx);
    case Type::List:
      return t->numberOfChildren();
    case Type::ListSequence:
      // TODO: Handle undef Approximation.
      return Approximation::To<float>(t->child(1));
    case Type::ListSlice: {
      assert(Integer::Is<uint8_t>(t->child(1)) &&
             Integer::Is<uint8_t>(t->child(2)));
      int listLength = GetListLength(t->child(0), ctx);
      int start = Integer::Handler(t->child(1)).to<uint8_t>();
      start = std::max(start, 1);
      int end = Integer::Handler(t->child(2)).to<uint8_t>();
      end = std::min(end, listLength);
      // TODO: Handle undef Approximation.
      return std::max(end - start + 1, 0);
    }
    case Type::RandIntNoRep:
      assert(Integer::Is<uint8_t>(t->child(2)));
      return Integer::Handler(t->child(2)).to<uint8_t>();
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        return GetListLength(definition, ctx);
      }
      return k_nonListListLength;
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        Tree* clone = t->clone();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        int result = GetListLength(clone, ctx);
        clone->removeTree();
        return result;
      }
      // Fallthrough so f({1,3,4}) returns 3. TODO : Maybe k_nonListListLength ?
    }
    default: {
      // TODO sort lists first to optimize GetListLength ?
      for (const Tree* child : t->children()) {
        int childListDim = GetListLength(child, ctx);
        if (childListDim >= 0) {
          return childListDim;
        }
      }
      return k_nonListListLength;
    }
  }
}

bool Dimension::DeepCheckDimensions(const Tree* t, Poincare::Context* ctx) {
  Dimension childDim[t->numberOfChildren()];
  bool hasUnitChild = false;
  bool hasNonKelvinChild = false;
  for (IndexedChild<const Tree*> child : t->indexedChildren()) {
    if (!DeepCheckDimensions(child, ctx)) {
      return false;
    }
    childDim[child.index] = GetDimension(child, ctx);
    if (childDim[child.index].isUnit()) {
      // Cannot mix non-Kelvin temperature unit with any unit.
      // TODO: UnitConvert should be able to handle this.
      if (hasNonKelvinChild) {
        return false;
      }
      if (childDim[child.index].hasNonKelvinTemperatureUnit()) {
        if (hasUnitChild) {
          return false;
        }
        hasNonKelvinChild = true;
      }
      hasUnitChild = true;
    }
    if (!t->isPiecewise() && !t->isParenthesis() && !t->isDependency() &&
        !t->isList() &&
        childDim[child.index].isBoolean() != t->isLogicalOperatorOrBoolean()) {
      /* Only piecewises, parenthesis, dependencies, lists and boolean operators
       * can have boolean child. Boolean operators must have boolean child. */
      return false;
    }
    if (childDim[child.index].isPoint()) {
      // A few operations are allowed on points.
      switch (t->type()) {
        case Type::Piecewise:
          if (child.index % 2 == 1) {
            return false;
          }
          break;
        case Type::Diff:
        case Type::NthDiff:
        case Type::ListSequence:
          if (child.index != Parametric::FunctionIndex(t)) {
            return false;
          }
          break;
        case Type::ListElement:
        case Type::ListSlice:
          if (child.index > 0) {
            return false;
          }
          break;
        case Type::Dim:
        case Type::Dependency:
        case Type::Set:
        case Type::List:
        case Type::ListSort:
          break;
        default:
          return false;
      }
    }
    assert(childDim[child.index].isSanitized());
  }
  bool unitsAllowed = false;
  bool angleUnitsAllowed = false;
  switch (t->type()) {
    case Type::Add:
    case Type::Sub:
      for (int i = 1; i < t->numberOfChildren(); i++) {
        if (childDim[0] != childDim[i]) {
          return false;
        }
      }
      return true;
    case Type::Opposite:
    case Type::Div:
    case Type::Mult: {
      /* TODO: Forbid Complex * units. Units are already forbidden in complex
       * builtins. */
      uint8_t cols = 0;
      Units::DimensionVector unitVector = Units::DimensionVector::Empty();
      for (int i = 0; i < t->numberOfChildren(); i++) {
        bool secondDivisionChild = (i == 1 && t->isDiv());
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
    case Type::Pow:
    case Type::PowReal:
    case Type::PowMatrix: {
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
      // TODO: Handle operations such as m^(1+1) or m^(-1*n) or m^(1/2) or m^0.5
      return index->isInteger() ||
             (index->isOpposite() && index->child(0)->isInteger()) ||
             (index->isMult() && index->numberOfChildren() == 2 &&
              index->child(0)->isMinusOne() && index->child(1)->isInteger());
    }
    case Type::Sum:
    case Type::Product:
      return childDim[Parametric::k_variableIndex].isScalar() &&
             childDim[Parametric::k_lowerBoundIndex].isScalar() &&
             childDim[Parametric::k_upperBoundIndex].isScalar() &&
             (!t->isProduct() ||
              childDim[Parametric::k_integrandIndex].isScalar() ||
              childDim[Parametric::k_integrandIndex].isSquareMatrix());

    // Matrices
    case Type::Dim:
      return childDim[0].isMatrix() || IsList(t->child(0));
    case Type::Ref:
    case Type::Rref:
    case Type::Transpose:
      return childDim[0].isMatrix();
    case Type::Det:
    case Type::Trace:
    case Type::Inverse:
      return childDim[0].isSquareMatrix();
    case Type::Identity:
      // TODO check for unknowns and display error message if not integral
      return childDim[0].isScalar() && t->child(0)->isInteger();
    case Type::Norm:
      return childDim[0].isVector();
    case Type::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]);
    case Type::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
             (childDim[0].matrix.rows == 3 || childDim[0].matrix.cols == 3);

    case Type::Round:
      return (childDim[0].isScalar() || childDim[0].isUnit()) &&
             childDim[1].isScalar();
    case Type::Piecewise: {
      /* A piecewise can contain any type provided it is the same everywhere
       * Conditions are stored on odd indices and should be booleans */
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (i % 2 == 1) {
          if (!childDim[i].isBoolean()) {
            return false;
          }
          continue;
        }
        if (childDim[i] != childDim[0]) {
          return false;
        }
      }
      return true;
    }
    case Type::UnitConversion:
      assert(childDim[1].isUnit());
      return childDim[0] == childDim[1] ||
             (childDim[1].isAngleUnit() && childDim[0].isScalar());
    case Type::Dependency:
      // Children can have a different dimension : [[x/x]] -> dep([[1]], {1/x})
      return true;
    case Type::Set:
    case Type::List:
      /* Lists can contain scalars, points or booleans but they must all be of
       * the same type. */
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (!(childDim[i].isScalar() || childDim[i].isPoint() ||
              childDim[i].isBoolean()) ||
            childDim[i] != childDim[0]) {
          return false;
        }
      }
      return true;
    case Type::ListElement:
      return Integer::Is<uint8_t>(t->child(1));
    case Type::ListSlice:
      return Integer::Is<uint8_t>(t->child(1)) &&
             Integer::Is<uint8_t>(t->child(2));
    case Type::Abs:
    case Type::Floor:
    case Type::Ceil:
    case Type::Sign:
    // case Type::Sqrt: TODO: Handle _m^(1/2)
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        Tree* clone = t->clone();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        bool result = DeepCheckDimensions(clone, ctx);
        clone->removeTree();
        return result;
      }
      unitsAllowed = true;
      break;
    }
    case Type::Cos:
    case Type::Sin:
    case Type::Tan:
    case Type::Trig:
      angleUnitsAllowed = true;
      break;
    case Type::Matrix:
      break;
    case Type::Parenthesis:
      return true;
    case Type::RandIntNoRep:
      return Integer::Is<uint8_t>(t->child(2));
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        return DeepCheckDimensions(definition, ctx);
      }
      break;
    }
    default:
      if (t->isLogicalOperatorOrBoolean()) {
        return true;
      }
      assert(t->isScalarOnly());
      break;
  }
  if (hasNonKelvinChild ||
      (hasUnitChild && !(unitsAllowed || angleUnitsAllowed))) {
    // Early escape. By default, non-Kelvin temperature unit are forbidden.
    return false;
  }
  // Check each child against the flags
  for (int i = 0; i < t->numberOfChildren(); i++) {
    if (childDim[i].isScalar() || childDim[i].isPoint() ||
        (childDim[i].isUnit() &&
         (unitsAllowed ||
          (angleUnitsAllowed && childDim[i].isSimpleAngleUnit())))) {
      continue;
    }
    return false;
  }
  return true;
}

Dimension Dimension::GetDimension(const Tree* t, Poincare::Context* ctx) {
  switch (t->type()) {
    case Type::Div:
    case Type::Mult: {
      uint8_t rows = 0;
      uint8_t cols = 0;
      const Units::Representative* representative = nullptr;
      Units::DimensionVector unitVector = Units::DimensionVector::Empty();
      bool secondDivisionChild = false;
      for (const Tree* child : t->children()) {
        Dimension dim = GetDimension(child, ctx);
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
        secondDivisionChild = (t->isDiv());
      }
      // Only unique and celsius, fahrenheit or radians representatives matter.
      return rows > 0
                 ? Matrix(rows, cols)
                 : (unitVector.isEmpty() ? Scalar()
                                         : Unit(unitVector, representative));
    }
    case Type::Sum:
    case Type::Product:
    case Type::ListSequence:
    case Type::Diff:
    case Type::NthDiff:
      return GetDimension(t->child(Parametric::FunctionIndex(t)), ctx);
    case Type::Dependency:
      return GetDimension(Dependency::Main(t), ctx);
    case Type::PowMatrix:
    case Type::PowReal:
    case Type::Pow: {
      Dimension dim = GetDimension(t->child(0), ctx);
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
    case Type::Abs:
    case Type::Opposite:
    case Type::Sqrt:
    case Type::Floor:
    case Type::Ceil:
    case Type::Round:
    case Type::Add:
    case Type::Sub:
    case Type::Cross:
    case Type::Inverse:
    case Type::Ref:
    case Type::Rref:
    case Type::Piecewise:
    case Type::Parenthesis:
    case Type::ListElement:
    case Type::ListSort:
      return GetDimension(t->child(0), ctx);
    case Type::Matrix:
      return Matrix(Matrix::NumberOfRows(t), Matrix::NumberOfColumns(t));
    case Type::Dim:
      return GetDimension(t->child(0), ctx).isMatrix() ? Matrix(1, 2)
                                                       : Scalar();
    case Type::Transpose: {
      Dimension dim = GetDimension(t->child(0), ctx);
      return Matrix(dim.matrix.cols, dim.matrix.rows);
    }
    case Type::Identity: {
      int n = Approximation::To<float>(t->child(0));
      return Matrix(n, n);
    }
    case Type::UnitConversion:
      /* Use first child because it's representative is needed in
       * Unit::ProjectToBestUnits in case of non kelvin units. */
      return GetDimension(t->child(0), ctx);
    case Type::Unit:
      return Dimension::Unit(t);
    case Type::PhysicalConstant:
      return Dimension::Unit(PhysicalConstant::GetProperties(t).m_dimension,
                             nullptr);
    case Type::Point:
      return Point();
    case Type::Set:
    case Type::ListSlice:
    case Type::List:
      return GetListLength(t, ctx) > 0 ? GetDimension(t->child(0), ctx)
                                       : Scalar();
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        return GetDimension(definition, ctx);
      }
      return Scalar();
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(t) : nullptr;
      if (definition) {
        Tree* clone = t->clone();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        Dimension result = GetDimension(clone, ctx);
        clone->removeTree();
        return result;
      }
      // TODO: Maybe scalar ?
      return GetDimension(t->child(0), ctx);
    }
    case Type::ACos:
    case Type::ASin:
    case Type::ATan:
      // Note: Angle units could be returned here.
    default:
      if (t->isLogicalOperatorOrBoolean() || t->isComparison()) {
        return Boolean();
      }
      return Scalar();
  }
}

bool Dimension::operator==(const Dimension& other) const {
  if (type != other.type) {
    return false;
  }
  if (type == DimensionType::Matrix) {
    return matrix.rows == other.matrix.rows && matrix.cols == other.matrix.cols;
  }
  if (type == DimensionType::Unit) {
    return unit.vector == other.unit.vector &&
           (unit.vector != Units::Temperature::Dimension ||
            unit.representative == other.unit.representative);
  }
  return true;
}

}  // namespace Poincare::Internal
