#ifndef POINCARE_ZOOM_H
#define POINCARE_ZOOM_H

#include <poincare/range.h>
#include <poincare/solver.h>
#include <ion/display.h>

namespace Poincare {

class Zoom {
public:
  constexpr static float k_smallUnitMantissa = 1.f;
  constexpr static float k_mediumUnitMantissa = 2.f;
  constexpr static float k_largeUnitMantissa = 5.f;

  typedef Coordinate2D<float> (*Function2DWithContext)(float, const void *, Context *);

  static Range2D Sanitize(Range2D range, float normalRatio, float maxFloat);
  static Range2D DefaultRange(float normalRatio, float maxFloat) { return Sanitize(Range2D(), normalRatio, maxFloat); }

  Zoom(float tMin, float tMax, float normalRatio, Context * context) : m_bounds(tMin, tMax), m_context(context), m_normalRatio(normalRatio) {}

  /* This method is guaranteed to return a displayable range, that is a range
   * with non-nan bounds, and non-empty axes of some minimal length, with
   * bounds smaller than maxFloat in absolute value. */
  Range2D range(float maxFloat, bool forceNormalization) const;
  void setBounds(float min, float max) { m_bounds = Range1D(min, max); }
  /* These three functions will extend both X and Y axes. */
  void fitFullFunction(Function2DWithContext f, const void * model);
  void fitPointsOfInterest(Function2DWithContext f, const void * model);
  void fitIntersections(Function2DWithContext f1, const void * model1, Function2DWithContext f2, const void * model2);
  /* This function will only touch the Y axis. */
  void fitMagnitude(Function2DWithContext f, const void * model);

private:
  class HorizontalAsymptoteHelper {
  public:
    HorizontalAsymptoteHelper(float center) : m_center(center), m_left(NAN), m_right(NAN) {}

    Coordinate2D<float> left() const { return m_left; }
    Coordinate2D<float> right() const { return m_right; }
    void update(Coordinate2D<float> x, float slope);

  private:
    constexpr static float k_threshold = 0.2f; // TODO Tune
    constexpr static float k_hysteresis = 0.01f; // TODO Tune

    float m_center;
    Coordinate2D<float> m_left;
    Coordinate2D<float> m_right;
  };

  struct InterestParameters {
    Function2DWithContext f;
    const void * model;
    Context * context;
    HorizontalAsymptoteHelper * asymptotes;
  };

  constexpr static size_t k_sampleSize = Ion::Display::Width / 2;

  // Static methods for the Solver API
  static Solver<float>::Interest PointIsInteresting(Coordinate2D<float> a, Coordinate2D<float> b, Coordinate2D<float> c, const void * aux);
  static Coordinate2D<float> HonePoint(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest, float precision);

  Range1D sanitizedXRange() const;
  Range2D prettyRange() const;
  void fitWithSolver(Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone);
  void fitWithSolverHelper(float start, float end, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone);

  /* m_interestingRange is edited by fitFullFunction, fitPointsOfInterest and
   * fitIntersections, and will always be included in the final range. */
  Range2D m_interestingRange;
  Range1D m_magnitudeYRange;
  Range1D m_bounds;
  Context * m_context;
  float m_normalRatio;
};

}

#endif
