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
  return nextMinimumOfFunction(start, step, max, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1 = nullptr) {
        return function0->evaluateAtAbscissa(x, context);
      }, context);
}

CartesianFunction::Point CartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  Point minimumOfOpposite = nextMinimumOfFunction(start, step, max, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1 = nullptr) {
        return -function0->evaluateAtAbscissa(x, context);
      }, context);
  return {.abscissa = minimumOfOpposite.abscissa, .value = -minimumOfOpposite.value};
}

double CartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return nextIntersectionWithFunction(start, step, max, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1 = nullptr) {
        return function0->evaluateAtAbscissa(x, context);
      }, context, nullptr);
}

CartesianFunction::Point CartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::Function * function) const {
  double resultAbscissa = nextIntersectionWithFunction(start, step, max, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1) {
        return function0->evaluateAtAbscissa(x, context)-function1->evaluateAtAbscissa(x, context);
      }, context, function);
  CartesianFunction::Point result = {.abscissa = resultAbscissa, .value = evaluateAtAbscissa(resultAbscissa, context)};
  if (std::fabs(result.value) < step*k_precision) {
    result.value = 0.0;
  }
  return result;
}

CartesianFunction::Point CartesianFunction::nextMinimumOfFunction(double start, double step, double max, Evaluation evaluate, Context * context, const Shared::Function * function, bool lookForRootMinimum) const {
  double bracket[3];
  Point result = {.abscissa = NAN, .value = NAN};
  double x = start;
  bool endCondition = false;
  do {
    bracketMinimum(x, step, max, bracket, evaluate, context, function);
    result = brentMinimum(bracket[0], bracket[2], evaluate, context, function);
    x = bracket[1];
    endCondition = std::isnan(result.abscissa) && (step > 0.0 ? x <= max : x >= max);
    if (lookForRootMinimum) {
      endCondition |= std::fabs(result.value) >= k_sqrtEps && (step > 0.0 ? x <= max : x >= max);
    }
  } while (endCondition);

  if (std::fabs(result.abscissa) < step*k_precision) {
    result.abscissa = 0;
    result.value = evaluate(0, context, this, function);
  }
  if (std::fabs(result.value) < step*k_precision) {
    result.value = 0;
  }
  if (lookForRootMinimum) {
    result.abscissa = std::fabs(result.value) >= k_sqrtEps ? NAN : result.abscissa;
  }
  return result;
}

