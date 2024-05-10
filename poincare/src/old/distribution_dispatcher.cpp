#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/distribution_dispatcher.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

constexpr OExpression::FunctionHelper NormCDF::s_functionHelper;
constexpr OExpression::FunctionHelper NormCDFRange::s_functionHelper;
constexpr OExpression::FunctionHelper NormPDF::s_functionHelper;
constexpr OExpression::FunctionHelper InvNorm::s_functionHelper;

constexpr OExpression::FunctionHelper StudentCDF::s_functionHelper;
constexpr OExpression::FunctionHelper StudentCDFRange::s_functionHelper;
constexpr OExpression::FunctionHelper StudentPDF::s_functionHelper;
constexpr OExpression::FunctionHelper InvStudent::s_functionHelper;

constexpr OExpression::FunctionHelper PoissonCDF::s_functionHelper;
constexpr OExpression::FunctionHelper PoissonPDF::s_functionHelper;

constexpr OExpression::FunctionHelper BinomCDF::s_functionHelper;
constexpr OExpression::FunctionHelper BinomPDF::s_functionHelper;
constexpr OExpression::FunctionHelper InvBinom::s_functionHelper;

constexpr OExpression::FunctionHelper GeomCDF::s_functionHelper;
constexpr OExpression::FunctionHelper GeomCDFRange::s_functionHelper;
constexpr OExpression::FunctionHelper GeomPDF::s_functionHelper;
constexpr OExpression::FunctionHelper InvGeom::s_functionHelper;

constexpr OExpression::FunctionHelper HypergeomCDF::s_functionHelper;
constexpr OExpression::FunctionHelper HypergeomCDFRange::s_functionHelper;
constexpr OExpression::FunctionHelper HypergeomPDF::s_functionHelper;
constexpr OExpression::FunctionHelper InvHypergeom::s_functionHelper;

size_t DistributionDispatcherNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits, name());
}

int DistributionDispatcherNode::simplificationOrderSameType(
    const ExpressionNode *e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  const DistributionDispatcherNode *other =
      static_cast<const DistributionDispatcherNode *>(e);
  if (m_distributionType < other->m_distributionType) {
    return -1;
  } else if (m_distributionType > other->m_distributionType) {
    return 1;
  }
  if (m_methodType < other->m_methodType) {
    return -1;
  } else if (m_methodType > other->m_methodType) {
    return 1;
  }
  return ExpressionNode::simplificationOrderSameType(e, ascending,
                                                     ignoreParentheses);
}

OExpression DistributionDispatcherNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return DistributionDispatcher(this).shallowReduce(reductionContext);
}

OExpression DistributionDispatcher::shallowReduce(
    ReductionContext reductionContext, bool *stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }

#if 0  // TODO_PCJ
  int childIndex = 0;
  OExpression abscissae[DistributionMethod::k_maxNumberOfParameters];
  for (int i = 0; i < DistributionMethod::numberOfParameters(methodType());
       i++) {
    abscissae[i] = childAtIndex(childIndex++);
  }
  OExpression parameters[Distribution::k_maxNumberOfParameters];
  for (int i = 0; i < Distribution::numberOfParameters(distributionType());
       i++) {
    parameters[i] = childAtIndex(childIndex++);
  }
  const DistributionMethod *function = DistributionMethod::Get(methodType());
  const Distribution *distribution = Distribution::Get(distributionType());

  bool parametersAreOk;
  bool couldCheckParameters = distribution->expressionParametersAreOK(
      &parametersAreOk, parameters, reductionContext.context());

  if (!couldCheckParameters) {
    return *this;
  }
  if (!parametersAreOk) {
    return replaceWithUndefinedInPlace();
  }

  OExpression e = function->shallowReduce(abscissae, distribution, parameters,
                                          reductionContext, this);
  if (!e.isUninitialized()) {
    return e;
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
#else
  // Implemented in Internal::Simplification::SimplifyDistribution
  assert(false);
#endif
  return *this;
}

template <typename T>
Evaluation<T> DistributionDispatcherNode::templatedApproximate(
    const ApproximationContext &approximationContext) const {
  return ApproximationHelper::Map<T>(
      this, approximationContext,
      [](const std::complex<T> *c, int numberOfComplexes,
         Preferences::ComplexFormat complexFormat,
         Preferences::AngleUnit angleUnit, void *ctx) -> std::complex<T> {
        const DistributionDispatcherNode *self =
            static_cast<DistributionDispatcherNode *>(ctx);
        int childIndex = 0;
        T abscissa[DistributionMethod::k_maxNumberOfParameters];
        for (int i = 0;
             i < DistributionMethod::numberOfParameters(self->m_methodType);
             i++) {
          abscissa[i] = ComplexNode<T>::ToScalar(c[childIndex++]);
        }
        T parameters[Distribution::k_maxNumberOfParameters];
        for (int i = 0;
             i < Distribution::numberOfParameters(self->m_distributionType);
             i++) {
          parameters[i] = ComplexNode<T>::ToScalar(c[childIndex++]);
        }
        const DistributionMethod *function =
            DistributionMethod::Get(self->m_methodType);
        const Distribution *distribution =
            Distribution::Get(self->m_distributionType);
        return function->EvaluateAtAbscissa(abscissa, distribution, parameters);
      },
      ApproximationHelper::UndefinedOnBooleans, true,
      static_cast<void *>(const_cast<DistributionDispatcherNode *>(this)));
}

}  // namespace Poincare
