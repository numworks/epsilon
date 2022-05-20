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

template <typename T> constexpr T sqrt_epsilon;
template<> constexpr float sqrt_epsilon<float>  = 3.4e-4;
template<> constexpr double sqrt_epsilon<double> = 1.5e-8;

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

Layout IntegralNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return IntegralLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int IntegralNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Integral::s_functionHelper.name());
}

Expression IntegralNode::shallowReduce(ReductionContext reductionContext) {
  return Integral(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> IntegralNode::templatedApproximate(ApproximationContext approximationContext) const {
  /* TODO : Reduction is mapped on list, but not approximation.
  * Find a smart way of doing it. */
  Evaluation<T> aInput = childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(3)->approximate(T(), approximationContext);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  m_a = a;
  m_b = b;
  if (std::isnan(a) || std::isnan(b)) {
    return Complex<T>::RealUndefined();
  }
  ExpressionNode::ReductionContext context(approximationContext.context(), approximationContext.complexFormat(), Preferences::AngleUnit::Radian, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::SystemForAnalysis);
  bool fIsNanInA = std::isnan(firstChildScalarValueForArgument(a, approximationContext));
  bool fIsNanInB = std::isnan(firstChildScalarValueForArgument(b, approximationContext));
  /* Rewrite the integrand to be able to compute it directly at abscissa a +
     delta to avoid precision loss when we are really close from a */
  if (fIsNanInA && a != 0) {
    Expression formula = Expression(childAtIndex(0)).clone();
    Symbol symbol = Expression(childAtIndex(1)).clone().convert<Symbol>();
    Expression bound = Expression(childAtIndex(2)).clone();
    Expression expr = Addition::Builder(bound, symbol);
    formula.replaceSymbolWithExpression(symbol, expr);
    m_expr_a = formula.deepReduce(context);
  }
  // Same near b - delta
  if (fIsNanInB && b != 0) {
    Expression formula = Expression(childAtIndex(0)).clone();
    Symbol symbol = Expression(childAtIndex(1)).clone().convert<Symbol>();
    Expression bound = Expression(childAtIndex(3)).clone();
    Expression expr = Addition::Builder(bound, Multiplication::Builder(Rational::Builder(-1), symbol));
    formula.replaceSymbolWithExpression(symbol, expr);
    m_expr_b = formula.deepReduce(context);
  }

  // The integrand has a singularity on a bound of the interval, use tanh-sinh quadrature
  if (fIsNanInA || fIsNanInB) {
    /* We are using 4 levels of refinement which means ≈ 64 integrand evaluations
     * = 2 (R- and R+) * 2^4 (ticks/unit) * 4 (typical decay on the examples)
     * It could be increased but precision is likely lost somewhere else in
     * hard examples. */
    DetailedResult<T> detailedResult = tanhSinhQuadrature<T>(4, approximationContext);
    m_expr_a = Expression();
    m_expr_b = Expression();
    // Arbitrary value to have the best choice of quadrature on the examples
    if (!std::isnan(detailedResult.integral) && detailedResult.absoluteError < 0.001) {
      return Complex<T>::Builder(detailedResult.integral);
    }
  }
  /* Choose the right substitution to use in Gauss-Konrod.
   * We are using a (0,inf) -> (-1,1) like substitution when a bound is above
   * these thresholds to deal with big but finite bounds */
  T leftOpenThreshold = -1000;
  T rightOpenThreshold = 1000;
  T start, end, scale = 1.0;
  if (a < leftOpenThreshold) {
    if (b > rightOpenThreshold) {
      m_substitution = Substitution::RealLine;
      // Two solutions but we need the one with the sign of a (resp. b)
      start = std::isfinite(a) ? ((std::sqrt(4*a*a+1)-1)/(2*a)) : -1;
      end = std::isfinite(b) ? ((std::sqrt(4*b*b+1)-1)/(2*b)) : 1;
    } else {
      m_substitution = Substitution::LeftOpen;
      start = std::isfinite(a) ? (a-b+1)/(b-a+1) : -1;
      end = 1;
      scale = 2.0;
    }
  } else {
    if (b > rightOpenThreshold) {
      m_substitution = Substitution::RightOpen;
      start = std::isfinite(b) ? (a-b+1)/(b-a+1) : -1;
      end = 1;
      scale = 2.0;
    } else {
      m_substitution = Substitution::None;
      start = a;
      end = b;
    }
  }
  /* The tolerance sqrt(eps) estimated by the method is an upper bound and the
   * real is error is typically eps */
  DetailedResult<T> detailedResult = adaptiveQuadrature<T>(start, end, sqrt_epsilon<T>, k_maxNumberOfIterations, approximationContext);
  // Arbitrary value: precision at which we decide to ignore the result
  T result = detailedResult.absoluteError > 0.1 ? NAN : scale * detailedResult.integral;
  return Complex<T>::Builder(result);
}

template<typename T>
T IntegralNode::integrand(T x, ApproximationContext approximationContext) const {
  switch (m_substitution) {
  case Substitution::None:
    return firstChildScalarValueForArgument(x, approximationContext);
  case Substitution::LeftOpen:
  {
    T z = 1 / (x + 1);
    T arg = m_b - (2 * z - 1);
    return firstChildScalarValueForArgument(arg, approximationContext) * z * z;
  }
  case Substitution::RightOpen:
  {
    T z = 1 / (x + 1);
    T arg = 2 * z + m_a - 1;
    return firstChildScalarValueForArgument(arg, approximationContext) * z * z;
  }
  case Substitution::RealLine:
  {
    T x2 = x * x;
    T inv = 1 / (1 - x2);
    T w = (1 + x2) * inv * inv;
    T arg = x * inv;
    return firstChildScalarValueForArgument(arg, approximationContext) * w;
  }
  }
}

template<typename T>
T IntegralNode::integrandNearBound(T x, T xc, ApproximationContext approximationContext) const {
  T scale = (m_b - m_a) / 2;
  T arg = xc * scale;
  if (x < 0) {
    if (!m_expr_a.isUninitialized()) {
      return approximateExpressionWithArgument(m_expr_a.node(), arg, approximationContext).toScalar() * scale;
    }
    arg = arg + m_a;
  } else {
    if (!m_expr_b.isUninitialized()) {
      return approximateExpressionWithArgument(m_expr_b.node(), arg, approximationContext).toScalar() * scale;
    }
    arg = m_b - arg;
  }
  return firstChildScalarValueForArgument(arg, approximationContext) * scale;
}

constexpr double halfPi = M_PI_2;
template <typename T> constexpr T ignoreTailThreshold;
template<> constexpr float ignoreTailThreshold<float>  = 1e-15;
template<> constexpr double ignoreTailThreshold<double> = 1e-15;


/* Tanh-Sinh quadrature
 * cf https://www.davidhbailey.com/dhbpapers/dhb-tanh-sinh.pdf */
template<typename T>
IntegralNode::DetailedResult<T> IntegralNode::tanhSinhQuadrature(int level, ApproximationContext approximationContext) const {
  T h = 2;
  T result = halfPi * integrandNearBound(0.0, 1.0, approximationContext); // j=0
  int j=1;
  T sn2, sn1 = 0;
  T maxWjFj = 0;
  for (int k=0; k<level; k++) {
    sn2 = sn1;
    sn1 = result * h;
    h = h / 2.0;
    j = 1;
    bool leftOk = true;
    bool rightOk = true;
    while (leftOk || rightOk) {
      T sinh = halfPi * std::sinh(h * j);
      T cs = std::cosh(sinh);
      T weight = halfPi * std::cosh(h * j) / (cs * cs);
      T abscissa = std::tanh(sinh);
      T distanceToBound = 1 / (std::exp(sinh) * std::cosh(sinh));
      if (leftOk) {
        T leftValue = integrandNearBound(-abscissa, distanceToBound, approximationContext);
        if (std::isnan(leftValue)) {
          leftOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * leftValue));
          result += weight * leftValue;
        }
        // criterion used in boost: abs(y * weights) > abs(L1_I0 * tail_tolerance) but
        // L1_IO is abs(pi/2 * f(0)) before the first row and tail_tolerance = tolerance^2
        if (std::abs(weight * leftValue) < ignoreTailThreshold<T>) leftOk = false;
      }
      if (rightOk) {
        T rightValue = integrandNearBound(abscissa, distanceToBound, approximationContext);
        if (std::isnan(rightValue)) {
          rightOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * rightValue));
          result += weight * rightValue;
        }
        if (std::abs(weight * rightValue) < ignoreTailThreshold<T>) rightOk = false;
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
    error = 0;
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
T IntegralNode::lagrangeGaussQuadrature(T a, T b, ApproximationContext approximationContext) const {
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
IntegralNode::DetailedResult<T> IntegralNode::kronrodGaussQuadrature(T a, T b, ApproximationContext approximationContext) const {
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
  T fCenter = integrand(center, approximationContext);
  if (std::isnan(fCenter)) {
    return errorResult;
  }
  T kronrodIntegral = wKronrod[10] * fCenter;
  T absKronrodIntegral = std::fabs(kronrodIntegral);
  for (int j = 0; j < 10; j++) {
    T xDelta = halfLength * x[j];
    T fval1 = integrand(center - xDelta, approximationContext);
    if (std::isnan(fval1)) {
      return errorResult;
    }
    T fval2 = integrand(center + xDelta, approximationContext);
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
IntegralNode::DetailedResult<T> IntegralNode::adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, ApproximationContext approximationContext) const {
  DetailedResult<T> quadKG = kronrodGaussQuadrature(a, b, approximationContext);
  if (quadKG.absoluteError <= eps) {
    return quadKG;
  } else if (--numberOfIterations > 0) {
    T m = (a+b)/2;
    DetailedResult<T> left = adaptiveQuadrature<T>(a, m, eps/2, numberOfIterations, approximationContext);
    DetailedResult<T> right = adaptiveQuadrature<T>(m, b, eps/2, numberOfIterations, approximationContext);
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

Expression Integral::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
