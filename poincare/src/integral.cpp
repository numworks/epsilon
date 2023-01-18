#include <poincare/integral.h>
#include <poincare/complex.h>
#include <poincare/integral_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
#include <algorithm>
#include <cmath>
#include <float.h>
#include <stdlib.h>

namespace Poincare {

constexpr Expression::FunctionHelper Integral::s_functionHelper;

int IntegralNode::numberOfChildren() const { return Integral::s_functionHelper.numberOfChildren(); }

int IntegralNode::polynomialDegree(Context * context, const char * symbolName) const {
  if (childAtIndex(0)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(1)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(2)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(3)->polynomialDegree(context, symbolName) == 0)
  {
    // If no child depends on the symbol, the polynomial degree is 0.
    return 0;
  }
  return ExpressionNode::polynomialDegree(context, symbolName);
}

Layout IntegralNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return IntegralLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int IntegralNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Integral::s_functionHelper.aliasesList().mainAlias());
}

Expression IntegralNode::shallowReduce(const ReductionContext& reductionContext) {
  return Integral(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> IntegralNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  /* TODO : Reduction is mapped on list, but not approximation.
  * Find a smart way of doing it. */
  Evaluation<T> aInput = childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(3)->approximate(T(), approximationContext);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  if (std::isnan(a) || std::isnan(b)) {
    return Complex<T>::RealUndefined();
  }
  bool fIsNanInA = std::isnan(firstChildScalarValueForArgument(a, approximationContext));
  bool fIsNanInB = std::isnan(firstChildScalarValueForArgument(b, approximationContext));
  // The integrand has a singularity on a bound of the interval, use tanh-sinh quadrature
  if (fIsNanInA || fIsNanInB) {
    /* When we have a singularity at a bound, we want to evaluate the integrand
     * really close to the bound since the area there is non-negligible.  If the
     * bound is non-null, say 1/sqrt(1-x) near 1, the closest point from 1 where
     * the integrand can be evaluated is 1-1e-15. However by using reduction to
     * simplify the expression near one 1/sqrt(1-(1-dx)) = 1/sqrt(dx) we can
     * evaluate the integrand really close to 1 (about 1-1e-300). */
    AlternativeIntegrand alternativeIntegrand;
    alternativeIntegrand.a = a;
    alternativeIntegrand.b = b;
    /* We need SystemForAnalysis to remove the constant part by expanding
     * polynomials introduced by the replacement, e.g. 1-(1-x)^2 -> 2x-x^2 */
    const ReductionContext reductionContext(approximationContext.context(), approximationContext.complexFormat(), approximationContext.angleUnit(), Preferences::UnitFormat::Metric, ReductionTarget::SystemForAnalysis);
    /* Rewrite the integrand to be able to compute it directly at abscissa a + x */
    if (fIsNanInA && a != 0) {
      alternativeIntegrand.integrandNearA = rewriteIntegrandNear(childAtIndex(2), reductionContext);
    }
    // Same near b - x
    if (fIsNanInB && b != 0) {
      alternativeIntegrand.integrandNearB = rewriteIntegrandNear(childAtIndex(3), reductionContext);
    }
    /* We are using 4 levels of refinement which means ≈ 64 integrand evaluations
     * = 2 (R- and R+) * 2^4 (ticks/unit) * 4 (typical decay on the examples)
     * It could be increased but precision is likely lost somewhere else in
     * hard examples. */
    DetailedResult<T> detailedResult = tanhSinhQuadrature<T>(4, alternativeIntegrand, approximationContext);
    // Arbitrary value to have the best choice of quadrature on the examples
    constexpr T insufficientPrecision = 0.001;
    if (!std::isnan(detailedResult.integral) && detailedResult.absoluteError < insufficientPrecision) {
      return Complex<T>::Builder(detailedResult.integral);
    }
  }
  /* Choose the right substitution to use in Gauss-Konrod.
   * We are using a (0,inf) -> (-1,1) like substitution when a bound is above
   * these thresholds to deal with big but finite bounds */
  constexpr T leftOpenThreshold = -1000.0;
  constexpr T rightOpenThreshold = 1000.0;
  T start, end, scale = 1.0;
  if (a > b) {
    scale = -1.0;
    std::swap(a, b);
  }
  Substitution<T> substitution;
  substitution.originA = a;
  substitution.originB = b;
  if (a < leftOpenThreshold) {
    if (b > rightOpenThreshold) {
      substitution.type = Substitution<T>::Type::RealLine;
      // Two solutions but we need the one with the sign of a (resp. b)
      start = std::isfinite(a) ? ((std::sqrt(4*a*a+1.0)-1.0)/(2.0*a)) : -1.0;
      end = std::isfinite(b) ? ((std::sqrt(4*b*b+1.0)-1.0)/(2.0*b)) : 1.0;
    } else {
      substitution.type = Substitution<T>::Type::LeftOpen;
      start = std::isfinite(a) ? (a-b+1.0)/(b-a+1.0) : -1.0;
      end = 1.0;
      scale *= 2.0;
    }
  } else {
    if (b > rightOpenThreshold) {
      substitution.type = Substitution<T>::Type::RightOpen;
      start = std::isfinite(b) ? (a-b+1.0)/(b-a+1.0) : -1.0;
      end = 1.0;
      scale *= 2.0;
    } else {
      substitution.type = Substitution<T>::Type::None;
      start = a;
      end = b;
    }
  }
  /* The tolerance sqrt(eps) estimated by the method is an upper bound and the
   * real is error is typically eps */
  constexpr T precision = Float<T>::SqrtEpsilonLax();
  DetailedResult<T> detailedResult = adaptiveQuadrature<T>(start, end, precision, k_maxNumberOfIterations, substitution, approximationContext);
  constexpr T minimumPrecisionForDisplay = 0.1;
  T result = detailedResult.absoluteError > minimumPrecisionForDisplay ? NAN : scale * detailedResult.integral;
  return Complex<T>::Builder(result);
}

template<typename T>
T IntegralNode::integrand(T x, Substitution<T> substitution, const ApproximationContext& approximationContext) const {
  switch (substitution.type) {
  case Substitution<T>::Type::None:
    return firstChildScalarValueForArgument(x, approximationContext);
  case Substitution<T>::Type::LeftOpen:
  {
    T z = 1.0 / (x + 1.0);
    T arg = substitution.originB - (2.0 * z - 1.0);
    return firstChildScalarValueForArgument(arg, approximationContext) * z * z;
  }
  case Substitution<T>::Type::RightOpen:
  {
    T z = 1.0 / (x + 1);
    T arg = 2.0 * z + substitution.originA - 1.0;
    return firstChildScalarValueForArgument(arg, approximationContext) * z * z;
  }
  default:
  {
    assert(substitution.type == Substitution<T>::Type::RealLine);
    T x2 = x * x;
    T inv = 1.0 / (1.0 - x2);
    T w = (1.0 + x2) * inv * inv;
    T arg = x * inv;
    return firstChildScalarValueForArgument(arg, approximationContext) * w;
  }
  }
}

Expression IntegralNode::rewriteIntegrandNear(Expression bound, const ReductionContext& reductionContext) const {
  Expression integrand = Expression(childAtIndex(0)).clone();
  Symbol symbol = Expression(childAtIndex(1)).clone().convert<Symbol>();
  integrand.replaceSymbolWithExpression(symbol, Addition::Builder(bound.clone(), symbol));
  return integrand.deepReduce(reductionContext);
}

template<typename T>
T IntegralNode::integrandNearBound(T x, T xc, AlternativeIntegrand alternativeIntegrand, const ApproximationContext& approximationContext) const {
  T scale = (alternativeIntegrand.b - alternativeIntegrand.a) / 2.0;
  T arg = xc * scale;
  if (x < 0) {
    if (!alternativeIntegrand.integrandNearA.isUninitialized()) {
      return approximateExpressionWithArgument(alternativeIntegrand.integrandNearA.node(), arg, approximationContext).toScalar() * scale;
    }
    arg = arg + alternativeIntegrand.a;
  } else {
    if (!alternativeIntegrand.integrandNearB.isUninitialized()) {
      return approximateExpressionWithArgument(alternativeIntegrand.integrandNearB.node(), -arg, approximationContext).toScalar() * scale;
    }
    arg = alternativeIntegrand.b - arg;
  }
  return firstChildScalarValueForArgument(arg, approximationContext) * scale;
}

/* Tanh-Sinh quadrature
 * cf https://www.davidhbailey.com/dhbpapers/dhb-tanh-sinh.pdf */
template<typename T>
IntegralNode::DetailedResult<T> IntegralNode::tanhSinhQuadrature(int level, AlternativeIntegrand alternativeIntegrand, const ApproximationContext& approximationContext) const {
  T h = 2.0;
  T result = M_PI_2 * integrandNearBound(0.0, 1.0, alternativeIntegrand, approximationContext); // j=0
  int j = 1;
  T sn2 = 0, sn1 = 0;
  T maxWjFj = 0;
  for (int k=0; k<level; k++) {
    sn2 = sn1;
    sn1 = result * h;
    h = h / 2.0;
    j = 1;
    bool leftOk = true;
    bool rightOk = true;
    while (leftOk || rightOk) {
      T sinh = M_PI_2 * std::sinh(h * j);
      T cs = std::cosh(sinh);
      T weight = M_PI_2 * std::cosh(h * j) / (cs * cs);
      T abscissa = std::tanh(sinh);
      T distanceToBound = 1.0 / (std::exp(sinh) * std::cosh(sinh));
      if (leftOk) {
        T leftValue = integrandNearBound(-abscissa, distanceToBound, alternativeIntegrand, approximationContext);
        if (std::isnan(leftValue)) {
          leftOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * leftValue));
          result += weight * leftValue;
        }
        // criterion used in boost: abs(y * weights) > abs(L1_I0 * tail_tolerance) but
        // L1_IO is abs(pi/2 * f(0)) before the first row and tail_tolerance = tolerance^2
        if (std::abs(weight * leftValue) < Float<T>::EpsilonLax()) leftOk = false;
      }
      if (rightOk) {
        T rightValue = integrandNearBound(abscissa, distanceToBound, alternativeIntegrand, approximationContext);
        if (std::isnan(rightValue)) {
          rightOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * rightValue));
          result += weight * rightValue;
        }
        if (std::abs(weight * rightValue) < Float<T>::EpsilonLax()) rightOk = false;
      }
      // computing only odd ticks after the first level
      if (k) {
        j += 2;
      } else {
        j += 1;
      }
    }
  }
  T error;
  T sn = h * result;
  if (sn == sn1) {
    error = 0.0;
  } else {
    /* We need to have an estimation of the precision to be able to know that
     * 1/x is nonintegrable between 0 and 1 for instance */
    T d1 = std::log10(std::abs(sn-sn1));
    T d2 = std::log10(std::abs(sn1-sn2));
    T d3 = std::log10(maxWjFj) - 15;
    T d = std::max({d1*d1/d2, 2*d1, d3});
    error = std::pow(static_cast<T>(10.0), d);
  }
  DetailedResult<T> detailedResult;
  detailedResult.integral = sn;
  detailedResult.absoluteError = error;
  return detailedResult;
}

