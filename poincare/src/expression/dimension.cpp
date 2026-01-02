#include "dimension.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/symbol_context.h>
#include <stdint.h>

#include <algorithm>

#include "approximation.h"
#include "dependency.h"
#include "integer.h"
#include "list.h"
#include "matrix.h"
#include "number.h"
#include "parametric.h"
#include "physical_constant.h"
#include "rational.h"
#include "symbol.h"
#include "units/representatives.h"
#include "variables.h"

namespace Poincare::Internal {

template <typename T>
class LazyArray {
 public:
  LazyArray(const Tree* root, const Poincare::SymbolContext& symbolContext,
            T (*getter)(const Tree* e,
                        const Poincare::SymbolContext& symbolContext))
      : m_root(root),
        m_ctx(symbolContext),
        m_getter(getter),
        m_lastAccessedIndex(0) {
    if (m_root->numberOfChildren() > 0) {
      m_element0 = m_getter(m_root->child(0), m_ctx);
    }
  }
  T operator[](size_t index) {
    if (index == 0) {
      return m_element0;
    }
    if (index == m_lastAccessedIndex) {
      return m_lastAccessedElement;
    }
    m_lastAccessedElement = m_getter(m_root->child(index), m_ctx);
    m_lastAccessedIndex = index;
    return m_lastAccessedElement;
  }

