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

CartesianFunction::Point CartesianFunction::mininimumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Point p = brentAlgorithm(start, end, context);
  if (evaluateAtAbscissa(p.abscissa-k_sqrtEps, context) < p.value || evaluateAtAbscissa(p.abscissa+k_sqrtEps, context) < p.value || std::isnan(p.value)) {
    p.abscissa = NAN;
    p.value = NAN;
  }
  return p;
}

char CartesianFunction::symbol() const {
  return 'x';
}

CartesianFunction::Point CartesianFunction::brentAlgorithm(double ax, double bx, Context * context) const {
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluateAtAbscissa(x, context);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a)) {
      Point result = {.abscissa = x, .value = fx};
      return result;
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
    fu = evaluateAtAbscissa(u, context);
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
