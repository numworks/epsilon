#include <poincare/integral.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"
#include "layout/integral_layout.h"

namespace Poincare {

Integral::Integral() :
  Function("int", 3)
{
}

Expression::Type Integral::type() const {
  return Type::Integral;
}

Expression * Integral::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Integral * i = new Integral();
  i->setArgument(newOperands, numberOfOperands, cloneOperands);
  return i;
}

Evaluation * Integral::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  VariableContext xContext = VariableContext('x', &context);
  Evaluation * aInput = m_args[1]->evaluate(context, angleUnit);
  float a = aInput->toFloat();
  delete aInput;
  Evaluation * bInput = m_args[2]->evaluate(context, angleUnit);
  float b = bInput->toFloat();
  delete bInput;
  if (isnan(a) || isnan(b)) {
    return new Complex(Complex::Float(NAN));
  }
#ifdef LAGRANGE_METHOD
  float result = lagrangeGaussQuadrature(a, b, xContext, angleUnit);
#else
  float result = adaptiveQuadrature(a, b, 0.1, k_maxNumberOfIterations, xContext, angleUnit);
#endif
  return new Complex(Complex::Float(result));
}

ExpressionLayout * Integral::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = m_args[0]->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = new StringLayout("dx", 2);
  return new IntegralLayout(m_args[1]->createLayout(floatDisplayMode, complexFormat), m_args[2]->createLayout(floatDisplayMode, complexFormat), new HorizontalLayout(childrenLayouts, 2));
}

float Integral::functionValueAtAbscissa(float x, VariableContext xContext, AngleUnit angleUnit) const {
  Complex e = Complex::Float(x);
  Symbol xSymbol = Symbol('x');
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  Evaluation * f = m_args[0]->evaluate(xContext, angleUnit);
  float result = f->toFloat();
  delete f;
  return result;
}

#ifdef LAGRANGE_METHOD

float Integral::lagrangeGaussQuadrature(float a, float b, VariableContext xContext, AngleUnit angleUnit) const {
  /* We here use Gauss-Legendre quadrature with n = 5
   * Gauss-Legendre abscissae and weights taken from
   * http://www.holoborodko.com/pavel/numerical-methods/numerical-integration/*/
  const static float x[10]={0.0765265211334973337546404f, 0.2277858511416450780804962f, 0.3737060887154195606725482f, 0.5108670019508270980043641f,
   0.6360536807265150254528367f, 0.7463319064601507926143051f, 0.8391169718222188233945291f, 0.9122344282513259058677524f,
   0.9639719272779137912676661f, 0.9931285991850949247861224f};
  const static float w[10]={0.1527533871307258506980843f, 0.1491729864726037467878287f, 0.1420961093183820513292983f, 0.1316886384491766268984945f, 0.1181945319615184173123774f,
    0.1019301198172404350367501f, 0.0832767415767047487247581f, 0.0626720483341090635695065f, 0.0406014298003869413310400f, 0.0176140071391521183118620f};
  float xm = 0.5f*(a+b);
  float xr = 0.5f*(b-a);
  float result = 0.0f;
  for (int j = 0; j < 10; j++) {
    float dx = xr * x[j];
    result += w[j]*(functionValueAtAbscissa(xm+dx, xContext, angleUnit) + functionValueAtAbscissa(xm-dx, xContext, angleUnit));
  }
  result *= xr;
  return result;
}

#else