#ifdef LAGRANGE_METHOD

template<typename T>
T IntegralNode::lagrangeGaussQuadrature(T a, T b, const ApproximationContext& approximationContext) const {
  /* We here use Gauss-Legendre quadrature with n = 5
   * Gauss-Legendre abscissae and weights can be found in
   * C/C++ library source code. */
  constexpr T x[10]={0.0765265211334973337546404, 0.2277858511416450780804962, 0.3737060887154195606725482, 0.5108670019508270980043641,
   0.6360536807265150254528367, 0.7463319064601507926143051, 0.8391169718222188233945291, 0.9122344282513259058677524,
   0.9639719272779137912676661, 0.9931285991850949247861224};
  constexpr T w[10]={0.1527533871307258506980843, 0.1491729864726037467878287, 0.1420961093183820513292983, 0.1316886384491766268984945, 0.1181945319615184173123774,
    0.1019301198172404350367501, 0.0832767415767047487247581, 0.0626720483341090635695065, 0.0406014298003869413310400, 0.0176140071391521183118620};
  T xm = 0.5*(a+b);
  T xr = 0.5*(b-a);
  T result = 0;
  for (int j = 0; j < 10; j++) {
    T dx = xr * x[j];
    T evaluationAfterX = integrand(xm+dx, approximationContext);
    if (std::isnan(evaluationAfterX)) {
      return NAN;
    }
    T evaluationBeforeX = integrand(xm-dx, approximationContext);
    if (std::isnan(evaluationBeforeX)) {
      return NAN;
    }
    result += w[j]*(evaluationAfterX + evaluationBeforeX);
  }
  result *= xr;
  return result;
}

