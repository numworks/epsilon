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

AnalysisExpression AnalysisExpression::getPolynomialReducedCoefficients(
    const char* symbolName, bool keepDependencies) const {
  return Builder(PolynomialParser::GetReducedCoefficients(tree(), symbolName,
                                                          keepDependencies));
}

template <typename T>
T AnalysisExpression::approximateAnalysisToRealScalar() const {
  if (!Internal::Dimension::IsNonListScalar(tree())) {
    return static_cast<T>(NAN);
  }
  return Approximation::To<T>(
      tree(), Approximation::Parameters{
                  .policy = {.optimize = true},
                  .expressionParameters =
                      Approximation::SystemExpressionPolicy{.prepare = true}});
}

template float AnalysisExpression::approximateAnalysisToRealScalar<float>()
    const;
template double AnalysisExpression::approximateAnalysisToRealScalar<double>()
    const;

}  // namespace Poincare
