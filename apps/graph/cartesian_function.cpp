#include "cartesian_function.h"
#include <float.h>
#include <cmath>

using namespace Poincare;

namespace Graph {

CartesianFunction::CartesianFunction(const char * text, KDColor color) :
  Shared::Function(text, color),
  m_displayDerivative(false)
{
}

bool CartesianFunction::displayDerivative() {
  return m_displayDerivative;
}

void CartesianFunction::setDisplayDerivative(bool display) {
  m_displayDerivative = display;
}

double CartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Complex<double> abscissa = Poincare::Complex<double>::Float(x);
  Poincare::Expression * args[2] = {expression(context), &abscissa};
  Poincare::Derivative derivative(args, true);
  /* TODO: when we will simplify derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return derivative.approximateToScalar<double>(*context);
}

double CartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Poincare::Complex<double> x = Poincare::Complex<double>::Float(start);
  Poincare::Complex<double> y = Poincare::Complex<double>::Float(end);
  Poincare::Expression * args[3] = {expression(context), &x, &y};
  Poincare::Integral integral(args, true);
  /* TODO: when we will simplify integral, we might want to simplify the
   * integral here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return integral.approximateToScalar<double>(*context);
}

CartesianFunction::Point CartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return nextMinimumOfFunction(start, step, max, [](const Function * function, double x, Context * context) {
        return function->evaluateAtAbscissa(x, context);
      }, context);
}

CartesianFunction::Point CartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  Point minimumOfOpposite = nextMinimumOfFunction(start, step, max, [](const Function * function, double x, Context * context) {
        return -function->evaluateAtAbscissa(x, context);
      }, context);
  return {.abscissa = minimumOfOpposite.abscissa, .value = -minimumOfOpposite.value};
}

CartesianFunction::Point CartesianFunction::nextMinimumOfFunction(double start, double step, double max, Evaluation evaluate, Context * context) const {
  double bracket[3];
  Point result = {.abscissa = NAN, .value = NAN};
  double x = start;
  do {
    bracketMinimum(x, step, max, bracket, evaluate, context);
    result = brentMinimum(bracket[0], bracket[2], evaluate, context);
    x = bracket[1];
  } while (std::isnan(result.abscissa) && (step > 0.0 ? x <= max : x >= max));

  if (std::fabs(result.abscissa) < 1E2*k_sqrtEps) {
    result.abscissa = 0;
    result.value = evaluate(this, 0, context);
  }
  return result;
}

void CartesianFunction::bracketMinimum(double start, double step, double max, double result[3], Evaluation evaluate, Context * context) const {
  Point p[3];
  p[0] = {.abscissa = start, .value = evaluate(this, start, context)};
  p[1] = {.abscissa = start+step, .value = evaluate(this, start+step, context)};
  double x = start+2.0*step;
  while (step > 0.0 ? x <= max : x >= max) {
    p[2] = {.abscissa = x, .value = evaluate(this, x, context)};
    if (p[0].value > p[1].value && p[2].value > p[1].value) {
      result[0] = p[0].abscissa;
      result[1] = p[1].abscissa;
      result[2] = p[2].abscissa;
      return;
    }
    if (p[0].value > p[1].value && p[1].value == p[2].value) {
    } else {
      p[0] = p[1];
      p[1] = p[2];
    }
    x += step;
  }
  result[0] = NAN;
  result[1] = NAN;
  result[2] = NAN;
}

char CartesianFunction::symbol() const {
  return 'x';
}

CartesianFunction::Point CartesianFunction::brentMinimum(double ax, double bx, Evaluation evaluate, Context * context) const {
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  if (ax > bx) {
    return brentMinimum(bx, ax, evaluate, context);
  }
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluate(this, x, context);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a))  {
      double middleFax = evaluate(this, (x+a)/2.0, context);
      double middleFbx = evaluate(this, (x+b)/2.0, context);
      double fa = evaluate(this, a, context);
      double fb = evaluate(this, b, context);
      if (middleFax-fa <= k_sqrtEps && fx-middleFax <= k_sqrtEps && fx-middleFbx <= k_sqrtEps && middleFbx-fb <= k_sqrtEps) {
        Point result = {.abscissa = x, .value = fx};
        return result;
      }
    }
    double p = 0;
    double q = 0;
    double r = 0;
    if (std::fabs(e) > tol1) {
      r = (x-w)*(fx-fv);
      q = (x-v)*(fx-fw);
      p = (x-v)*q -(x-w)*r;
      q = 2.0*(q-r);
      if (q>0.0) {
        p = -p;
      } else {
        q = -q;
      }
      r = e;
      e = d;
    }
    if (std::fabs(p) < std::fabs(0.5*q*r) && p<q*(a-x) && p<q*(b-x)) {
      d = p/q;
      u= x+d;
      if (u-a < tol2 || b-u < tol2) {
        d = x < m ? tol1 : -tol1;
      }
    } else {
      e = x<m ? b-x : a-x;
      d = k_goldenRatio*e;
    }
    u = x + (std::fabs(d) >= tol1 ? d : (d>0 ? tol1 : -tol1));
    fu = evaluate(this, u, context);
    if (fu <= fx) {
      if (u<x) {
        b = x;
      } else {
        a = x;
      }
      v = w;
      fv = fw;
      w = x;
      fw = fx;
      x = u;
      fx = fu;
    } else {
      if (u<x) {
        a = u;
      } else {
        b = u;
      }
      if (fu <= fw || w == x) {
        v = w;
        fv = fw;
        w = u;
        fw = fu;
      } else if (fu <= fv || v == x || v == w) {
        v = u;
        fv = fu;
      }
    }
  }
  Point result = {.abscissa = NAN, .value = NAN};
  return result;
}

}
