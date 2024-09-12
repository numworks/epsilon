#include "dimension.h"

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>

#include "approximation.h"
#include "dependency.h"
#include "integer.h"
#include "matrix.h"
#include "parametric.h"
#include "physical_constant.h"
#include "symbol.h"
#include "units/representatives.h"
#include "variables.h"

namespace Poincare::Internal {

Dimension Dimension::Unit(const Tree* unit) {
  return Unit(Units::Unit::GetSIVector(unit),
              Units::Unit::GetRepresentative(unit));
}

bool IsIntegerExpression(const Tree* e) {
  /* TODO: when the dimension depends on an integer expression (that is not an
   * integer node) we need a nested Simplify to properly compute the value of
   * the inner expression before continuing. This will have consequences on the
   * API because we need to pass the projection context and to alter the tree to
   * avoid recomputing this sub-expression too often. */
  if (e->isInteger()) {
    return true;
  }
  switch (e->type()) {
    case Type::Parentheses:
    case Type::Opposite:
    case Type::Mult:
    case Type::Add: {
      for (const Tree* child : e->children()) {
        if (!IsIntegerExpression(child)) {
          return false;
        }
      }
      return true;
    }
    default:
      return false;
  }
}

bool Dimension::DeepCheckListLength(const Tree* e, Poincare::Context* ctx) {
  using Type = Internal::Type;
  // TODO complexity should be linear
  int childLength[e->numberOfChildren()];
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (!DeepCheckListLength(child, ctx)) {
      return false;
    }
    childLength[child.index] = ListLength(child, ctx);
  }
  switch (e->type()) {
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
      for (int i = 0; i < e->numberOfChildren(); i++) {
        if (childLength[i++] >= 0) {
          // List of lists are forbidden
          return false;
        }
      }
      return true;
    }
    case Type::UserSymbol: {
      // UserSymbols in context should always be well defined
#if ASSERTIONS
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      assert(!definition || DeepCheckListLength(definition, ctx));
#endif
      return true;
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        Tree* clone = e->cloneTree();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        bool result = DeepCheckListLength(clone, ctx);
        clone->removeTree();
        return result;
      }
      return true;
    }
    default: {
      assert(!e->isListToScalar());
      int thisLength = k_nonListListLength;
      for (int i = 0; i < e->numberOfChildren(); i++) {
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
        if (e->isListSequence() || e->isRandIntNoRep()) {
          return false;
        }
        Dimension dim = Get(e, ctx);
        if (dim.isMatrix() || dim.isUnit()) {
          return false;
        }
      }
    }
  }
  return true;
}

bool Dimension::isSimpleRadianAngleUnit() const {
  return isSimpleAngleUnit() &&
         unit.representative == &Units::Angle::representatives.radian;
}

bool Dimension::hasNonKelvinTemperatureUnit() const {
  return isUnit() && Units::Unit::IsNonKelvinTemperature(unit.representative);
}

int Dimension::ListLength(const Tree* e, Poincare::Context* ctx) {
  switch (e->type()) {
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
      return ListLength(e->child(0), ctx);
    case Type::List:
      return e->numberOfChildren();
    case Type::ListSequence:
      // TODO: Handle undef Approximation.
      return Approximation::To<float>(e->child(1));
    case Type::ListSlice: {
      assert(Integer::Is<uint8_t>(e->child(1)) &&
             Integer::Is<uint8_t>(e->child(2)));
      int listLength = ListLength(e->child(0), ctx);
      int start = Integer::Handler(e->child(1)).to<uint8_t>();
      start = std::max(start, 1);
      int end = Integer::Handler(e->child(2)).to<uint8_t>();
      end = std::min(end, listLength);
      // TODO: Handle undef Approximation.
      return std::max(end - start + 1, 0);
    }
    case Type::RandIntNoRep:
      assert(Integer::Is<uint8_t>(e->child(2)));
      return Integer::Handler(e->child(2)).to<uint8_t>();
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        return ListLength(definition, ctx);
      }
      return k_nonListListLength;
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        Tree* clone = e->cloneTree();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        int result = ListLength(clone, ctx);
        clone->removeTree();
        return result;
      }
      // Fallthrough so f({1,3,4}) returns 3. TODO : Maybe k_nonListListLength ?
      [[fallthrough]];
    }
    default: {
      // TODO sort lists first to optimize ListLength ?
      for (const Tree* child : e->children()) {
        int childListDim = ListLength(child, ctx);
        if (childListDim >= 0) {
          return childListDim;
        }
      }
      return k_nonListListLength;
    }
  }
}

