#ifndef POINCARE_JUNIOR_NUMERIC_ZOOM_H
#define POINCARE_JUNIOR_NUMERIC_ZOOM_H

#include <ion/display.h>
#include <poincare/piecewise_operator.h>
#include <poincare/range.h>
#include <poincare/solver.h>

/* The unit tests need to be able to read the working values of
 * m_interestingRange and m_magnitudeYRange, but we do not want to make public
 * getters for those as it would weaken the Zoom API. */
class ZoomTest;

namespace PoincareJ {

class Zoom {
  friend class ::ZoomTest;

 public:
  constexpr static float k_smallUnitMantissa = 1.f;
  constexpr static float k_mediumUnitMantissa = 2.f;
  constexpr static float k_largeUnitMantissa = 5.f;

  template <typename T>
  using Function2DWithContext = Coordinate2D<T> (*)(T, const void *, Context *);

  /* Sanitize will turn any random range into a range fit for display (see
   * comment on range() method below), that includes the original range. */
  static Range2D<float> Sanitize(Range2D<float> range, float normalRatio,
                                 float maxFloat);
  static Range2D<float> DefaultRange(float normalRatio, float maxFloat) {
    return Sanitize(Range2D<float>(), normalRatio, maxFloat);
  }

  Zoom(float tMin, float tMax, float normalRatio, Context *context,
       float maxFloat)
      : m_bounds(tMin, tMax),
        m_context(context),
        m_defaultHalfLength(Range1D<float>::k_defaultHalfLength),
        m_normalRatio(normalRatio),
        m_maxFloat(maxFloat),
        m_maxPointsOnOneSide(k_defaultMaxPointsOnOneSide),
        m_thresholdForFunctionsExceedingNbOfPoints(
            k_defaultThresholdForFunctionsExceedingNbOfPoints) {
    /* The calculator screen is wider than it is high, but nothing in Zoom
     * relies on this assumption. */
    // assert(m_normalRatio < 1.f);
  }

  /* This method is guaranteed to return a displayable range, that is a range
   * with non-nan bounds, and non-empty axes of some minimal length, with
   * bounds smaller than maxFloat in absolute value.
   * If beautify is false, the range will only be sanitized, without attempting
   * to improve its ratio. */
  Range2D<float> range(bool beautify, bool forceNormalization) const;
  void setBounds(float min, float max) {
    m_bounds = Range1D<float>(min, max, m_maxFloat);
  }
  void setForcedRange(Range2D<float> range) { m_forcedRange = range; }
  void setMaxPointsOneSide(int maxPointOnOneSide,
                           int thresholdForFunctionsExceedingNbOfPoints) {
    m_maxPointsOnOneSide = maxPointOnOneSide;
    m_thresholdForFunctionsExceedingNbOfPoints =
        thresholdForFunctionsExceedingNbOfPoints;
  }
  /* These four functions will extend both X and Y axes. */
  void fitPoint(Coordinate2D<float> xy, bool flipped = false,
                float leftMargin = 0.f, float rightMargin = 0.f,
                float bottomMargin = 0.f, float topMargin = 0.f);
  void fitPointsOfInterest(Function2DWithContext<float> f, const void *model,
                           bool vertical = false,
                           Function2DWithContext<double> fDouble = nullptr,
                           bool *finiteNumberOfPoints = nullptr);
  void fitRoots(Function2DWithContext<float> f, const void *model,
                bool vertical = false,
                Function2DWithContext<double> fDouble = nullptr,
                bool *finiteNumberOfPoints = nullptr);
  void fitIntersections(Function2DWithContext<float> f1, const void *model1,
                        Function2DWithContext<float> f2, const void *model2,
                        bool vertical = false);
  void fitConditions(PiecewiseOperator p,
                     Function2DWithContext<float> fullFunction,
                     const void *model, const char *symbol,
                     Preferences::ComplexFormat complexFormat,
                     Preferences::AngleUnit angleUnit, bool vertical = false);
  /* This function will only touch the Y axis. */
  void fitMagnitude(Function2DWithContext<float> f, const void *model,
                    bool cropOutliers, bool vertical = false);
  void fitBounds(Function2DWithContext<float> f, const void *model,
                 bool vertical = false);

