#include <poincare/float.h>
#include <poincare/solver.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <algorithm>

namespace Poincare {

template<typename T>
Coordinate2D<T> SolverHelper<T>::NextPointOfInterest(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, T start, T end, T relativePrecision, T minimalStep, T maximalStep) {
  assert(relativePrecision > static_cast<T>(0.f) && minimalStep >= static_cast<T>(0.f) && maximalStep >= minimalStep);

  constexpr T overflow = Float<T>::Max();
  constexpr T underflow = Float<T>::Min();

  if (start == static_cast<T>(INFINITY)) {
    start = overflow;
  } else if (start == static_cast<T>(-INFINITY)) {
    start = -overflow;
  }
  if (end == static_cast<T>(INFINITY)) {
    end = overflow;
  } else if (end == static_cast<T>(-INFINITY)) {
    end = -overflow;
  }
  if (minimalStep == 0) {
    minimalStep = underflow;
  }

  if (start * end >= static_cast<T>(0.f)) {
    return NextPointOfInterestHelper(evaluation, context, auxiliary, search, start, end, relativePrecision, minimalStep, maximalStep);
  }

  /* By design, NextPointOfInterestHelper only works on intervals that do not
   * contain 0. If start and end are of different signs, we have to cut the
   * interval in three: negative, around 0, and positive. */

  assert(start * end < static_cast<T>(0.f));
  Coordinate2D<T> result = NextPointOfInterestHelper(evaluation, context, auxiliary, search, start, static_cast<T>(0.f), relativePrecision, minimalStep, maximalStep);
  if (std::isfinite(result.x1())) {
    /* Althoug this method can return NaN when there is no solution, here we
     * only return if a solution was found, as there are two other intervals to
     * check otherwise. */
    return result;
  }
  constexpr T marginAroundZero = static_cast<T>(1e-3);
  /* We deviate slightly from zero, as many common functions with an
   * interesting behaviour around zero are undefined for x=0. */
  T fakeZero = (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON) * static_cast<T>(start < end ? 1.f : -1.f);
  result = search(-marginAroundZero, fakeZero, marginAroundZero, evaluation(-marginAroundZero, context, auxiliary), evaluation(fakeZero, context, auxiliary), evaluation(marginAroundZero, context, auxiliary), evaluation, context, auxiliary);
  if (std::isfinite(result.x1())) {
    return result;
  }
  return NextPointOfInterestHelper(evaluation, context, auxiliary, search, static_cast<T>(0.f), end, relativePrecision, minimalStep, maximalStep);
}

template<typename T>
Coordinate2D<T> SolverHelper<T>::NextPointOfInterestHelper(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, T start, T end, T relativePrecision, T minimalStep, T maximalStep) {
  assert(start * end >= static_cast<T>(0.f));
  assert(relativePrecision > static_cast<T>(0.f));

  T h = std::fabs(start) < std::fabs(end) ? relativePrecision : -relativePrecision;
  T m, M;
  if (start < end) {
    m = minimalStep;
    M = maximalStep;
  } else {
    m = -minimalStep;
    M = -maximalStep;
  }

  T x1 = start, x2 = Step(start, h, m, M);
  T x3 = Step(x2, h, m, M);
  T y1 = evaluation(x1, context, auxiliary), y2 = evaluation(x2, context, auxiliary), y3 = evaluation(x3, context, auxiliary);

  Coordinate2D<T> result(NAN, NAN);

  while (!std::isfinite(result.x1()) && std::isfinite(x1) && (x1 < end) == (start < end)) {
    result = search(x1, x2, x3, y1, y2, y3, evaluation, context, auxiliary);

    x1 = x2;
    y1 = y2;
    x2 = x3;
    y2 = y3;
    x3 = Step(x2, h, m, M);
    y3 = evaluation(x3, context, auxiliary);
  }

  return result;
}

template<typename T>
T SolverHelper<T>::Step(T x, T growthSpeed, T minimalStep, T maximalStep) {
  assert(std::fabs(minimalStep) <= std::fabs(maximalStep));
  T acceleration = std::fmax(
      std::fabs(std::log10(std::fabs(x)) - static_cast<T>(1.f)) - static_cast<T>(2.f),
      static_cast<T>(0.f));
  /* For growthSpeed < 0 and x large enough, step < -x. The new value of x
   * would change sign, which is prohibited. We thus bound the step so that x
   * cannot lose more than one order of magnitude. */
  constexpr T maxLossOfMagnitude = static_cast<T>(-0.9f);
  T step = x * std::fmax(growthSpeed * (static_cast<T>(1.f) + acceleration), maxLossOfMagnitude);
  if (!std::isfinite(step) || std::fabs(minimalStep) > std::fabs(step)) {
    step = minimalStep;
  } else if (std::fabs(maximalStep) < std::fabs(step)) {
    step = maximalStep;
  }
  T res = x + step;
  assert(minimalStep > 0 ? res > x : res < x);
  return res;
}

template<typename T>
bool SolverHelper<T>::RootExistsOnInterval(T fa, T fb, T fc) {
  return ((fb == static_cast<T>(0.f) && fa * fc < static_cast<T>(0.f))
       || fb * fc < static_cast<T>(0.f));
}

double Solver::NextRoot(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double start, double end, double relativePrecision, double minimalStep, double maximalStep) {
  SolverHelper<double>::BracketSearch search = [](double a, double b, double c, double fa, double fb, double fc, SolverHelper<double>::ValueAtAbscissa f, Context * context, const void * auxiliary) {
    Coordinate2D<double> result(NAN, NAN);
    if (SolverHelper<double>::RootExistsOnInterval(fa, fb, fc)) {
      /* a might be a root and a being a root is not supposed to trigger
       * RootExistsOnInterval. If we're here, this means there is a second root
       * between b and c, and the Brent algorithm could pick up either.
       * To avoid inifnite loop, we only focus on the second range [b, c]. */
      result = Coordinate2D<double>(BrentRoot(b, c, std::fabs(c-b)/k_precisionByGradUnit, f, context, auxiliary), 0.);
    } else if (SolverHelper<double>::MinimumExistsOnInterval(fa, fb, fc) && fb >= 0) {
      result = BrentMinimum(a, c, f, context, auxiliary);
    } else if (SolverHelper<double>::MaximumExistsOnInterval(fa, fb, fc) && fb <= 0) {
      const void * pack[2] = { &f, auxiliary };
      ValueAtAbscissa fOpposite = [](double x, Context * ctx, const void * aux) {
        const void * const * pack = static_cast<const void * const *>(aux);
        ValueAtAbscissa fOriginal = *static_cast<const ValueAtAbscissa *>(pack[0]);
        const void * auxOriginal = pack[1];
        return -fOriginal(x, ctx, auxOriginal);
      };
      result = BrentMinimum(a, c, fOpposite, context, pack);
    }
    result = RoundCoordinatesToZero(result, a, c, f, context, auxiliary);
    if (result.x2() == 0.) {
      return result;
    }
    return Coordinate2D<double>(NAN, NAN);
  };
  double result = SolverHelper<double>::NextPointOfInterest(evaluation, context, auxiliary, search, start, end, relativePrecision, minimalStep, maximalStep).x1();
  // Because of float approximation, exact zero is never reached
  if (std::fabs(result) < relativePrecision * k_zeroPrecision) {
    return 0.;
  }
  return result;
}

Coordinate2D<double> Solver::NextMinimum(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double start, double end, double relativePrecision, double minimalStep, double maximalStep) {
  SolverHelper<double>::BracketSearch search = [](double a, double b, double c, double fa, double fb, double fc, SolverHelper<double>::ValueAtAbscissa f, Context * context, const void * auxiliary) {
    Coordinate2D<double> res = SolverHelper<double>::MinimumExistsOnInterval(fa, fb, fc) ? BrentMinimum(a, c, f, context, auxiliary) : Coordinate2D<double>(NAN, NAN);
    return RoundCoordinatesToZero(res, a, c, f, context, auxiliary);
  };
  Coordinate2D<double> result = SolverHelper<double>::NextPointOfInterest(evaluation, context, auxiliary, search, start, end, relativePrecision, minimalStep, maximalStep);
  /* Ignore extremum whose value is undefined or too big because they are
   * really unlikely to be local extremum. */
  if (std::isnan(result.x2()) || std::fabs(result.x2()) > k_maxFloat) {
    result.setX1(NAN);
  }
  return result;
}

double Solver::DefaultMaximalStep(double start, double stop) {
  double width = std::fabs(stop - start);
  double step = std::fmax(k_minimalStep, k_relativePrecision * width);
  /* The maximal step must be large enough not to be cancelled out in a sum. */
  double bound = std::fmax(std::fabs(start), std::fabs(stop));
  double epsilon = std::nextafter(bound, static_cast<double>(INFINITY)) - bound;
  return std::fmax(step, epsilon);
}

double Solver::BrentRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, const void * auxiliary) {
  if (ax > bx) {
    return BrentRoot(bx, ax, precision, evaluation, context, auxiliary);
  }
  double a = ax;
  double b = bx;
  double c = bx;
  double d = b-a;
  double e = b-a;
  double fa = evaluation(a, context, auxiliary);
  if (fa == 0.0) {
    // We are looking for a root. If a is already a root, just return it.
    return a;
  }
  double fb = evaluation(b, context, auxiliary);
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
      double fbcMiddle = evaluation(0.5*(b+c), context, auxiliary);
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
    fb = evaluation(b, context, auxiliary);
  }
  return NAN;
}