#else

template<typename T>
IntegralNode::DetailedResult<T> IntegralNode::kronrodGaussQuadrature(T a, T b, Substitution<T> substitution, const ApproximationContext& approximationContext) const {
  constexpr T epsilon = Float<T>::Epsilon();
  constexpr T max = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  /* We here use Kronrod-Legendre quadrature with n = 21
   * The abscissa and weights are taken from QUADPACK library. */

  // Abscissae for the gauss (odd weights) and kronrod rules (all weights)
  constexpr T x[11]= {0.995657163025808080735527280689003, 0.973906528517171720077964012084452,
    0.930157491355708226001207180059508, 0.865063366688984510732096688423493, 0.780817726586416897063717578345042,
    0.679409568299024406234327365114874, 0.562757134668604683339000099272694, 0.433395394129247190799265943165784,
    0.294392862701460198131126603103866, 0.148874338981631210884826001129720, 0.000000000000000000000000000000000};

  // Weights for the gauss integral
  constexpr T wGauss[5]= {0.066671344308688137593568809893332, 0.149451349150580593145776339657697,
    0.219086362515982043995534934228163, 0.269266719309996355091226921569469, 0.295524224714752870173892994651338};

  // Weights for the kronrod rule
  constexpr T wKronrod[11]= {0.011694638867371874278064396062192, 0.032558162307964727478818972459390,
    0.054755896574351996031381300244580, 0.075039674810919952767043140916190, 0.093125454583697605535065465083366,
    0.109387158802297641899210590325805, 0.123491976262065851077958109831074, 0.134709217311473325928054001771707,
    0.142775938577060080797094273138717, 0.147739104901338491374841515972068, 0.149445554002916905664936468389821};

  T fv1[10];
  T fv2[10];

  T center = static_cast<T>(0.5) * (a+b);
  T halfLength = static_cast<T>(0.5) * (b-a);
  T absHalfLength = std::fabs(halfLength);

  DetailedResult<T> errorResult;
  errorResult.integral = NAN;
  errorResult.absoluteError = 0;

  T gaussIntegral = 0;
  T fCenter = integrand(center ,substitution, approximationContext);
  if (std::isnan(fCenter)) {
    return errorResult;
  }
  T kronrodIntegral = wKronrod[10] * fCenter;
  T absKronrodIntegral = std::fabs(kronrodIntegral);
  for (int j = 0; j < 10; j++) {
    T xDelta = halfLength * x[j];
    T fval1 = integrand(center - xDelta, substitution, approximationContext);
    if (std::isnan(fval1)) {
      return errorResult;
    }
    T fval2 = integrand(center + xDelta, substitution, approximationContext);
    if (std::isnan(fval2)) {
      return errorResult;
    }
    fv1[j] = fval1;
    fv2[j] = fval2;
    T fsum = fval1 + fval2;
    if (j % 2 == 1) {
      gaussIntegral += wGauss[j/2] * fsum;
    }
    kronrodIntegral += wKronrod[j] * fsum;
    absKronrodIntegral += wKronrod[j] * (std::fabs(fval1) + std::fabs(fval2));
  }

  T halfKronrodIntegral = static_cast<T>(0.5) * kronrodIntegral;
  T kronrodIntegralDifference = wKronrod[10] * std::fabs(fCenter - halfKronrodIntegral);
  for (int j = 0; j < 10; j++) {
    kronrodIntegralDifference += wKronrod[j] * (std::fabs(fv1[j] - halfKronrodIntegral) + std::fabs(fv2[j] - halfKronrodIntegral));
  }
  T integral = kronrodIntegral * halfLength;
  absKronrodIntegral = absKronrodIntegral * absHalfLength;
  kronrodIntegralDifference = kronrodIntegralDifference * absHalfLength;
  T absError = std::fabs((kronrodIntegral - gaussIntegral) * halfLength);
  if (kronrodIntegralDifference != 0 && absError != 0) {
    T errorCoefficient = std::pow(static_cast<T>(200*absError/kronrodIntegralDifference), static_cast<T>(1.5));
    absError = 1 > errorCoefficient ? kronrodIntegralDifference * errorCoefficient : kronrodIntegralDifference;
  }
  if (absKronrodIntegral > max/(static_cast<T>(50.0) * epsilon)) {
    T minError = epsilon * 50 * absKronrodIntegral;
    absError = absError > minError ? absError : minError;
  }
  DetailedResult<T> result;
  result.integral = integral;
  result.absoluteError = absError;
  return result;
}