 private:
  T m_element0;
  T m_lastAccessedElement;
  const Tree* m_root;
  const Poincare::SymbolContext& m_ctx;
  T (*m_getter)(const Tree* e, const Poincare::SymbolContext& symbolContext);
  size_t m_lastAccessedIndex;
};

Dimension Dimension::Unit(const Tree* unit) {
  return Unit(Units::Unit::GetSIVector(unit),
              Units::Unit::GetRepresentative(unit));
}

bool IsIntegerExpression(const Tree* e, bool excludeFunctionCalls = false) {
  /* TODO: when the dimension depends on an integer expression (that is not an
   * integer node) we need a nested Simplify to properly compute the value of
   * the inner expression before continuing. This will have consequences on the
   * API because we need to pass the projection context and to alter the tree to
   * avoid recomputing this sub-expression too often. */
  if (e->isInteger()) {
    return true;
  }
  switch (e->type()) {
    case Type::AngleUnitContext:
    case Type::Parentheses:
    case Type::Opposite:
    case Type::Mult:
    case Type::Add:
    case Type::Sub: {
      for (const Tree* child : e->children()) {
        if (!IsIntegerExpression(child, excludeFunctionCalls)) {
          return false;
        }
      }
      return true;
    }
    case Type::Pow:
      return IsIntegerExpression(e->child(0), excludeFunctionCalls) &&
             Integer::Is<uint8_t>(e->child(1));
    case Type::Ceil:
    case Type::Floor:
      return !excludeFunctionCalls;
    case Type::Round:
      return !excludeFunctionCalls && e->child(1)->isZero();
    default:
      return false;
  }
}

Tree* CloneAndReplaceFunctionDefinition(const Tree* e, const Tree* definition) {
  Tree* clone = e->cloneTree();
  assert(e->isUserFunction());
  Variables::ReplaceUserFunctionOrSequenceWithTree(clone, definition);
  return clone;
}

/* Returns nullptr if the variable is not defined in the symbolContext, returns
 * undef if the definition exists but is circular, and returns the variable
 * definition otherwise. */
const Tree* DefinitionWithCircularityCheck(const Tree* e,
                                           const SymbolContext& symbolContext) {
  const Tree* definition = symbolContext.expressionForUserNamed(e);
  if (definition && Symbol::InvolvesCircularity(definition, symbolContext)) {
    return KUndef;
  }
  return definition;
}

bool Dimension::DeepCheckListLength(
    const Tree* e, const Poincare::SymbolContext& symbolContext) {
  // TODO complexity should be linear
  LazyArray<int> childLength(e, symbolContext, ListLength);
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (!DeepCheckListLength(child, symbolContext)) {
      return false;
    }
  }
  switch (e->type()) {
    case Type::SampleStdDev:
    case Type::Mean:
    case Type::StdDev:
    case Type::Median:
    case Type::Variance: {
      int minListLen = e->type() == Type::SampleStdDev ? 2 : 1;
      // At least minListLen child is needed.
      return childLength[0] >= minListLen &&
             ((childLength[1] == k_nonListListLength &&
               Number::IsOne(e->child(1))) ||
              childLength[0] == childLength[1]);
    }
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
    case Type::List: {
      for (int i = 0; i < e->numberOfChildren(); i++) {
        if (childLength[i] >= 0) {
          // List of lists are forbidden
          return false;
        }
      }
      return true;
    }
    case Type::UserSymbol: {
      // UserSymbols in symbolContext should always be well defined
#if ASSERTIONS
      const Tree* definition = symbolContext.expressionForUserNamed(e);
      assert(!definition || DeepCheckListLength(definition, symbolContext));
#endif
      return true;
    }
    case Type::UserFunction: {
      const Tree* definition = DefinitionWithCircularityCheck(e, symbolContext);
      if (definition) {
        Tree* functionEvaluation =
            CloneAndReplaceFunctionDefinition(e, definition);
        bool result = DeepCheckListLength(functionEvaluation, symbolContext);
        functionEvaluation->removeTree();
        return result;
      }
      return true;
    }
    case Type::Dep:
    case Type::DepList:
      // List length does not need to be coherent in dependencies
      return true;
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
        Dimension dim = Get(e, symbolContext);
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

int Dimension::ListLength(const Tree* e,
                          const Poincare::SymbolContext& symbolContext) {
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
    case Type::Dep:
    case Type::ListSort:
      return ListLength(e->child(0), symbolContext);
    case Type::List:
      return e->numberOfChildren();
    case Type::ListSequence: {
      float n = Approximation::To<float>(e->child(Parametric::k_valueIndex),
                                         Approximation::Parameters{});
      assert(std::floor(n) == n);
      assert(n >= 0 && n <= NAry::k_maxNumberOfChildren);
      return n;
    }
    case Type::ListSlice: {
      assert(Integer::Is<uint8_t>(e->child(1)) &&
             Integer::Is<uint8_t>(e->child(2)));
      int listLength = ListLength(e->child(0), symbolContext);
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
      const Tree* definition = symbolContext.expressionForUserNamed(e);
      if (definition) {
        return ListLength(definition, symbolContext);
      }
      return k_nonListListLength;
    }
    case Type::UserFunction: {
      const Tree* definition = DefinitionWithCircularityCheck(e, symbolContext);
      if (definition) {
        Tree* functionEvaluation =
            CloneAndReplaceFunctionDefinition(e, definition);
        int result = ListLength(functionEvaluation, symbolContext);
        functionEvaluation->removeTree();
        return result;
      }
      // Fallthrough so f({1,3,4}) returns 3. TODO : Maybe k_nonListListLength ?
      [[fallthrough]];
    }
    default: {
      // TODO sort lists first to optimize ListLength ?
      for (const Tree* child : e->children()) {
        int childListDim = ListLength(child, symbolContext);
        if (childListDim >= 0) {
          return childListDim;
        }
      }
      return k_nonListListLength;
    }
  }
}

bool Dimension::DeepCheckDimensions(
    const Tree* e, const Poincare::SymbolContext& symbolContext) {
  bool hasUnitChild = false;
  bool hasNonKelvinChild = false;
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (!DeepCheckDimensions(child, symbolContext)) {
      return false;
    }
    Dimension childDim = Get(child, symbolContext);
    if (childDim.isUnit()) {
      // Cannot mix non-Kelvin temperature unit with any unit.
      // TODO: UnitConvert should be able to handle this.
      if (hasNonKelvinChild) {
        return false;
      }
      if (childDim.hasNonKelvinTemperatureUnit() && !e->isUnitConversion()) {
        if (hasUnitChild) {
          return false;
        }
        hasNonKelvinChild = true;
      }
      hasUnitChild = true;
    }
    if (!e->isPiecewise() && !e->isParentheses() && !e->isDep() &&
        !e->isDepList() && !e->isList() && !e->isListSort() &&
        !e->isListSequence() && !List::IsEmptyList(child) &&
        childDim.isBoolean() != e->isLogicalOperatorOrBoolean()) {
      /* Only piecewises, parenthesis, dependencies, lists and boolean operators
       * can have boolean child. Boolean operators must have boolean child.
       * Empty lists which have been bubbled up are tolerated.
       * TODO: UserFunctions taking boolean parameters are not handled. If we
       * handle it, things should be reworked in UserFunction approximation too.
       */
      return false;
    }
    if (childDim.isPoint()) {
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
        case Type::List:
        case Type::ListSort:
          break;
        default:
          return false;
      }
    }
    assert(childDim.isSanitized());
  }
  return DeepCheckDimensionsAux(e, symbolContext, hasUnitChild,
                                hasNonKelvinChild);
}

