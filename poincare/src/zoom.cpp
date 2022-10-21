#include <poincare/zoom.h>
#include <poincare/trinary_boolean.h>
#include <string.h>

namespace Poincare {

// Public

Range2D Zoom::Sanitize(Range2D range, float normalYXRatio, float maxFloat) {
  Zoom zoom(-maxFloat, maxFloat, normalYXRatio, nullptr);
  zoom.m_interestingRange = range;
  return zoom.range(maxFloat, false);
}

 Range2D Zoom::range(float maxFloat, bool forceNormalization) const {
  Range2D result;
  Range2D pretty = prettyRange();
  result.x().setMin(pretty.xMin(), maxFloat);
  result.x().setMax(pretty.xMax(), maxFloat);
  result.y().setMin(pretty.yMin(), maxFloat);
  result.y().setMax(pretty.yMax(), maxFloat);

  if (forceNormalization) {
    result.setRatio(m_normalRatio, false);
  }

  return result;
}

void Zoom::fitFullFunction(Function2DWithContext f, const void * model) {
  float step = m_bounds.length() / (k_sampleSize - 1);
  for (size_t i = 0; i < k_sampleSize; i++) {
    float t = m_bounds.min() + step * i;
    m_interestingRange.extend(f(t, model, m_context));
  }
}

void Zoom::fitPointsOfInterest(Function2DWithContext f, const void * model) {
  struct Parameters {
    Function2DWithContext f;
    const void * model;
    Context * context;
  };
  Parameters params = { .f = f, .model = model, .context = m_context };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const Parameters * p = static_cast<const Parameters *>(aux);
    return p->f(t, p->model, p->context).x2(); // TODO Zoom could also work for x=f(y) functions
  };
  fitWithSolver(evaluator, &params, PointIsInteresting);
}

void Zoom::fitIntersections(Function2DWithContext f1, const void * model1, Function2DWithContext f2, const void * model2) {
  struct Parameters {
    Function2DWithContext f1;
    Function2DWithContext f2;
    const void * model1;
    const void * model2;
    Context * context;
  };
  Parameters params = { .f1 = f1, .f2 = f2, .model1 = model1, .model2 = model2, .context = m_context };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const Parameters * p = static_cast<const Parameters *>(aux);
    return p->f1(t, p->model1, p->context).x2() - p->f2(t, p->model2, p->context).x2();
  };
  fitWithSolver(evaluator, &params, Solver<float>::EvenOrOddRootInBracket);
}

void Zoom::fitMagnitude(Function2DWithContext f, const void * model) {
  /* We comute the log mean value of the expression, which gives an idea of the
   * order of magnitude of the function, to crop the Y axis. */
  constexpr float aboutZero = Solver<float>::k_minimalAbsoluteStep;
  Range1D sample;
  float nSum = 0.f, pSum = 0.f;
  int nPop = 0, pPop = 0;

  Range1D xRange = sanitizedXRange();
  float step = xRange.length() / (k_sampleSize - 1);
  for (int i = 0; i < k_sampleSize; i++) {
    float x = xRange.min() + i * step;
    float y = f(x, model, m_context).x2(); // TODO Zoom could also work for x=f(y) functions
    sample.extend(y);
    float yAbs = std::fabs(y);
    if (yAbs <= aboutZero) {
      continue;
    }
    float yLog = std::log(yAbs);
    if (y < 0.f) {
      nSum += yLog;
      nPop++;
    } else {
      pSum += yLog;
      pPop++;
    }
  }
  float yMax = pPop > 0 ? std::min(sample.max(), std::exp(pSum / pPop  + 1.f)) : sample.max();
  m_magnitudeYRange.extend(yMax);
  float yMin = nPop > 0 ? std::max(sample.min(), -std::exp(nSum / nPop  + 1.f)) : sample.min();
  m_magnitudeYRange.extend(yMin);
}

// Private

Solver<float>::Interest Zoom::PointIsInteresting(float ya, float yb, float yc) {
  Solver<float>::BracketTest tests[] = { Solver<float>::OddRootInBracket, Solver<float>::MinimumInBracket, Solver<float>::MaximumInBracket, Solver<float>::DiscontinuityInBracket };
  Solver<float>::Interest interest;
  for (Solver<float>::BracketTest & test : tests) {
    interest = test(ya, yb, yc);
    if (interest != Solver<float>::Interest::None) {
      break;
    }
  }
  return interest;
}