bool Dimension::DeepCheckDimensions(const Tree* e, Poincare::Context* ctx) {
  Dimension childDim[e->numberOfChildren()];
  bool hasUnitChild = false;
  bool hasNonKelvinChild = false;
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (!DeepCheckDimensions(child, ctx)) {
      return false;
    }
    childDim[child.index] = Get(child, ctx);
    if (childDim[child.index].isUnit()) {
      // Cannot mix non-Kelvin temperature unit with any unit.
      // TODO: UnitConvert should be able to handle this.
      if (hasNonKelvinChild) {
        return false;
      }
      if (childDim[child.index].hasNonKelvinTemperatureUnit() &&
          !e->isUnitConversion()) {
        if (hasUnitChild) {
          return false;
        }
        hasNonKelvinChild = true;
      }
      hasUnitChild = true;
    }
    if (!e->isPiecewise() && !e->isParentheses() && !e->isDep() &&
        !e->isList() && !e->isListSort() &&
        childDim[child.index].isBoolean() != e->isLogicalOperatorOrBoolean()) {
      /* Only piecewises, parenthesis, dependencies, lists and boolean operators
       * can have boolean child. Boolean operators must have boolean child. */
      return false;
    }
    if (childDim[child.index].isPoint()) {
      // A few operations are allowed on points.
      switch (e->type()) {
        case Type::Piecewise:
          if (child.index % 2 == 1) {
            return false;
          }
          break;
        case Type::Diff:
        case Type::ListSequence:
          if (child.index != Parametric::FunctionIndex(e)) {
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
        case Type::Dep:
        case Type::Set:
        case Type::DepList:
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
  bool angleUnitsAllowed = e->isDirectTrigonometryFunction() ||
                           e->isDirectAdvancedTrigonometryFunction();
  switch (e->type()) {
    case Type::Add:
    case Type::Sub:
      for (int i = 1; i < e->numberOfChildren(); i++) {
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
      Units::SIVector unitVector = Units::SIVector::Empty();
      for (int i = 0; i < e->numberOfChildren(); i++) {
        bool secondDivisionChild = (i == 1 && e->isDiv());
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
      return IsIntegerExpression(e->child(1));
    }
    case Type::Sum:
    case Type::Product:
      return childDim[Parametric::k_variableIndex].isScalar() &&
             childDim[Parametric::k_lowerBoundIndex].isScalar() &&
             childDim[Parametric::k_upperBoundIndex].isScalar() &&
             (!e->isProduct() ||
              childDim[Parametric::k_integrandIndex].isScalar() ||
              childDim[Parametric::k_integrandIndex].isSquareMatrix());

    // Matrices
    case Type::Dim:
      return childDim[0].isMatrix() || IsList(e->child(0));
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
      return childDim[0].isScalar() && IsIntegerExpression(e->child(0));
    case Type::Norm:
      return childDim[0].isVector();
    case Type::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]);
    case Type::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
             (childDim[0].matrix.rows == 3 || childDim[0].matrix.cols == 3);
    case Type::Piecewise: {
      /* A piecewise can contain any type provided it is the same everywhere
       * Conditions are stored on odd indices and should be booleans */
      for (int i = 0; i < e->numberOfChildren(); i++) {
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
      /* Not using Dimension operator == because different representatives are
       * allowed. */
      assert(childDim[0].isUnit());
      return childDim[0].unit.vector == childDim[1].unit.vector ||
             (childDim[1].isSimpleAngleUnit() && childDim[0].isScalar());
    case Type::Dep:
      // Children can have a different dimension : [[x/x]] -> dep([[1]], {1/x})
      return true;
    case Type::Set:
    case Type::DepList:
    case Type::List:
      /* Lists can contain scalars, points or booleans but they must all be of
       * the same type. */
      for (int i = 0; i < e->numberOfChildren(); i++) {
        if (!(childDim[i].isScalar() || childDim[i].isPoint() ||
              childDim[i].isBoolean()) ||
            childDim[i] != childDim[0]) {
          return false;
        }
      }
      return true;
    case Type::ListElement:
      return Integer::Is<uint8_t>(e->child(1));
    case Type::ListSlice:
      return Integer::Is<uint8_t>(e->child(1)) &&
             Integer::Is<uint8_t>(e->child(2));
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        Tree* clone = e->cloneTree();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        bool result = DeepCheckDimensions(clone, ctx);
        clone->removeTree();
        return result;
      }
      [[fallthrough]];
    }
    case Type::Abs:
      // case Type::Sqrt:  TODO: Handle _m^(1/2)
      unitsAllowed = true;
      break;
    case Type::Matrix:
      break;
    case Type::ListSort:
    case Type::Parentheses:
      return true;
    case Type::RandIntNoRep:
      return Integer::Is<uint8_t>(e->child(2));
    case Type::UserSymbol: {
      // UserSymbols in context should always be well defined
#if ASSERTIONS
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      assert(!definition || DeepCheckDimensions(definition, ctx));
#endif
      return true;
    }
    case Type::NonNull:
    case Type::RealPos:
      // TODO: could be unit
      return childDim[0].isScalar();
    default:
      if (e->isLogicalOperatorOrBoolean()) {
        return true;
      }
      assert(e->isScalarOnly());
      break;
  }
  if (hasNonKelvinChild ||
      (hasUnitChild && !(unitsAllowed || angleUnitsAllowed))) {
    // Early escape. By default, non-Kelvin temperature unit are forbidden.
    return false;
  }
  // Check each child against the flags
  for (int i = 0; i < e->numberOfChildren(); i++) {
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

Dimension Dimension::Get(const Tree* e, Poincare::Context* ctx) {
  switch (e->type()) {
    case Type::Div:
    case Type::Mult: {
      uint8_t rows = 0;
      uint8_t cols = 0;
      const Units::Representative* representative = nullptr;
      Units::SIVector unitVector = Units::SIVector::Empty();
      bool secondDivisionChild = false;
      for (const Tree* child : e->children()) {
        Dimension dim = Get(child, ctx);
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
        secondDivisionChild = (e->isDiv());
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
      return Get(e->child(Parametric::FunctionIndex(e)), ctx);
    case Type::Dep:
      return Get(Dependency::Main(e), ctx);
    case Type::PowMatrix:
    case Type::PowReal:
    case Type::Pow: {
      Dimension dim = Get(e->child(0), ctx);
      if (dim.isUnit()) {
        float index = Approximation::To<float>(e->child(1));
        if (index < INT8_MAX && index > INT8_MIN &&
            std::round(index) == index) {
          // TODO: Handle/forbid index > int8_t
          assert(!std::isnan(index) &&
                 std::fabs(index) < static_cast<float>(INT8_MAX));
          Units::SIVector unitVector = Units::SIVector::Empty();
          unitVector.addAllCoefficients(dim.unit.vector,
                                        static_cast<int8_t>(index));

          dim = unitVector.isEmpty()
                    ? Scalar()
                    : Unit(unitVector, dim.unit.representative);
        }
        // Handle the unit as a scalar if the index is not an integer
        // TODO_PCJ: Either forbid or ensure this is handled correctly
      }
      return dim;
    }
    case Type::Abs:
    case Type::Opposite:
    case Type::Sqrt:
    case Type::Add:
    case Type::Sub:
    case Type::Cross:
    case Type::Inverse:
    case Type::Ref:
    case Type::Rref:
    case Type::Piecewise:
    case Type::Parentheses:
    case Type::ListElement:
    case Type::ListSort:
      return Get(e->child(0), ctx);
    case Type::Matrix:
      return Matrix(Matrix::NumberOfRows(e), Matrix::NumberOfColumns(e));
    case Type::Dim:
      return Get(e->child(0), ctx).isMatrix() ? Matrix(1, 2) : Scalar();
    case Type::Transpose: {
      Dimension dim = Get(e->child(0), ctx);
      return Matrix(dim.matrix.cols, dim.matrix.rows);
    }
    case Type::Identity: {
      int n = Approximation::To<float>(e->child(0));
      return Matrix(n, n);
    }
    case Type::UnitConversion:
      /* Use first child because it's representative is needed in
       * Unit::ProjectToBestUnits in case of non kelvin units. */
      return Get(e->child(0), ctx);
    case Type::Unit:
      return Dimension::Unit(e);
    case Type::PhysicalConstant:
      return Dimension::Unit(PhysicalConstant::GetProperties(e).m_dimension,
                             nullptr);
    case Type::Point:
      return Point();
    case Type::Set:
    case Type::DepList:
    case Type::ListSlice:
    case Type::List:
      return ListLength(e, ctx) > 0 ? Get(e->child(0), ctx) : Scalar();
    case Type::UserSymbol: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        return Get(definition, ctx);
      }
      return Scalar();
    }
    case Type::UserFunction: {
      const Tree* definition = ctx ? ctx->treeForSymbolIdentifier(e) : nullptr;
      if (definition) {
        Tree* clone = e->cloneTree();
        // Replace function's symbol with definition
        Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
        Dimension result = Get(clone, ctx);
        clone->removeTree();
        return result;
      }
      // TODO: Maybe scalar ?
      return Get(e->child(0), ctx);
    }
    default:
      if (e->isLogicalOperatorOrBoolean() || e->isComparison()) {
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
