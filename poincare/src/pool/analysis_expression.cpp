#include <poincare/analysis_expression.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/polynomial.h>

namespace Poincare {

using namespace Internal;

AnalysisExpression AnalysisExpression::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

/* TODO: Find a better way to avoid duplication of Builder, clone, and
 *       cloneChildAtIndex methods with other types of expressions. */
AnalysisExpression AnalysisExpression::Builder(const Tree* tree) {
  PoolHandle p = BuildPoolHandleFromTree(tree);
  return static_cast<AnalysisExpression&>(p);
}

AnalysisExpression AnalysisExpression::Builder(Tree* tree) {
  AnalysisExpression result = Builder(const_cast<const Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

int AnalysisExpression::polynomialDegree(const char* symbolName) const {
  return Degree::Get(tree(), symbolName);
}

int AnalysisExpression::getPolynomialReducedCoefficients(
    const char* symbolName, AnalysisExpression coefficients[],
    bool keepDependencies) const {
  Tree* coefList = PolynomialParser::GetReducedCoefficients(tree(), symbolName,
                                                            keepDependencies);
  if (!coefList) {
    return -1;
  }
  int degree = coefList->numberOfChildren() - 1;
  assert(degree >= 0);
  Tree* child = coefList->nextNode();
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = Builder(child);
  }
  coefList->removeNode();
  return degree;
}

template <typename T>
T AnalysisExpression::approximateSystemToRealScalar() const {
  if (!Internal::Dimension::IsNonListScalar(tree())) {
    return NAN;
  }
  return Approximation::To<T>(
      tree(),
      Approximation::Parameters{.policy = {.prepare = true, .optimize = true}});
}

template float AnalysisExpression::approximateSystemToRealScalar<float>() const;
template double AnalysisExpression::approximateSystemToRealScalar<double>()
    const;

}  // namespace Poincare
