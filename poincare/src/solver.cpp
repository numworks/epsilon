#include <poincare/solver.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <float.h>
#include <cmath>

namespace Poincare {

Coordinate2D<double> Solver::BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
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
        return Coordinate2D<double>(x, fx);
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
  return Coordinate2D<double>(x, fx);
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
      bool isContinuous = (fb <= fbcMiddle && fbcMiddle <= fc) || (fc <= fbcMiddle && fbcMiddle <= fb);
      if (isContinuous) {
        return b;
      }
    }
    if (std::fabs(e) >= tol1 && std::fabs(fa) > std::fabs(fb)) {
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
      b += xm > 0.0 ? tol1 : -tol1;
    }
    fb = evaluation(b, context, complexFormat, angleUnit, context1, context2, context3);
  }
  return NAN;
}


Coordinate2D<double> Solver::IncreasingFunctionRoot(double ax, double bx, double resultPrecision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3, double * resultEvaluation) {
  assert(ax < bx);
  double min = ax;
  double max = bx;
  double currentAbscissa = min;
  double eval = evaluation(currentAbscissa, context, complexFormat, angleUnit, context1, context2, context3);
  if (eval >= 0) {
    if (resultEvaluation != nullptr) {
      *resultEvaluation = eval;
    }
    // The minimal value is already bigger than 0, return min.
    return Coordinate2D<double>(currentAbscissa, eval);
  }
  while (max - min > resultPrecision) {
    currentAbscissa = (min + max) / 2.0;
    /* If the mean between min and max is the same double (in IEEE754
     * representation) as one of the bounds - min or max, we look for another
     * representable double between min and max strictly. If there is, we choose
     * it instead, otherwise, we reached the most precise result possible. */
    if (currentAbscissa == min) {
      currentAbscissa = IEEE754<double>::next(min);
    }
    if (currentAbscissa == max) {
      currentAbscissa = IEEE754<double>::previous(max);
    }
    if (currentAbscissa == min || currentAbscissa == max) {
      break;
    }
    eval = evaluation(currentAbscissa, context, complexFormat, angleUnit, context1, context2, context3);
    if (eval > DBL_EPSILON) {
      max = currentAbscissa;
    } else if (eval < -DBL_EPSILON) {
      min = currentAbscissa;
    } else {
      break;
    }
  }
  if (resultEvaluation != nullptr) {
    *resultEvaluation = eval;
  }
  return Coordinate2D<double>(currentAbscissa, eval);
}

template<typename T>
T Solver::CumulativeDistributiveInverseForNDefinedFunction(T * probability, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
  T precision = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  assert(*probability <= (((T)1.0) - precision) && *probability >= precision);
  (void) precision;
  T p = 0.0;
  int k = 0;
  T delta = 0.0;
  do {
    delta = std::fabs(*probability-p);
    p += evaluation(k++, context, complexFormat, angleUnit, context1, context2, context3);
    if (p >= k_maxProbability && std::fabs(*probability-(T)1.0) <= delta) {
      *probability = (T)1.0;
      return (T)(k-1);
    }
  } while (std::fabs(*probability-p) <= delta && k < k_maxNumberOfOperations && p < (T)1.0);
  p -= evaluation(--k, context, complexFormat, angleUnit, context1, context2, context3);
  if (k == k_maxNumberOfOperations) {
    *probability = (T)1.0;
    return INFINITY;
  }
  *probability = p;
  if (std::isnan(p)) {
    return NAN;
  }
  return k-1;
}

template<typename T>
T Solver::CumulativeDistributiveFunctionForNDefinedFunction(T x, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
  int end = std::round(x);
  double result = 0.0;
  for (int k = 0; k <=end; k++) {
    result += evaluation(k, context, complexFormat, angleUnit, context1, context2, context3);
    /* Avoid too long loop */
    if (k > k_maxNumberOfOperations) {
      break;
    }
    if (result >= k_maxProbability) {
      result = 1.0;
      break;
    }
  }
  return result;
}

template float Solver::CumulativeDistributiveInverseForNDefinedFunction(float *, ValueAtAbscissa, Context *, Preferences::ComplexFormat, Preferences::AngleUnit, const void *, const void *, const void *);
template double Solver::CumulativeDistributiveInverseForNDefinedFunction(double *, ValueAtAbscissa, Context *, Preferences::ComplexFormat, Preferences::AngleUnit, const void *, const void *, const void *);
template float Solver::CumulativeDistributiveFunctionForNDefinedFunction(float, ValueAtAbscissa, Context *, Preferences::ComplexFormat, Preferences::AngleUnit, const void *, const void *, const void *);
template double Solver::CumulativeDistributiveFunctionForNDefinedFunction(double, ValueAtAbscissa, Context *, Preferences::ComplexFormat, Preferences::AngleUnit, const void *, const void *, const void *);

}
