#include <poincare/solver.h>
#include <assert.h>
#include <float.h>
#include <cmath>

namespace Poincare {

Coordinate2D Solver::BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  if (ax > bx) {
    return BrentMinimum(bx, ax, evaluation, context, complexFormat, angleUnit, context1, context2, context3);
  }
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluation(x, context, complexFormat, angleUnit, context1, context2, context3);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a))  {
      double middleFax = evaluation((x+a)/2.0, context, complexFormat, angleUnit, context1, context2, context3);
      double middleFbx = evaluation((x+b)/2.0, context, complexFormat, angleUnit, context1, context2, context3);
      double fa = evaluation(a, context, complexFormat, angleUnit, context1, context2, context3);
      double fb = evaluation(b, context, complexFormat, angleUnit, context1, context2, context3);
      if (middleFax-fa <= k_sqrtEps && fx-middleFax <= k_sqrtEps && fx-middleFbx <= k_sqrtEps && middleFbx-fb <= k_sqrtEps) {
        return Coordinate2D(x, fx);
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
    fu = evaluation(u, context, complexFormat, angleUnit, context1, context2, context3);
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
  return Coordinate2D(x, fx);
}

double Solver::BrentRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
  if (ax > bx) {
    return BrentRoot(bx, ax, precision, evaluation, context, complexFormat, angleUnit, context1, context2, context3);
  }
  double a = ax;
  double b = bx;
  double c = bx;
  double d = b-a;
  double e = b-a;
  double fa = evaluation(a, context, complexFormat, angleUnit, context1, context2, context3);
  if (fa == 0) {
    // We are looking for a root. If a is already a root, just return it.
    return a;
  }
  double fb = evaluation(b, context, complexFormat, angleUnit, context1, context2, context3);
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
      double fbcMiddle = evaluation(0.5*(b+c), context, complexFormat, angleUnit, context1, context2, context3);
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
    fb = evaluation(b, context, complexFormat, angleUnit, context1, context2, context3);
  }
  return NAN;
}

Coordinate2D Solver::IncreasingFunctionRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
  assert(ax < bx);
  double min = ax;
  double max = bx;
  double currentAbscissa = min;
  double eval = evaluation(currentAbscissa, context, complexFormat, angleUnit, context1, context2, context3);
  if (eval >= 0) {
    if (eval <= DBL_EPSILON) {
      // The value on the left bracket is 0, return it.
      return Coordinate2D(currentAbscissa, eval);
    }
    // The minimal value is already bigger than 0, return NAN.
    return Coordinate2D(currentAbscissa, NAN);
  }
  while (max - min > precision) {
    currentAbscissa = (min + max) / 2.0;
    eval = evaluation(currentAbscissa, context, complexFormat, angleUnit, context1, context2, context3);
    if (eval > DBL_EPSILON) {
      max = currentAbscissa;
    } else if (eval < -DBL_EPSILON) {
      min = currentAbscissa;
    } else {
      return Coordinate2D(currentAbscissa, eval);
    }
  }
  return Coordinate2D(currentAbscissa, std::fabs(eval) < precision ? eval : NAN);
}

}