/* To reduce the stack frame of the recursive method [DeepCheckDimensions], the
 * (unexplicably big) local variable [childDim] had to be located elsewhere.
 * The separation of the 2 functions allows this method to have a big stack
 * frame without risking a stack overflow on big Trees when evaluating the
 * recursive part of [DeepCheckDimensions] */
bool __attribute__((noinline))
Dimension::DeepCheckDimensionsAux(const Tree* e,
                                  const Poincare::SymbolContext& symbolContext,
                                  bool hasUnitChild, bool hasNonKelvinChild) {
  LazyArray<Dimension> childDim(e, symbolContext, Get);
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
    case Type::Div:
    case Type::Mult: {
      /* TODO: Forbid Complex * units. Units are already forbidden in complex
       * builtins. */
      uint8_t cols = 0;
      Units::SIVector unitVector = Units::SIVector::Empty();
      Units::SIVector posUnitVector = Units::SIVector::Empty();
      Units::SIVector negUnitVector = Units::SIVector::Empty();
      for (int i = 0; i < e->numberOfChildren(); i++) {
        bool secondDivisionChild = (i == 1 && e->isDiv());
        Dimension next = childDim[i];
        if (next.isMatrix()) {
          // Matrix size must match. Forbid Matrices on denominator
          if ((cols != 0 && cols != next.matrix.rows) || secondDivisionChild) {
            return false;
          }
          cols = next.matrix.cols;
        } else if (next.isUnit()) {
          if (hasNonKelvinChild && secondDivisionChild) {
            // Cannot divide by non-Kelvin temperature unit
            assert(next.hasNonKelvinTemperatureUnit());
            return false;
          }
          if (!unitVector.addAllCoefficients(next.unit.vector,
                                             secondDivisionChild ? -1 : 1,
                                             &posUnitVector, &negUnitVector)) {
            return false;
          }
        }
      }
      // Forbid units * matrices
      return cols == 0 || unitVector.isEmpty();
    }
    case Type::Pow:
    case Type::PowReal: {
      if (!childDim[1].isScalar()) {
        return false;
      }
      if (childDim[0].isMatrix()) {
        // Forbid non-integer power of matrices
        return childDim[0].isSquareMatrix() && IsIntegerExpression(e->child(1));
      }
      if (!childDim[0].isUnit()) {
        return true;
      }
      if (hasNonKelvinChild) {
        assert(childDim[0].hasNonKelvinTemperatureUnit());
        // Powers of non-Kelvin temperature unit are forbidden
        return false;
      }
      /* Forbid units of non-integer power or of too big of a power.
       * TODO_PCJ:  Handle the unit as a scalar if the index is not an integer
       */
      const Tree* exponent = e->child(1);
      if (!(IsIntegerExpression(exponent, true))) {
        return false;
      }
      float index =
          Approximation::To<float>(exponent, Approximation::Parameters{});
      assert(!std::isfinite(index) || std::round(index) == index);
      if (!std::isfinite(index) || index > static_cast<float>(INT8_MAX) ||
          (index < static_cast<float>(INT8_MIN))) {
        return false;
      }
      Units::SIVector unitVector = Units::SIVector::Empty();
      return unitVector.addAllCoefficients(childDim[0].unit.vector,
                                           static_cast<int8_t>(index));
    }
    case Type::Sum:
    case Type::Product:
      return childDim[Parametric::k_variableIndex].isScalar() &&
             childDim[Parametric::k_lowerBoundIndex].isScalar() &&
             childDim[Parametric::k_upperBoundIndex].isScalar() &&
             (!e->isProduct() ||
              childDim[Parametric::k_prodArgumentIndex].isScalar() ||
              childDim[Parametric::k_prodArgumentIndex].isSquareMatrix());

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
    case Type::Identity: {
#if 0
      // TODO check for unknowns and display error message if not integral
      if (!childDim[0].isScalar() || !IsIntegerExpression(e->child(0))) {
        return false;
      }
      float approx =
          Approximation::To<float>(e->child(0), Approximation::Parameters{});
      return std::isfinite(approx) && approx > 0;
#endif
      const Tree* size = e->child(0);
      return size->isOne() || size->isTwo() || size->isIntegerPosShort();
    }
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
            const Tree* child = e->child(i);
            if (List::IsEmptyList(child)) {
              // Empty lists which have been bubbled up are tolerated
              continue;
            }
            return false;
          }
        } else if (childDim[i] != childDim[0]) {
          return false;
        }
      }
      return true;
    }
    case Type::UnitConversion:
      /* Not using Dimension operator == because different representatives are
       * allowed. */
      return childDim[0].isUnit() && childDim[1].isUnit() &&
             (e->child(0)->isUndefUnit() ||
              childDim[0].unit.vector == childDim[1].unit.vector);
    case Type::DepList:
      /* For now units and booleans are allowed in depLists. For a later
       * refactoring, it would be better to have a dependency creation function
       * that ensures in advance that no units or booleans are injected in a
       * dependency. */
      return true;
    case Type::Dep:
      // Children can have a different dimension : [[x/x]] -> dep([[1]],
      // {1/x})
      return true;
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
    case Type::ListSequence: {
      if (hasUnitChild ||
          !IsIntegerExpression(e->child(Parametric::k_valueIndex))) {
        return false;
      }
      float n =
          Approximation::To<float>(e->child(1), Approximation::Parameters{});
      assert(!std::isfinite(n) || std::floor(n) == n);
      return std::isfinite(n) && n >= 0 && n <= NAry::k_maxNumberOfChildren;
    }
    case Type::ListSlice:
      return Integer::Is<uint8_t>(e->child(1)) &&
             Integer::Is<uint8_t>(e->child(2));
    /* Note: This might cause dimensional issues, notably because of Re and Im
     * not being handled. */
    case Type::Abs:
      // case Type::Sqrt:  TODO: Handle _m^(1/2)
      unitsAllowed = true;
      break;
    case Type::Matrix:
      break;
    case Type::AngleUnitContext:
    case Type::ListSort:
    case Type::Opposite:
    case Type::Parentheses:
      return true;
    case Type::RandIntNoRep: {
      if (!IsIntegerExpression(e->child(0)) ||
          !IsIntegerExpression(e->child(1)) ||
          !Integer::Is<uint8_t>(e->child(2))) {
        return false;
      }
      // NOTE Using double approx here to ensure the bounds are respected
      double a =
          Approximation::To<double>(e->child(0), Approximation::Parameters{});
      assert(!std::isfinite(a) || std::floor(a) == a);
      if (!std::isfinite(a) || INT32_MIN > a || a > INT32_MAX) {
        return false;
      }
      double b =
          Approximation::To<double>(e->child(1), Approximation::Parameters{});
      assert(!std::isfinite(b) || std::floor(b) == b);
      if (!std::isfinite(b) || INT32_MIN > b || b > INT32_MAX) {
        return false;
      }
      uint8_t n = Integer::Handler(e->child(2)).to<uint8_t>();
      return a <= b && n <= b - a + 1;
    }
    case Type::UserSymbol: {
      // UserSymbols in symbolContext should always be well defined
#if ASSERTIONS
      const Tree* definition = symbolContext.expressionForUserNamed(e);
      assert(!definition || DeepCheckDimensions(definition, symbolContext));
#endif
      return true;
    }
    case Type::UserFunction: {
      const Tree* definition = DefinitionWithCircularityCheck(e, symbolContext);
      if (definition) {
        Tree* functionEvaluation =
            CloneAndReplaceFunctionDefinition(e, definition);
        bool result = DeepCheckDimensions(functionEvaluation, symbolContext);
        functionEvaluation->removeTree();
        return result;
      }
      [[fallthrough]];
    }
    case Type::UserSequence:
      return childDim[0].isScalar();
    case Type::NonNull:
    case Type::Real:
    case Type::RealPos:
    case Type::RealInteger:
      return childDim[0].isScalar() || childDim[0].isUnit();
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

