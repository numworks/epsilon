#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>

namespace Poincare::Internal {
class Tree;
}  // namespace Poincare::Internal

namespace Poincare {

/* Expressions that have been prepared for approximation. It can depend on a
 * Variable of index 0 called Var0. */
class PreparedFunction : public Expression {
 public:
  /* Static builders */
  static PreparedFunction Builder(const Internal::Tree* tree);
  // Eat the tree
  static PreparedFunction Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static PreparedFunction Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }

  /* General helpers */
  PreparedFunction cloneChildAtIndex(int i) const;
  PreparedFunction clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<PreparedFunction&>(clone);
  }

  /* Other helpers */
  template <typename T>
  Coordinate2D<T> approximateToPoint() const;
  // Approximate to real scalar replacing Var0 with value.
  template <typename T>
  T approximateToRealScalarWithValue(T x, int listElement = -1) const;
  // Approximate to PointOrRealScalar replacing Var0 with value.
  template <typename T>
  PointOrRealScalar<T> approximateToPointOrRealScalarWithValue(T x) const;

  /* Properties getters */
  bool involvesDiscontinuousFunction() const;
  template <typename T>
  bool isDiscontinuousOnInterval(T minBound, T maxBound) const;
  bool hasSequences() const;
  bool approximationBasedOnCostlyAlgorithms() const;
};

/* Aliases used to specify a Expression's type. TODO: split them into
 * actual classes to prevent casting one into another. */
// PreparedFunction with Scalar approximation
using PreparedFunctionScalar = PreparedFunction;
// PreparedFunction with Point approximation
using PreparedFunctionPoint = PreparedFunction;

}  // namespace Poincare
