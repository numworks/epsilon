#ifndef POINCARE_ZOOM_H
#define POINCARE_ZOOM_H

#include <poincare/point_of_interest.h>
#include <poincare/range.h>
#include <poincare/solver.h>
#include <ion/display.h>

namespace Poincare {

class Zoom {
public:
  /* FIXME Removing the "context" argument from Solver<>::FunctionEvaluation
   * was shortsighted, and makes it necessary to hack a new lambda in
   * grossFitToInterest. */
  typedef Coordinate2D<float> (*FunctionEvaluation2DWithContext)(float, const void *, Context *);

  constexpr static float k_smallUnitMantissa = 1.f;
  constexpr static float k_mediumUnitMantissa = 2.f;
  constexpr static float k_largeUnitMantissa = 5.f;

  static Range2D Sanitize(Range2D range, float normalYXRatio);
  static Range2D DefaultRange(float normalYXRatio) { return Sanitize(Range2D(), normalYXRatio); }

  /* A YX ratio is length of Y axis over length of X axis. For instance, a
   * normal YX ratio of 0.5 means the range ([-1,1],[2,3]) is normalized. */
  Zoom(float tMin, float tMax, float normalYXRatio, Context * context) : m_tMax(tMax), m_tMin(tMin), m_normalRatio(normalYXRatio), m_context(context), m_function(nullptr), m_sampleUpToDate(false) {}

  Range2D range() const { return m_range; }
  void setFunction(FunctionEvaluation2DWithContext f, const void * model);
  void includePoint(Coordinate2D<float> p);
  /* The fitX method will compute an X axis based on the points of interest of
   * the expression, or a default one if none are found. It will also compute
   * the Y range associated with those points. */
  void fitX();
  /* The fitOnlyY method is guaranteed to leave the X axis unmodified. */
  void fitOnlyY();
  /* This methods will tweak both X and Y to further refine the range. */
  void fitBothXAndY(bool forceNormalization);
  void fitFullFunction();

private:
  constexpr static size_t k_sampleSize = Ion::Display::Width / 2;
  constexpr static int k_maxPointsOnOneSide = 10;
  constexpr static float k_defaultHalfRange = Range1D::k_defaultHalfLength;

  // Static methods for the Solver API
  static Solver<float>::Interest PointIsInteresting(float ya, float yb, float yc);
  static Coordinate2D<float> SelectMiddle(Solver<float>::FunctionEvaluation f, const void * model, float a, float b, Solver<float>::Interest, float precision);

  Coordinate2D<float> approximate(float x) const { assert(m_function); return m_function(x, m_model, m_context); }
  void sampleY();
  void grossFitToInterest(float xStart, float xEnd);
  bool findNormalYAxis();
  bool findYAxisForOrderOfMagnitude();
  void expandSparseWindow();

  float m_sample[k_sampleSize];
  Range2D m_range;
  float m_tMax, m_tMin;
  float m_normalRatio;
  Context * m_context;
  FunctionEvaluation2DWithContext m_function;
  const void * m_model;
  bool m_sampleUpToDate;
};

}

#endif