template<typename T>
IntegralNode::DetailedResult<T> IntegralNode::adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, Substitution<T> substitution, const ApproximationContext& approximationContext) const {
  DetailedResult<T> quadKG = kronrodGaussQuadrature(a, b, substitution, approximationContext);
  if (quadKG.absoluteError <= eps) {
    return quadKG;
  } else if (--numberOfIterations > 0) {
    T m = (a+b)/2;
    DetailedResult<T> left = adaptiveQuadrature<T>(a, m, eps/2, numberOfIterations, substitution, approximationContext);
    DetailedResult<T> right = adaptiveQuadrature<T>(m, b, eps/2, numberOfIterations, substitution, approximationContext);
    DetailedResult<T> result;
    result.integral = left.integral + right.integral;
    result.absoluteError = left.absoluteError + right.absoluteError;
    return result;
  } else {
    return quadKG;
  }
}
#endif

Expression Integral::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
}

void Integral::deepReduceChildren(const ReductionContext& reductionContext) {
  /* First child is reduced with target SystemForAnalysis */
  ReductionContext childContext = reductionContext;
  childContext.setTarget(ReductionTarget::SystemForAnalysis);
  childAtIndex(0).deepReduce(childContext);

  /* Other children are reduced with the same reduction target as the parent */
  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);
  for (int i = 1; i < childrenCount; i++) {
    childAtIndex(i).deepReduce(reductionContext);
  }
}

Expression Integral::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
