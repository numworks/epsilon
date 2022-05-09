#include <poincare/distribution_function.h>
#include <poincare/binomial_distribution.h>
#include <poincare/geometric_distribution.h>
#include <poincare/student_distribution.h>
#include <poincare/poisson_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/distribution_method.h>
#include <poincare/cdf_function.h>
#include <poincare/cdf_range_function.h>
#include <poincare/pdf_function.h>
#include <poincare/inv_function.h>
#include <poincare/simplification_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDF::s_functionHelper;
constexpr Expression::FunctionHelper NormCDFRange::s_functionHelper;
constexpr Expression::FunctionHelper NormPDF::s_functionHelper;
constexpr Expression::FunctionHelper InvNorm::s_functionHelper;

constexpr Expression::FunctionHelper StudentCDF::s_functionHelper;
constexpr Expression::FunctionHelper StudentCDFRange::s_functionHelper;
constexpr Expression::FunctionHelper StudentPDF::s_functionHelper;
constexpr Expression::FunctionHelper InvStudent::s_functionHelper;

constexpr Expression::FunctionHelper PoissonCDF::s_functionHelper;
constexpr Expression::FunctionHelper PoissonPDF::s_functionHelper;

constexpr Expression::FunctionHelper BinomCDF::s_functionHelper;
constexpr Expression::FunctionHelper BinomPDF::s_functionHelper;
constexpr Expression::FunctionHelper InvBinom::s_functionHelper;

constexpr Expression::FunctionHelper GeomCDF::s_functionHelper;
constexpr Expression::FunctionHelper GeomCDFRange::s_functionHelper;
constexpr Expression::FunctionHelper GeomPDF::s_functionHelper;
constexpr Expression::FunctionHelper InvGeom::s_functionHelper;

Layout DistributionFunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(DistributionFunction(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int DistributionFunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression DistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return DistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression DistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  int childIndex = 0;
  Expression abscissa[DistributionMethod::k_maxNumberOfParameters];
  for (int i=0; i < DistributionMethod::numberOfParameters(functionType()); i++) {
    abscissa[i] = childAtIndex(childIndex++);
  }
  Expression parameters[Distribution::k_maxNumberOfParameters];
  for (int i=0; i < Distribution::numberOfParameters(distributionType()); i++) {
    parameters[i] = childAtIndex(childIndex++);
  }
  const DistributionMethod * function = DistributionMethod::Get(functionType());
  const Distribution * distribution = Distribution::Get(distributionType());

  bool parametersAreOk;
  bool couldCheckParameters = distribution->ExpressionParametersAreOK(&parametersAreOk, parameters, context);

  if (!couldCheckParameters) {
    return *this;
  }
  if (!parametersAreOk) {
    return replaceWithUndefinedInPlace();
  }

  Expression e = function->shallowReduce(abscissa, context, this);
  if (!e.isUninitialized()) {
    return e;
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

template<typename T>
Evaluation<T> DistributionFunctionNode::templatedApproximate(ApproximationContext approximationContext) const {
  int childIndex = 0;
  T abscissa[DistributionMethod::k_maxNumberOfParameters];
  for (int i=0; i < DistributionMethod::numberOfParameters(m_functionType); i++) {
    Evaluation<T> evaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
    abscissa[i] = evaluation.toScalar();
  }
  T parameters[Distribution::k_maxNumberOfParameters];
  for (int i=0; i < Distribution::numberOfParameters(m_distributionType); i++) {
    Evaluation<T> evaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
    parameters[i] = evaluation.toScalar();
  }
  const DistributionMethod * function = DistributionMethod::Get(m_functionType);
  const Distribution * distribution = Distribution::Get(m_distributionType);
  return Complex<T>::Builder(function->EvaluateAtAbscissa(abscissa, distribution, parameters));
}

}