 private:
  class HorizontalAsymptoteHelper {
    /* This helper is used to keep track of the slope at each step of the
     * search for points of interest, in order to detect horizontal asymptotes.
     * - if the bound is INFINITY, no slope greater than the threshold has been
     *   found yet.
     * - if the bound is NAN, the last slope was greater than the threshold,
     *   and finding a slope lower than the threshold will mark the beginning
     *   of an asymptote.
     * - if the bound is finite, we have found an asymptote ; finding a slope
     *   greater than the threshold will invalidate it.
     * We introduce hysteresis to avoid constantly finding and invalidating
     * asymptotes on functions such as y=0.2x, which oscillates around the
     * threshold due to imprecisions. */
   public:
    HorizontalAsymptoteHelper(float center)
        : m_center(center), m_left(-INFINITY, NAN), m_right(INFINITY, NAN) {}

    Coordinate2D<float> left() const { return privateGet(&m_left); }
    Coordinate2D<float> right() const { return privateGet(&m_right); }
    void update(Coordinate2D<float> x, float slope);

   private:
    constexpr static float k_threshold = 0.2f;    // TODO Tune
    constexpr static float k_hysteresis = 0.01f;  // TODO Tune

    Coordinate2D<float> privateGet(const Coordinate2D<float> *p) const {
      return std::isfinite(p->x()) ? *p : Coordinate2D<float>();
    }

    float m_center;
    Coordinate2D<float> m_left;
    Coordinate2D<float> m_right;
  };

  struct InterestParameters {
    Function2DWithContext<float> f;
    Function2DWithContext<double> fDouble;
    const void *model;
    Context *context;
    HorizontalAsymptoteHelper *asymptotes;
    float (Coordinate2D<float>::*ordinate)() const;
    double (Coordinate2D<double>::*ordinateDouble)() const;
  };

  struct IntersectionParameters {
    Function2DWithContext<float> f1;
    Function2DWithContext<float> f2;
    const void *model1;
    const void *model2;
    Context *context;
  };

  constexpr static size_t k_sampleSize = Ion::Display::Width / 2;

  constexpr static int k_defaultMaxPointsOnOneSide = 20;
  constexpr static int k_defaultThresholdForFunctionsExceedingNbOfPoints = 3;

  static Solver<float>::Interest PointIsInteresting(Coordinate2D<float> a,
                                                    Coordinate2D<float> b,
                                                    Coordinate2D<float> c,
                                                    const void *aux);
  static Coordinate2D<float> HonePoint(Solver<float>::FunctionEvaluation f,
                                       const void *aux, float a, float b,
                                       Solver<float>::Interest, float precision,
                                       TrinaryBoolean discontinuous);
  static Coordinate2D<float> HoneRoot(Solver<float>::FunctionEvaluation f,
                                      const void *aux, float a, float b,
                                      Solver<float>::Interest, float precision,
                                      TrinaryBoolean discontinuous);
  static Coordinate2D<float> HoneIntersection(
      Solver<float>::FunctionEvaluation f, const void *aux, float a, float b,
      Solver<float>::Interest, float precision, TrinaryBoolean discontinuous);

  Range2D<float> sanitize2DHelper(Range2D<float> range) const;
  Range2D<float> sanitizedRange() const {
    return sanitize2DHelper(m_interestingRange);
  }
  bool xLengthCompatibleWithNormalization(float xLength,
                                          float xLengthNormalized) const;
  bool yLengthCompatibleWithNormalization(float yLength,
                                          float yLengthNormalized) const;
  Range2D<float> prettyRange(bool forceNormalization) const;
  void fitWithSolver(
      bool *leftInterrupted, bool *rightInterrupted,
      Solver<float>::FunctionEvaluation evaluator, const void *aux,
      Solver<float>::BracketTest test, Solver<float>::HoneResult hone,
      bool vertical, Solver<double>::FunctionEvaluation fDouble = nullptr,
      Solver<float>::BracketTest testForCenterOfInterval = nullptr);
  /* Return true if the search was interrupted because too many points were
   * found. */
  bool fitWithSolverHelper(float start, float end,
                           Solver<float>::FunctionEvaluation evaluator,
                           const void *aux, Solver<float>::BracketTest test,
                           Solver<float>::HoneResult hone, bool vertical,
                           Solver<double>::FunctionEvaluation fDouble);
  void privateFitPoint(Coordinate2D<float> xy, bool flipped = false);

  /* m_interestingRange is edited by fitPointsOfInterest and fitIntersections,
   * and will always be included in the final range, up to values of
   * ±m_floatMax. */
  Range2D<float> m_interestingRange;
  Range2D<float> m_magnitudeRange;
  Range2D<float> m_forcedRange;
  Range1D<float> m_bounds;
  Context *m_context;
  float m_defaultHalfLength;
  float m_normalRatio;
  const float m_maxFloat;
  int m_maxPointsOnOneSide;
  int m_thresholdForFunctionsExceedingNbOfPoints;
};

}  // namespace PoincareJ

#endif