Dimension Dimension::Get(const Tree* e,
                         const Poincare::SymbolContext& symbolContext) {
  switch (e->type()) {
    case Type::Div:
    case Type::Mult: {
      uint8_t rows = 0;
      uint8_t cols = 0;
      const Units::Representative* representative = nullptr;
      Units::SIVector unitVector = Units::SIVector::Empty();
      bool secondDivisionChild = false;
      for (const Tree* child : e->children()) {
        Dimension dim = Get(child, symbolContext);
        if (dim.isMatrix()) {
          if (rows == 0) {
            rows = dim.matrix.rows;
          }
          cols = dim.matrix.cols;
        } else if (dim.isUnit()) {
          [[maybe_unused]] bool success = unitVector.addAllCoefficients(
              dim.unit.vector, secondDivisionChild ? -1 : 1);
          assert(success);
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
      return Get(e->child(Parametric::FunctionIndex(e)), symbolContext);
    case Type::Dep:
      return Get(Dependency::Main(e), symbolContext);
    case Type::PowReal:
    case Type::Pow: {
      Dimension dim = Get(e->child(0), symbolContext);
      if (dim.isUnit()) {
        float index = Approximation::To<float>(
            e->child(1), Approximation::Parameters{},
            Approximation::Context(AngleUnit::None, ComplexFormat::None,
                                   symbolContext));
        assert(!std::isnan(index) && index <= static_cast<float>(INT8_MAX) &&
               index >= static_cast<float>(INT8_MIN) &&
               std::round(index) == index);
        Units::SIVector unitVector = Units::SIVector::Empty();
        [[maybe_unused]] bool success = unitVector.addAllCoefficients(
            dim.unit.vector, static_cast<int8_t>(index));
        assert(success);
        dim = unitVector.isEmpty() ? Scalar()
                                   : Unit(unitVector, dim.unit.representative);
      }
      return dim;
    }
    case Type::Add:
      return e->numberOfChildren() > 0 ? Get(e->child(0), symbolContext)
                                       : Scalar();
    case Type::AngleUnitContext:
    case Type::Abs:
    case Type::Opposite:
    case Type::Sqrt:
    case Type::Sub:
    case Type::Cross:
    case Type::Inverse:
    case Type::Ref:
    case Type::Rref:
    case Type::Piecewise:
    case Type::Parentheses:
    case Type::ListElement:
    case Type::ListSort:
      return Get(e->child(0), symbolContext);
    case Type::Matrix:
      return Matrix(Matrix::NumberOfRows(e), Matrix::NumberOfColumns(e));
    case Type::Dim:
      return Get(e->child(0), symbolContext).isMatrix() ? Matrix(1, 2)
                                                        : Scalar();
    case Type::Transpose: {
      Dimension dim = Get(e->child(0), symbolContext);
      return Matrix(dim.matrix.cols, dim.matrix.rows);
    }
    case Type::Identity: {
#if 0
      uint8_t n =
          Approximation::To<float>(e->child(0), Approximation::Parameters{});
#endif
      const Tree* child = e->child(0);
      assert(child->isOne() || child->isTwo() || child->isIntegerPosShort());
      uint8_t size = Rational::Numerator(child).to<uint8_t>();
      return Matrix(size, size);
    }
    case Type::UnitConversion:
      /* Use first child because it's representative is needed in
       * Unit::ProjectToBestUnits in case of non kelvin units. */
      return Get(e->child(0), symbolContext);
    case Type::Unit:
      return Dimension::Unit(e);
    case Type::PhysicalConstant:
      return Dimension::Unit(PhysicalConstant::GetProperties(e).m_dimension,
                             nullptr);
    case Type::Point:
      return Point();
    case Type::ListSlice:
    case Type::List:
      return ListLength(e, symbolContext) > 0 ? Get(e->child(0), symbolContext)
                                              : Scalar();
    case Type::UserSymbol: {
      const Tree* definition = symbolContext.expressionForUserNamed(e);
      if (definition) {
        return Get(definition, symbolContext);
      }
      return Scalar();
    }
    case Type::UserFunction: {
      const Tree* definition = DefinitionWithCircularityCheck(e, symbolContext);
      if (definition) {
        Tree* functionEvaluation =
            CloneAndReplaceFunctionDefinition(e, definition);
        Dimension result = Get(functionEvaluation, symbolContext);
        functionEvaluation->removeTree();
        return result;
      }
      /* Without definition, consider the function to be scalar. */
      return Scalar();
    }
    case Type::UndefBoolean: {
      return Boolean();
    }
    case Type::UndefUnit: {
      return Dimension::Unit(Units::SIVector::Undef(), nullptr);
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
    if (unit.vector.isUndef() || other.unit.vector.isUndef()) {
      // UndefUnif has the same dimension as any other unit.
      return true;
    }
    return unit.vector == other.unit.vector &&
           (unit.vector != Units::Temperature::Dimension ||
            unit.representative == other.unit.representative);
  }
  return true;
}

}  // namespace Poincare::Internal