void CartesianFunction::bracketMinimum(double start, double step, double max, double result[3], Evaluation evaluate, Context * context, const Shared::Function * function) const {
  Point p[3];
  p[0] = {.abscissa = start, .value = evaluate(start, context, this, function)};
  p[1] = {.abscissa = start+step, .value = evaluate(start+step, context, this, function)};
  double x = start+2.0*step;
  while (step > 0.0 ? x <= max : x >= max) {
    p[2] = {.abscissa = x, .value = evaluate(x, context, this, function)};
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

CartesianFunction::Point CartesianFunction::brentMinimum(double ax, double bx, Evaluation evaluate, Context * context, const Shared::Function * function) const {
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  if (ax > bx) {
    return brentMinimum(bx, ax, evaluate, context, function);
  }
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluate(x, context, this, function);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a))  {
      double middleFax = evaluate((x+a)/2.0, context, this, function);
      double middleFbx = evaluate((x+b)/2.0, context, this, function);
      double fa = evaluate(a, context, this, function);
      double fb = evaluate(b, context, this, function);
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
    fu = evaluate(u, context, this, function);
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

double CartesianFunction::nextIntersectionWithFunction(double start, double step, double max, Evaluation evaluation, Context * context, const Shared::Function * function) const {
  double bracket[2];
  double result = NAN;
  static double precisionByGradUnit = 1E6;
  double x = start+step;
  do {
    bracketRoot(x, step, max, bracket, evaluation, context, function);
    result = brentRoot(bracket[0], bracket[1], std::fabs(step/precisionByGradUnit), evaluation, context, function);
    x = bracket[1];
  } while (std::isnan(result) && (step > 0.0 ? x <= max : x >= max));

  double extremumMax = std::isnan(result) ? max : result;
  Point resultExtremum[2] = {
    nextMinimumOfFunction(start, step, extremumMax, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1) {
        if (function1) {
          return function0->evaluateAtAbscissa(x, context)-function1->evaluateAtAbscissa(x, context);
        } else {
          return function0->evaluateAtAbscissa(x, context);
        }
      }, context, function, true),
    nextMinimumOfFunction(start, step, extremumMax, [](double x, Context * context, const Shared::Function * function0, const Shared::Function * function1) {
        if (function1) {
          return function1->evaluateAtAbscissa(x, context)-function0->evaluateAtAbscissa(x, context);
        } else {
          return -function0->evaluateAtAbscissa(x, context);
        }
      }, context, function, true)};
  for (int i = 0; i < 2; i++) {
    if (!std::isnan(resultExtremum[i].abscissa) && (std::isnan(result) || std::fabs(result - start) > std::fabs(resultExtremum[i].abscissa - start))) {
      result = resultExtremum[i].abscissa;
    }
  }
  if (std::fabs(result) < step*k_precision) {
    result = 0;
  }
  return result;
}

void CartesianFunction::bracketRoot(double start, double step, double max, double result[2], Evaluation evaluation, Context * context, const Shared::Function * function) const {
  double a = start;
  double b = start+step;
  while (step > 0.0 ? b <= max : b >= max) {
    double fa = evaluation(a, context, this, function);
    double fb = evaluation(b, context, this, function);
    if (fa*fb <= 0) {
      result[0] = a;
      result[1] = b;
      return;
    }
    a = b;
    b = b+step;
  }
  result[0] = NAN;
  result[1] = NAN;
}


double CartesianFunction::brentRoot(double ax, double bx, double precision, Evaluation evaluation, Poincare::Context * context, const Shared::Function * function) const {
  if (ax > bx) {
    return brentRoot(bx, ax, precision, evaluation, context, function);
  }
  double a = ax;
  double b = bx;
  double c = bx;
  double d = b-a;
  double e = b-a;
  double fa = evaluation(a, context, this, function);
  double fb = evaluation(b, context, this, function);
  double fc = fb;
  for (int i = 0; i < 100; i++) {
    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
      c = a;
      fc = fa;
      e = b-a;
      d = b-a;
    }
    if (std::fabs(fc) < std::fabs(fb)) {
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }
    double tol1 = 2.0*DBL_EPSILON*std::fabs(b)+0.5*precision;
    double xm = 0.5*(c-b);
    if (std::fabs(xm) <= tol1 || fb == 0.0) {
      double fbcMiddle = evaluation(0.5*(b+c), context, this, function);
      double isContinuous = (fb <= fbcMiddle && fbcMiddle <= fc) || (fc <= fbcMiddle && fbcMiddle <= fb);
      if (isContinuous) {
        return b;
      }
    }
    if (std::fabs(e) >= tol1 && std::fabs(fa) > std::fabs(b)) {
      double s = fb/fa;
      double p = 2.0*xm*s;
      double q = 1.0-s;
      if (a != c) {
        q = fa/fc;
        double r = fb/fc;
        p = s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
        q = (q-1.0)*(r-1.0)*(s-1.0);
      }
      q = p > 0.0 ? -q : q;
      p = std::fabs(p);
      double min1 = 3.0*xm*q-std::fabs(tol1*q);
      double min2 = std::fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2)) {
        e = d;
        d = p/q;
      } else {
        d = xm;
        e =d;
      }
    } else {
      d = xm;
      e = d;
    }
    a = b;
    fa = fb;
    if (std::fabs(d) > tol1) {
      b += d;
    } else {
      b += xm > 0.0 ? tol1 : tol1;
    }
    fb = evaluation(b, context, this, function);
  }
  return NAN;
}

}
