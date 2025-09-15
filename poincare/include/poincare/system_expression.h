#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>
#include <poincare/preferences.h>
#include <poincare/projection_context.h>

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class UserExpression;
class PreparedFunction;

/* Expressions that have been projected and systematic reduced. */
class SystemExpression : public Expression {
 public:
  /* Static builders */
  static SystemExpression ExpressionFromAddress(const void* address,
                                                size_t size);
  static SystemExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static SystemExpression Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static SystemExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  static SystemExpression Builder(int32_t n);
  template <typename T>
  static SystemExpression Builder(T x);
  template <typename T>
  static SystemExpression Builder(Coordinate2D<T> point);
  template <typename T>
  static SystemExpression Builder(PointOrRealScalar<T> pointOrReal);
  // Build an expression of Undefined tree.
  static SystemExpression Undefined();

  static SystemExpression DecimalBuilderFromDouble(double v);
  static SystemExpression RationalBuilder(int32_t numerator,
                                          int32_t denominator);
  static SystemExpression CreateReduce(const Internal::Tree* structure,
                                       Internal::ContextTrees ctx);
  static SystemExpression CreateIntegralOfAbsOfDifference(
      SystemExpression lowerBound, SystemExpression upperBound,
      SystemExpression integrandA, SystemExpression integrandB);

  /* General helpers */
  SystemExpression cloneChildAtIndex(int i) const;
  SystemExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<SystemExpression&>(clone);
  }

  /* SystemExpression to SystemExpression helpers */
  template <typename T>
  SystemExpression approximateSystemToTree() const;
  // Replace symbol and reduce.
  SystemExpression cloneAndReplaceSymbolWithExpression(
      const char* symbolName, const SystemExpression& replaceSymbolWith,
      bool* reductionFailure, SymbolicComputation symbolic) const;
  SystemExpression getReducedDerivative(const char* symbolName,
                                        int derivationOrder = 1) const;
  // Return SystemExpression with sorted approximated elements.
  template <typename T>
  SystemExpression approximateListAndSort() const;
  // Return SystemExpression with undef list elements or points removed.
  SystemExpression removeUndefAndComplexListElements() const;

  /* Other helpers */

  // Expressions in parameters are outputs.
  void cloneAndBeautifyAndApproximate(UserExpression* beautifiedExpression,
                                      UserExpression* approximatedExpression,
                                      ProjectionContext& context) const;
  UserExpression cloneAndBeautify(ProjectionContext& context) const;
  /* Replace some UserSymbol into Var0 for
   * approximateToPointOrRealScalarWithValue. Returns undef if the expression's
   * dimension is not point or scalar. If scalarsOnly = true, returns undef if
   * it's a point or a list. */
  PreparedFunction getPreparedFunction(const char* symbolName,
                                       bool scalarsOnly = false) const;
  template <typename T>
  T approximateSystemToRealScalar() const;
  template <typename T>
  Coordinate2D<T> approximateToPoint() const;

  /* Properties getters */

  Sign sign() const;
  int polynomialDegree(const char* symbolName) const;
  /* Fills the table coefficients with the expressions of the first 3 polynomial
   * coefficients and returns the  polynomial degree. It is supposed to be
   * called on a reduced expression. coefficients has up to 3 entries.  */
  int getPolynomialReducedCoefficients(const char* symbolName,
                                       SystemExpression coefficients[],
                                       bool keepDependencies = false) const;
};

}  // namespace Poincare
