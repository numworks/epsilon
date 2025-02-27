#ifndef POINCARE_NUMERIC_ZOOM_H
#define POINCARE_NUMERIC_ZOOM_H

#include <ion/display.h>
#include <poincare/range.h>
#include <poincare/solver/solver.h>
#include <poincare/src/expression/context.h>
#include <poincare/src/memory/tree.h>

/* The unit tests need to be able to read the working values of
 * m_interestingRange and m_magnitudeYRange, but we do not want to make public
 * getters for those as it would weaken the Zoom API. */
template <typename T>
class ZoomTest;

namespace Poincare {

template <typename T>
using Function2D = Coordinate2D<T> (*)(T, const void*);

template <typename T>
class Zoom {
  friend class ::ZoomTest<T>;

 public:
  /* Sanitize will turn any random range into a range fit for display (see
   * comment on range() method below), that includes the original range. */
  static Range2D<T> Sanitize(Range2D<T> range, T normalRatio, T maxFloat);
  static Range2D<T> DefaultRange(T normalRatio, T maxFloat) {
    return Sanitize(Range2D<T>(), normalRatio, maxFloat);
  }

  Zoom(T tMin, T tMax, T normalRatio, T maxFloat)
      : m_bounds(tMin, tMax),
        m_defaultHalfLength(Range1D<T>::k_defaultHalfLength),
        m_normalRatio(normalRatio),
        m_maxFloat(maxFloat),
        m_maxPointsOnOneSide(k_defaultMaxPointsOnOneSide),
        m_thresholdForFunctionsExceedingNbOfPoints(
            k_defaultThresholdForFunctionsExceedingNbOfPoints) {
    /* The calculator screen is wider than it is high, but nothing in Zoom
     * relies on this assumption. */
    // assert(m_normalRatio <  static_cast<T>(1.));
  }

  /* This method is guaranteed to return a displayable range, that is a range
   * with non-nan bounds, and non-empty axes of some minimal length, with
   * bounds smaller than maxFloat in absolute value.
   * If beautify is false, the range will only be sanitized, without attempting
   * to improve its ratio. */
  Range2D<T> range(bool beautify, bool forceNormalization) const;
  void setBounds(T min, T max) { m_bounds = Range1D<T>(min, max, m_maxFloat); }
  void setForcedRange(Range2D<T> range) { m_forcedRange = range; }
  void setMaxPointsOneSide(int maxPointOnOneSide,
                           int thresholdForFunctionsExceedingNbOfPoints) {
    m_maxPointsOnOneSide = maxPointOnOneSide;
    m_thresholdForFunctionsExceedingNbOfPoints =
        thresholdForFunctionsExceedingNbOfPoints;
  }
  /* These four functions will extend both X and Y axes. */
  void fitPoint(Coordinate2D<T> xy, bool flipped = false, T leftMargin = 0.f,
                T rightMargin = 0.f, T bottomMargin = 0.f, T topMargin = 0.f);
  void fitPointsOfInterest(Function2D<T> f, const void* model,
                           bool vertical = false,
                           Function2D<double> fDouble = nullptr,
                           bool* finiteNumberOfPoints = nullptr);
  bool fitRoots(Function2D<T> f, const void* model, bool vertical = false,
                Function2D<double> fDouble = nullptr,
                bool* finiteNumberOfPoints = nullptr);
  void fitIntersections(Function2D<T> f1, const void* model1, Function2D<T> f2,
                        const void* model2, bool vertical = false);
  /* Piecewise should be a system function. */
  void fitConditions(const Internal::Tree* piecewise,
                     Function2D<T> fullFunction, const void* model,
                     bool vertical = false);
  /* This function will only touch the Y axis. */
  void fitMagnitude(Function2D<T> f, const void* model, bool cropOutliers,
                    bool vertical = false);
  void fitBounds(Function2D<T> f, const void* model, bool vertical = false);

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
    HorizontalAsymptoteHelper(T center)
        : m_center(center), m_left(-INFINITY, NAN), m_right(INFINITY, NAN) {}

