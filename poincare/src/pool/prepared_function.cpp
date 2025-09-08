#include <poincare/prepared_function.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/continuity.h>

namespace Poincare {

using namespace Internal;

PreparedFunction PreparedFunction::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

template <typename T>
T PreparedFunctionScalar::approximateToRealScalarWithValue(
    T x, int listElement) const {
  return Approximation::To<T>(
      tree(), x, Approximation::Parameters{.isRootAndCanHaveRandom = true},
      Approximation::Context(AngleUnit::None, ComplexFormat::None,
                             EmptySymbolContext{}, listElement));
}

bool PreparedFunction::involvesDiscontinuousFunction() const {
  return Continuity::InvolvesDiscontinuousFunction(tree());
}

template <typename T>
bool PreparedFunction::isDiscontinuousOnInterval(T minBound, T maxBound) const {
  return Continuity::IsDiscontinuousOnInterval<T>(tree(), minBound, maxBound);
}

template <typename T>
PointOrRealScalar<T> PreparedFunction::approximateToPointOrRealScalarWithValue(
    T x) const {
  return Internal::Approximation::ToPointOrRealScalar<T>(
      tree(), x, Approximation::Parameters{.isRootAndCanHaveRandom = true});
}

template <typename T>
Coordinate2D<T> PreparedFunction::approximateToPoint() const {
  return Approximation::ToPoint<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = false});
}

/* TODO: Find a better way to avoid duplication of Builder, clone, and
 *       cloneChildAtIndex methods with other types of expressions. */
PreparedFunction PreparedFunction::Builder(const Tree* tree) {
  PoolHandle p = BuildPoolHandleFromTree(tree);
  return static_cast<PreparedFunction&>(p);
}

PreparedFunction PreparedFunction::Builder(Tree* tree) {
  PreparedFunction result = Builder(const_cast<const Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

bool PreparedFunction::hasSequences() const {
  return !isUninitialized() &&
         tree()->hasDescendantSatisfying(
             [](const Tree* e) { return e->isSequence(); });
}

bool PreparedFunction::approximationBasedOnCostlyAlgorithms() const {
  return !isUninitialized() &&
         tree()->hasDescendantSatisfying([](const Tree* e) {
           return e->isSequence() || e->isParametric();
         });
}

template float PreparedFunctionScalar::approximateToRealScalarWithValue<float>(
    float, int) const;
template double
PreparedFunctionScalar::approximateToRealScalarWithValue<double>(double,
                                                                 int) const;
template PointOrRealScalar<float>
PreparedFunction::approximateToPointOrRealScalarWithValue<float>(float) const;
template PointOrRealScalar<double>
PreparedFunction::approximateToPointOrRealScalarWithValue<double>(double) const;
template Coordinate2D<float> PreparedFunction::approximateToPoint<float>()
    const;
template Coordinate2D<double> PreparedFunction::approximateToPoint<double>()
    const;
template bool PreparedFunction::isDiscontinuousOnInterval<float>(float,
                                                                 float) const;
template bool PreparedFunction::isDiscontinuousOnInterval<double>(double,
                                                                  double) const;

}  // namespace Poincare
