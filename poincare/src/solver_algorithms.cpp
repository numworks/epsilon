#include <poincare/solver_algorithms.h>

namespace Poincare {

Coordinate2D<double> SolverAlgorithms::BrentRoot(Solver2<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver2<double>::Interest interest, double precision) {
  constexpr int numberOfIterations = 100;

  if (xMax < xMin) {
    return BrentRoot(f, aux, xMax, xMin, interest, precision);
  }
  assert(interest == Solver2<double>::Interest::Root);
  assert(xMin < xMax);
  assert((f(xMin, aux) <= 0. && 0. <= f(xMax, aux)) || (f(xMax, aux) <= 0. && 0. <= f(xMin, aux)));

  double a = xMin;
  double b = xMax;
  double c = xMax;
  double d = b - a;
  double e = b - a;
  double fa = f(a, aux);
  if (fa == 0.) {
    return Coordinate2D<double>(a, fa);
  }
  double fb = f(b, aux);
  double fc = fb;

  for (int i = 0; i < numberOfIterations; i++) {
    if ((fb > 0. && fc > 0.) || (fb < 0. && fc < 0.)) {
      c = a;
      fc = fa;
      d = e = b - a;
    }
    if (std::fabs(fc) < std::fabs(fb)) {
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }

    double tol1 = 2. * DBL_EPSILON * std::fabs(b) + 0.5 * precision;
    double xm = 0.5 * (c - b);
    if (std::fabs(xm) <= tol1 || fb == 0.0) {
      double fbcMiddle = f(0.5 * (b + c), aux);
      if ((fb <= fbcMiddle && fbcMiddle <= fc) || (fc <= fbcMiddle && fbcMiddle <= fb)) {
        return Coordinate2D<double>(b, fb);
      }
    }

    if (std::fabs(e) >= tol1 && std::fabs(fa) > std::fabs(fb)) {
      double s = fb / fa;
      double p = 2. * xm * s;
      double q  = 1. - s;
      if (a != c) {
        q = fa / fc;
        double r = fb / fc;
        p = s * (2. * xm * q * (q - r) - (b - a) * (r - 1.));
        q = (q - 1.) * (r - 1.) * (s - 1.);
      }
      q = p > 0. ? -q : q;
      p = std::fabs(p);
      double min1 = 3. * xm * q - std::fabs(tol1 * q);
      double min2 = std::fabs(e * q);
      if (2. * p < (min1 < min2 ? min1 : min2)) {
        e = d;
        d = p / q;
      } else {
        d = e = xm;
      }
    } else {
      d = e = xm;
    }

    a = b;
    fa = fb;
    if (std::fabs(d) > tol1) {
      b += d;
    } else {
      b += xm > 0. ? tol1 : -tol1;
    }
    fb = f(b, aux);
  }

  return Coordinate2D<double>(NAN, NAN);
}

}