    Coordinate2D<T> left() const { return privateGet(&m_left); }
    Coordinate2D<T> right() const { return privateGet(&m_right); }
    void update(Coordinate2D<T> x, T slope);

   private:
    constexpr static T k_threshold = 0.2f;    // TODO Tune
    constexpr static T k_hysteresis = 0.01f;  // TODO Tune

    Coordinate2D<T> privateGet(const Coordinate2D<T>* p) const {
      return std::isfinite(p->x()) ? *p : Coordinate2D<T>();
    }

    T m_center;
    Coordinate2D<T> m_left;
    Coordinate2D<T> m_right;
  };

  struct InterestParameters {
    Function2D<T> f;
    Function2D<double> fDouble;
    const void* model;
    HorizontalAsymptoteHelper* asymptotes;
    T (Coordinate2D<T>::*ordinate)() const;
    double (Coordinate2D<double>::*ordinateDouble)() const;
  };

  struct IntersectionParameters {
    Function2D<T> f1;
    Function2D<T> f2;
    const void* model1;
    const void* model2;
  };

  constexpr static size_t k_sampleSize = Ion::Display::Width / 2;

  constexpr static int k_defaultMaxPointsOnOneSide = 20;
  constexpr static int k_defaultThresholdForFunctionsExceedingNbOfPoints = 3;

  static typename Solver<T>::Interest PointIsInteresting(Coordinate2D<T> a,
                                                         Coordinate2D<T> b,
                                                         Coordinate2D<T> c,
                                                         const void* aux);
  static Coordinate2D<T> HonePoint(typename Solver<T>::FunctionEvaluation f,
                                   const void* aux, T a, T b,
                                   typename Solver<T>::Interest, T precision,
                                   OMG::Troolean discontinuous);
  static Coordinate2D<T> HoneRoot(typename Solver<T>::FunctionEvaluation f,
                                  const void* aux, T a, T b,
                                  typename Solver<T>::Interest, T precision,
                                  OMG::Troolean discontinuous);
  static Coordinate2D<T> HoneIntersection(
      typename Solver<T>::FunctionEvaluation f, const void* aux, T a, T b,
      typename Solver<T>::Interest, T precision, OMG::Troolean discontinuous);

  Range2D<T> sanitize2DHelper(Range2D<T> range) const;
  Range2D<T> sanitizedRange() const {
    return sanitize2DHelper(m_interestingRange);
  }
  bool xLengthCompatibleWithNormalization(T xLength, T xLengthNormalized) const;
  bool yLengthCompatibleWithNormalization(T yLength, T yLengthNormalized) const;
  Range2D<T> prettyRange(bool forceNormalization) const;
  bool fitWithSolver(
      bool* leftInterrupted, bool* rightInterrupted,
      typename Solver<T>::FunctionEvaluation evaluator, const void* aux,
      typename Solver<T>::BracketTest test, typename Solver<T>::HoneResult hone,
      bool vertical, Solver<double>::FunctionEvaluation fDouble = nullptr,
      typename Solver<T>::BracketTest testForCenterOfInterval = nullptr);
  bool fitWithSolverHelper(T start, T end, bool* interrupted,
                           typename Solver<T>::FunctionEvaluation evaluator,
                           const void* aux,
                           typename Solver<T>::BracketTest test,
                           typename Solver<T>::HoneResult hone, bool vertical,
                           Solver<double>::FunctionEvaluation fDouble);
  void privateFitPoint(Coordinate2D<T> xy, bool flipped = false);

  /* m_interestingRange is edited by fitPointsOfInterest and fitIntersections,
   * and will always be included in the final range, up to values of
   * ±m_TMax. */
  Range2D<T> m_interestingRange;
  Range2D<T> m_magnitudeRange;
  Range2D<T> m_forcedRange;
  Range1D<T> m_bounds;
  Context* m_context;
  T m_defaultHalfLength;
  T m_normalRatio;
  const T m_maxFloat;
  int m_maxPointsOnOneSide;
  int m_thresholdForFunctionsExceedingNbOfPoints;
};

}  // namespace Poincare

#endif