Coordinate2D<float> Zoom::HonePoint(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest interest, float precision) {
  constexpr int k_numberOfIterations = 10; // TODO Tune
  bool convex = true;
  float fa = f(a, aux), fb = f(b, aux);
  float c = 0.5f * (a + b), fc = f(c, aux);
  float u, fu, v, fv;
  for (int i = 0; i < k_numberOfIterations; i++) {
    u = 0.5f * (a + c);
    v = 0.5f * (c + b);
    fu = f(u, aux);
    fv = f(v, aux);

    convex = convex && std::fabs(fu - fc) <= std::fabs(fa - fc) && std::fabs(fv - fc) <= std::fabs(fb - fc);

    if (PointIsInteresting(fc, fv, fb) != Solver<float>::Interest::None || PointIsInteresting(fa, fu, fc) == Solver<float>::Interest::None) {
      a = c;
      fa = fc;
      c = v;
      fc = fv;
    } else {
      b = c;
      fb = fc;
      c = u;
      fc = fu;
    }
  }
  return Coordinate2D<float>(b, interest == Solver<float>::Interest::Root ? 0.f : convex ? fb : NAN);
}

Range1D Zoom::sanitizedXRange() const {
  Range2D thisRange = m_interestingRange; // Copy for const-ness
  if (!thisRange.x().isValid()) {
    assert(!thisRange.y().isValid());
    return Range1D(-Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength);
  }
  if (thisRange.x().isEmpty()) {
    float c = thisRange.x().min();
    return Range1D(c - Range1D::k_defaultHalfLength, c + Range1D::k_defaultHalfLength);
  }
  // FIXME Add margin around interesting range ?
  return thisRange.x();
}

Range2D Zoom::prettyRange() const {
  Range2D thisRange = m_interestingRange; // Copy for const-ness
  Range1D xRange = sanitizedXRange();

  Range1D yRange = thisRange.y();
  yRange.extend(m_magnitudeYRange.min());
  yRange.extend(m_magnitudeYRange.max());

  float xLength = xRange.length();
  float yLength = yRange.length();
  float xLengthNormalized = yLength / m_normalRatio;
  float yLengthNormalized = xLength * m_normalRatio;
  constexpr float k_minimalXCoverage = 0.8f;
  constexpr float k_minimalYCoverage = 0.3f;
  bool yCanBeNormalized = yLengthNormalized * k_minimalYCoverage <= yLength && thisRange.y().length() <= yLengthNormalized;
  if (!yRange.isValid() || yLength == 0.f || yCanBeNormalized) {
    float yCenter = yRange.center();
    if (!std::isfinite(yCenter)) {
      yCenter = 0.f;
    }
    yRange = Range1D(yCenter - 0.5f * yLengthNormalized, yCenter + 0.5f * yLengthNormalized);
  } else if (xLengthNormalized * k_minimalXCoverage <= xLength && xLength <= xLengthNormalized) {
    float xCenter = xRange.center();
    xRange = Range1D(xCenter - xLengthNormalized * 0.5f, xCenter + xLengthNormalized * 0.5f);
  }

  assert(xRange.isValid() && !xRange.isEmpty() && yRange.isValid() && !yRange.isEmpty());
  return Range2D(xRange, yRange);
}

void Zoom::fitWithSolver(Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test) {
  /* Step away from the center, as it is more likely to be a significant value
   * (typically zero).*/
  float c = m_bounds.center();
  float d = std::max(Solver<float>::k_minimalAbsoluteStep, std::fabs(c * Solver<float>::k_relativePrecision));
  fitWithSolverHelper(c - d, m_bounds.max(), evaluator, aux, test);
  fitWithSolverHelper(c - d, m_bounds.min(), evaluator, aux, test);
}

void Zoom::fitWithSolverHelper(float start, float end, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test) {
  constexpr int k_maxPointsOnOneSide = 20;
  constexpr int k_maxPointsIfInfinite = 5;

  Solver<float> solver(start, end);
  Range2D tempRange;
  Coordinate2D<float> p = solver.next(evaluator, aux, test, HonePoint);
  int n = 0;
  while (solver.lastInterest() != Solver<float>::Interest::None) {
    m_interestingRange.extend(p);
    p = solver.next(evaluator, aux, test, HonePoint);
    n++;
    if (n == k_maxPointsIfInfinite) {
      tempRange = m_interestingRange;
    } else if (n >= k_maxPointsOnOneSide) {
      m_interestingRange = tempRange;
      break;
    }
  }
}

}
