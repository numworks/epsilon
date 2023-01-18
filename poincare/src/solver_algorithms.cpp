#include <poincare/solver_algorithms.h>

namespace Poincare {

Coordinate2D<double> SolverAlgorithms::IncreasingFunctionRoot(double ax, double bx, double resultPrecision, Solver<double>::FunctionEvaluation f, const void * aux, double * resultEvaluation) {
  assert(ax < bx);
  double min = ax;
  double max = bx;
  double currentAbscissa = min;
  double eval = f(currentAbscissa, aux);
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
    eval = f(currentAbscissa, aux);
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
T SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction(T * probability, typename Solver<T>::FunctionEvaluation f, const void * aux) {
  constexpr T precision = Float<T>::Epsilon();
  assert(*probability <= (static_cast<T>(1.f) - precision) && *probability >= precision);
  (void) precision;

  T cumulative = static_cast<T>(0.f);
  int result = -1;
  T delta;
  while (cumulative < *probability && cumulative < k_maxProbability && result < k_numberOfIterationsProbability) {
    cumulative += f(++result, aux);
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
  if (result == k_numberOfIterationsProbability) {
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
T SolverAlgorithms::CumulativeDistributiveFunctionForNDefinedFunction(T x, typename Solver<T>::FunctionEvaluation f, const void * aux) {
  int end = std::floor(x);
  T result = 0.0;
  for (int k = 0; k <=end; k++) {
    result += f(k, aux);
    /* Avoid too long loop */
    if (k > k_numberOfIterationsProbability) {
      break;
    }
    if (result >= k_maxProbability) {
      result = 1.0;
      break;
    }
  }
  return result;
}

Coordinate2D<double> SolverAlgorithms::BrentRoot(Solver<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver<double>::Interest interest, double precision) {
  if (xMax < xMin) {
    return BrentRoot(f, aux, xMax, xMin, interest, precision);
  }
  assert(interest == Solver<double>::Interest::Root);
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

  for (int i = 0; i < k_numberOfIterationsBrent; i++) {
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
      double q = 1. - s;
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

Coordinate2D<double> SolverAlgorithms::BrentMinimum(Solver<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver<double>::Interest interest, double precision) {
  if (xMax < xMin) {
    return BrentMinimum(f, aux, xMax, xMin, interest, precision);
  }
  assert(xMin < xMax);

  if (FunctionSeemsConstantOnTheInterval(f, aux, xMin, xMax)) {
    /* Some fake minimums can be detected due to approximations errors like in
     * f(x) = x/abs(x) in complex mode. */
    return Coordinate2D<double>(NAN, NAN);
  }

  double a = xMin;
  double b = xMax;
  double e = 0.;
  double x = a + k_goldenRatio * (b - a);
  double v = x;
  double w = x;
  double fx = f(x, aux);
  double fv = fx;
  double fw = fx;
  double d = NAN;

  double u, fu;

  for (int i = 0; i < k_numberOfIterationsBrent; i++) {
    double m = 0.5 * (a + b);
    double tol1 = k_sqrtEps * std::fabs(x) + precision;
    double tol2 = 2. * tol1;

    if (std::fabs(x - m) <= tol2 - 0.5 * (b - a)) {
      double faxMiddle = f((x + a) / 2., aux);
      double fbxMiddle = f((x + b) / 2., aux);
      double fa = f(a, aux);
      double fb = f(b, aux);

      if (faxMiddle - fa <= k_sqrtEps
       && fx - faxMiddle <= k_sqrtEps
       && fx - fbxMiddle <= k_sqrtEps
       && fbxMiddle - fb <= k_sqrtEps)
      {
        return Coordinate2D<double>(x, fx);
      }
    }

    double p = 0;
    double q = 0;
    double r = 0;

    if (std::fabs(e) > tol1) {
      r = (x - w) * (fx - fv);
      q = (x - v) * (fx - fw);
      p = (x - v) * q - (x - w) * r;
      q = 2. * (q - r);

      if (q > 0.) {
        p = -p;
      } else {
        q = -q;
      }

      r = e;
      e = d;
    }

    if (std::fabs(p) < std::fabs(0.5 * q * r) && p < q * (a - x) && p < q * (b - x)) {
      d = p / q;
      u = x + d;

      if (u - a < tol2 || b - u < tol2) {
        d = x < m ? tol1 : -tol1;
      }
    } else {
      e = x < m ? b - x : a - x;
      d = k_goldenRatio * e;
    }

    u = x + (std::fabs(d) >= tol1 ? d : (d > 0 ? tol1 : -tol1));
    fu = f(u, aux);

    if (fu <= fx || (std::isnan(fx) && !std::isnan(fu))) {
      if (u < x) {
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
      if (u < x) {
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

  return Coordinate2D<double>(NAN, NAN);
}

bool SolverAlgorithms::FunctionSeemsConstantOnTheInterval(Solver<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax) {
  assert(xMin < xMax);
  constexpr int k_numberOfSteps = 20;
  double values[k_numberOfSteps];
  int valuesCount[k_numberOfSteps];
  int currentNumberOfValues = 0;
  /* This loop computes 20 values of f on the interval and then checks the
   * repartition of these values. If the function takes a few number of
   * different values, it might mean that f is a constant function but
   * approximation errors led to thinking there was a minimum in the interval.
   * To mesure this "repartition" of values, the entropy of the data is
   * then calculated.
   * */
  for (int i = 0; i < k_numberOfSteps; i++) {
    double currentValue = f(xMin + (static_cast<double>(i) / k_numberOfSteps) * (xMax - xMin), aux);
    bool addValueToArray = true;
    for (int k = 0; k < currentNumberOfValues; k++) {
      if (values[k] == currentValue) {
        addValueToArray = false;
        valuesCount[k]++;
        break;
      }
    }
    if (addValueToArray) {
      values[currentNumberOfValues] = currentValue;
      valuesCount[currentNumberOfValues] = 1;
      currentNumberOfValues++;
    }
  }

  /* Entropy = -sum(log(pk)*pk) where pk is the probability of taking the
   * k-th value. */
  double entropy = 0.;
  for (int k = 0; k < currentNumberOfValues; k++) {
    double probabilityOfValue = static_cast<double>(valuesCount[k]) / (k_numberOfSteps + 1);
    entropy += - std::log(probabilityOfValue) * probabilityOfValue;
  }

  // Unfortunately, std::log is not constexpr
  double maxEntropy = std::log(static_cast<double>(k_numberOfSteps));
  assert(entropy >= 0 && entropy <= maxEntropy);
  /* If the entropy of the data is lower than 0.5 * maxEntropy, it is assumed
   * that the function is constant on [xMin, xMax].
   * The value of 0.5 has be chosen because of good experimental results but
   * could be tweaked.
   * */
  constexpr double k_entropyThreshold = 0.5;
  return entropy < maxEntropy * k_entropyThreshold;
}

// Explicit template instantiations

template float SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction(float * probability, Solver<float>::FunctionEvaluation f, const void * aux);
template double SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction(double * probability, Solver<double>::FunctionEvaluation f, const void * aux);
template float SolverAlgorithms::CumulativeDistributiveFunctionForNDefinedFunction(float x, Solver<float>::FunctionEvaluation f, const void * aux);
template double SolverAlgorithms::CumulativeDistributiveFunctionForNDefinedFunction(double x, Solver<double>::FunctionEvaluation f, const void * aux);
}