Integral::DetailedResult Integral::kronrodGaussQuadrature(float a, float b, VariableContext xContext, AngleUnit angleUnit) const {
  /* We here use Kronrod-Legendre quadrature with n = 21
   * The abscissa and weights are taken from QUADPACK library. */
  const static float wg[5]= {0.066671344308688137593568809893332f, 0.149451349150580593145776339657697f,
    0.219086362515982043995534934228163f, 0.269266719309996355091226921569469f, 0.295524224714752870173892994651338f};
  const static float xgk[11]= {0.995657163025808080735527280689003f, 0.973906528517171720077964012084452f,
    0.930157491355708226001207180059508f, 0.865063366688984510732096688423493f, 0.780817726586416897063717578345042f,
    0.679409568299024406234327365114874f, 0.562757134668604683339000099272694f, 0.433395394129247190799265943165784f,
    0.294392862701460198131126603103866f, 0.148874338981631210884826001129720f, 0.000000000000000000000000000000000f};
  const static float wgk[11]= {0.011694638867371874278064396062192f, 0.032558162307964727478818972459390f,
    0.054755896574351996031381300244580f, 0.075039674810919952767043140916190f, 0.093125454583697605535065465083366f,
    0.109387158802297641899210590325805f, 0.123491976262065851077958109831074f, 0.134709217311473325928054001771707f,
    0.142775938577060080797094273138717f, 0.147739104901338491374841515972068f, 0.149445554002916905664936468389821f};
  float fv1[10];
  float fv2[10];

  float centr = 0.5f*(a+b);
  float hlgth = 0.5f*(b-a);
  float dhlgth = std::fabs(hlgth);

  float resg =0.0f;
  float fc = functionValueAtAbscissa(centr, xContext, angleUnit);
  float resk = wgk[10]*fc;
  float resabs = std::fabs(resk);
  for (int j = 0; j < 5; j++) {
    int jtw = 2*j+1;
    float absc = hlgth*xgk[jtw];
    float fval1 = functionValueAtAbscissa(centr-absc, xContext, angleUnit);
    float fval2 = functionValueAtAbscissa(centr+absc, xContext, angleUnit);
    fv1[jtw] = fval1;
    fv2[jtw] = fval2;
    float fsum = fval1+fval2;
    resg += wg[j]*fsum;
    resk += wgk[jtw]*fsum;
    resabs += wgk[jtw]*(std::fabs(fval1)+std::fabs(fval2));
  }
  for (int j = 0; j < 5; j++) {
    int jtwm1 = 2*j;
    float absc = hlgth*xgk[jtwm1];
    float fval1 = functionValueAtAbscissa(centr-absc, xContext, angleUnit);
    float fval2 = functionValueAtAbscissa(centr+absc, xContext, angleUnit);
    fv1[jtwm1] = fval1;
    fv2[jtwm1] = fval2;
    float fsum = fval1+fval2;
    resk += wgk[jtwm1]*fsum;
    resabs += wgk[jtwm1]*(std::fabs(fval1)+std::fabs(fval2));
  }
  float reskh = resk*0.5f;
  float resasc = wgk[10]*std::fabs(fc-reskh);
  for (int j = 0; j < 10; j++) {
    resasc += wgk[j]*(std::fabs(fv1[j]-reskh)+std::fabs(fv2[j]-reskh));
  }
  float integral = resk*hlgth;
  resabs = resabs*dhlgth;
  resasc = resasc*dhlgth;
  float abserr = std::fabs((resk-resg)*hlgth);
  if (resasc != 0.0f && abserr != 0.0f) {
    abserr = 1.0f > std::pow(200.0f*abserr/resasc, 1.5f)? resasc*std::pow(200.0f*abserr/resasc, 1.5f) : resasc;
  }
  if (resabs > FLT_MAX/(50.0f*FLT_EPSILON)) {
    abserr = abserr > FLT_EPSILON*50.0f*resabs ? abserr : FLT_EPSILON*50.0f*resabs;
  }
  DetailedResult result;
  result.integral = integral;
  result.absoluteError = abserr;
  return result;
}

float Integral::adaptiveQuadrature(float a, float b, float eps, int numberOfIterations, VariableContext xContext, AngleUnit angleUnit) const {
  DetailedResult quadKG = kronrodGaussQuadrature(a, b, xContext, angleUnit);
  float result = quadKG.integral;
  if (numberOfIterations < k_maxNumberOfIterations && quadKG.absoluteError > eps) {
    float m = (a+b)/2.0f;
    result = adaptiveQuadrature(a, m, eps/2.0f, numberOfIterations+1, xContext, angleUnit) + adaptiveQuadrature(m, b, eps/2.0f, numberOfIterations+1, xContext, angleUnit);
  }
  if (quadKG.absoluteError > eps) {
    return NAN;
  }
  return result;
}
#endif

}