Coordinate2D<double> Solver::BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, const void * auxiliary) {
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  if (ax > bx) {
    return BrentMinimum(bx, ax, evaluation, context, auxiliary);
  }
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluation(x, context, auxiliary);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a))  {
      double middleFax = evaluation((x+a)/2.0, context, auxiliary);
      double middleFbx = evaluation((x+b)/2.0, context, auxiliary);
      double fa = evaluation(a, context, auxiliary);
      double fb = evaluation(b, context, auxiliary);
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
    fu = evaluation(u, context, auxiliary);
    if (fu <= fx || (std::isnan(fx) && !std::isnan(fu))) {
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

Coordinate2D<double> Solver::IncreasingFunctionRoot(double ax, double bx, double resultPrecision, ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double * resultEvaluation) {
  assert(ax < bx);
  double min = ax;
  double max = bx;
  double currentAbscissa = min;
  double eval = evaluation(currentAbscissa, context, auxiliary);
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
      if (currentAbscissa != -INFINITY) {
        currentAbscissa = std::nextafter(currentAbscissa, static_cast<double>(INFINITY));
      }
    }
    if (currentAbscissa == max) {
      if (currentAbscissa != INFINITY) {
        currentAbscissa = std::nextafter(currentAbscissa, -static_cast<double>(INFINITY));
      }
    }
    if (currentAbscissa == min || currentAbscissa == max) {
      break;
    }
    eval = evaluation(currentAbscissa, context, auxiliary);
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
T Solver::CumulativeDistributiveInverseForNDefinedFunction(T * probability, ValueAtAbscissa evaluation, Context * context, const void * auxiliary) {
  constexpr T precision = Float<T>::Epsilon();
  assert(*probability <= (static_cast<T>(1.f) - precision) && *probability >= precision);
  (void) precision;

  T cumulative = static_cast<T>(0.f);
  int result = -1;
  T delta;
  while (cumulative < *probability && cumulative < k_maxProbability && result < k_maxNumberOfOperations) {
    cumulative += evaluation(++result, context, auxiliary);
    delta = cumulative - *probability;
    if (delta * delta <= precision) {
      /* Consider we found the exact match. Otherwise, approximation errors
       * could round down and miss the exact result by one.
       * The tolerance used has been chosen empirically. */
      return result;
    }
  }
  if (cumulative >= k_maxProbability) {
    *probability = static_cast<T>(1.f);
    return result;
  }
  if (result == k_maxNumberOfOperations) {
    *probability = static_cast<T>(1.f);
    return INFINITY;
  }
  *probability = cumulative;
  if (std::isnan(cumulative)) {
    return NAN;
  }
  return result;
}

template<typename T>
T Solver::CumulativeDistributiveFunctionForNDefinedFunction(T x, ValueAtAbscissa evaluation, Context * context, const void * auxiliary) {
  int end = std::floor(x);
  double result = 0.0;
  for (int k = 0; k <=end; k++) {
    result += evaluation(k, context, auxiliary);
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

Coordinate2D<double> Solver::RoundCoordinatesToZero(Coordinate2D<double> xy, double a, double b, ValueAtAbscissa f, Context * context, const void * auxiliary) {
  Coordinate2D<double> result(xy);
  float tolerance = std::fabs(b - a) * k_zeroPrecision;
  if (std::fabs(result.x1()) < tolerance) {
    result.setX1(0.);
    result.setX2(f(0., context, auxiliary));
  }
  if (std::fabs(result.x2()) < tolerance) {
    result.setX2(0.);
  }
  return result;
}

template Coordinate2D<float> SolverHelper<float>::NextPointOfInterest(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, float start, float end, float relativePrecision, float minimalStep, float maximalStep);
template bool SolverHelper<float>::RootExistsOnInterval(float fa, float fb, float fc);

template float Solver::CumulativeDistributiveInverseForNDefinedFunction(float *, ValueAtAbscissa, Context *, const void *);
template double Solver::CumulativeDistributiveInverseForNDefinedFunction(double *, ValueAtAbscissa, Context *, const void *);
template float Solver::CumulativeDistributiveFunctionForNDefinedFunction(float, ValueAtAbscissa, Context *, const void *);
template double Solver::CumulativeDistributiveFunctionForNDefinedFunction(double, ValueAtAbscissa, Context *, const void *);
}

