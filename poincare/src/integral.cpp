#include <poincare/integral.h>
#include <poincare/complex.h>
#include <poincare/integral_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
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
  return Integral(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> IntegralNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aInput = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> bInput = childAtIndex(3)->approximate(T(), context, complexFormat, angleUnit);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  if (std::isnan(a) || std::isnan(b)) {
    return Complex<T>::RealUndefined();
  }
#ifdef LAGRANGE_METHOD
  T result = lagrangeGaussQuadrature<T>(a, b, context, complexFormat, angleUnit);
#else
  T result = adaptiveQuadrature<T>(a, b, 0.1, k_maxNumberOfIterations, context, complexFormat, angleUnit);
#endif
  return Complex<T>::Builder(result);
}

template<typename T>
T IntegralNode::functionValueAtAbscissa(T x, Context * xcontext, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  // Here we cannot use Expression::approximateWithValueForSymbol which would reset the sApproximationEncounteredComplex flag
  assert(childAtIndex(1)->type() == Type::Symbol);
  VariableContext variableContext = VariableContext(static_cast<SymbolNode *>(childAtIndex(1))->name(), xcontext);
  variableContext.setApproximationForVariable<T>(x);
  return childAtIndex(0)->approximate(T(), &variableContext, complexFormat, angleUnit).toScalar();
}

#ifdef LAGRANGE_METHOD

template<typename T>
T IntegralNode::lagrangeGaussQuadrature(T a, T b, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* We here use Gauss-Legendre quadrature with n = 5
   * Gauss-Legendre abscissae and weights can be found in
   * C/C++ library source code. */
  const static T x[10]={0.0765265211334973337546404, 0.2277858511416450780804962, 0.3737060887154195606725482, 0.5108670019508270980043641,
   0.6360536807265150254528367, 0.7463319064601507926143051, 0.8391169718222188233945291, 0.9122344282513259058677524,
   0.9639719272779137912676661, 0.9931285991850949247861224};
  const static T w[10]={0.1527533871307258506980843, 0.1491729864726037467878287, 0.1420961093183820513292983, 0.1316886384491766268984945, 0.1181945319615184173123774,
    0.1019301198172404350367501, 0.0832767415767047487247581, 0.0626720483341090635695065, 0.0406014298003869413310400, 0.0176140071391521183118620};
  T xm = 0.5*(a+b);
  T xr = 0.5*(b-a);
  T result = 0;
  for (int j = 0; j < 10; j++) {
    T dx = xr * x[j];
    T evaluationAfterX = functionValueAtAbscissa(xm+dx, context, complexFormat, angleUnit);
    if (std::isnan(evaluationAfterX)) {
      return NAN;
    }
    T evaluationBeforeX = functionValueAtAbscissa(xm-dx, context, complexFormat, angleUnit);
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
IntegralNode::DetailedResult<T> IntegralNode::kronrodGaussQuadrature(T a, T b, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  static T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  static T max = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  /* We here use Kronrod-Legendre quadrature with n = 21
   * The abscissa and weights are taken from QUADPACK library. */

  // Abscissae for the gauss (odd weights) and kronrod rules (all weights)
  const static T x[11]= {0.995657163025808080735527280689003, 0.973906528517171720077964012084452,
    0.930157491355708226001207180059508, 0.865063366688984510732096688423493, 0.780817726586416897063717578345042,
    0.679409568299024406234327365114874, 0.562757134668604683339000099272694, 0.433395394129247190799265943165784,
    0.294392862701460198131126603103866, 0.148874338981631210884826001129720, 0.000000000000000000000000000000000};

  // Weights for the gauss integral
  const static T wGauss[5]= {0.066671344308688137593568809893332, 0.149451349150580593145776339657697,
    0.219086362515982043995534934228163, 0.269266719309996355091226921569469, 0.295524224714752870173892994651338};

  // Weights for the kronrod rule
  const static T wKronrod[11]= {0.011694638867371874278064396062192, 0.032558162307964727478818972459390,
    0.054755896574351996031381300244580, 0.075039674810919952767043140916190, 0.093125454583697605535065465083366,
    0.109387158802297641899210590325805, 0.123491976262065851077958109831074, 0.134709217311473325928054001771707,
    0.142775938577060080797094273138717, 0.147739104901338491374841515972068, 0.149445554002916905664936468389821};

  T fv1[10];
  T fv2[10];

  T center = (T)0.5 * (a+b);
  T halfLength = (T)0.5 * (b-a);
  T absHalfLength = std::fabs(halfLength);

  DetailedResult<T> errorResult;
  errorResult.integral = NAN;
  errorResult.absoluteError = 0;

  T gaussIntegral = 0;
  T fCenter = functionValueAtAbscissa(center, context, complexFormat, angleUnit);
  if (std::isnan(fCenter)) {
    return errorResult;
  }
  T kronrodIntegral = wKronrod[10] * fCenter;
  T absKronrodIntegral = std::fabs(kronrodIntegral);
  for (int j = 0; j < 10; j++) {
    T xDelta = halfLength * x[j];
    T fval1 = functionValueAtAbscissa(center - xDelta, context, complexFormat, angleUnit);
    if (std::isnan(fval1)) {
      return errorResult;
    }
    T fval2 = functionValueAtAbscissa(center + xDelta, context, complexFormat, angleUnit);
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

  T halfKronrodIntegral = (T)0.5 * kronrodIntegral;
  T kronrodIntegralDifference = wKronrod[10] * std::fabs(fCenter - halfKronrodIntegral);
  for (int j = 0; j < 10; j++) {
    kronrodIntegralDifference += wKronrod[j] * (std::fabs(fv1[j] - halfKronrodIntegral) + std::fabs(fv2[j] - halfKronrodIntegral));
  }
  T integral = kronrodIntegral * halfLength;
  absKronrodIntegral = absKronrodIntegral * absHalfLength;
  kronrodIntegralDifference = kronrodIntegralDifference * absHalfLength;
  T absError = std::fabs((kronrodIntegral - gaussIntegral) * halfLength);
  if (kronrodIntegralDifference != 0 && absError != 0) {
    T errorCoefficient = std::pow((T)(200*absError/kronrodIntegralDifference), (T)1.5);
    absError = 1 > errorCoefficient ? kronrodIntegralDifference * errorCoefficient : kronrodIntegralDifference;
  }
  if (absKronrodIntegral > max/((T)50.0 * epsilon)) {
    T minError = epsilon * 50 * absKronrodIntegral;
    absError = absError > minError ? absError : minError;
  }
  DetailedResult<T> result;
  result.integral = integral;
  result.absoluteError = absError;
  return result;
}

template<typename T>
T IntegralNode::adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (Expression::ShouldStopProcessing()) {
    return NAN;
  }
  DetailedResult<T> quadKG = kronrodGaussQuadrature(a, b, context, complexFormat, angleUnit);
  T result = quadKG.integral;
  if (quadKG.absoluteError <= eps) {
    return result;
  } else if (--numberOfIterations > 0) {
    T m = (a+b)/2;
    return adaptiveQuadrature<T>(a, m, eps/2, numberOfIterations, context, complexFormat, angleUnit) + adaptiveQuadrature<T>(m, b, eps/2, numberOfIterations, context, complexFormat, angleUnit);
  } else {
    return NAN;
  }
}
#endif

Expression Integral::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::Matrix);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
}

Expression Integral::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(!childAtIndex(1).deepIsMatrix(context));
  if (childAtIndex(0).deepIsMatrix(context)
      || childAtIndex(2).deepIsMatrix(context)
      || childAtIndex(3).deepIsMatrix(context))
  {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

}
