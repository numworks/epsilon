#include <quiz.h>
#include "helper.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

class AdHocGraphController : public InteractiveCurveViewRangeDelegate {
public:
  /* These margins are obtained from instance methods of the various derived
   * class of SimpleInteractiveCurveViewController. As we cannot create an
   * instance of this class here, we define those directly. */
  static constexpr float k_topMargin = 0.068f;
  static constexpr float k_bottomMargin = 0.132948f;
  static constexpr float k_leftMargin = 0.04f;
  static constexpr float k_rightMargin = 0.04f;

  static float Ratio() { return InteractiveCurveViewRange::NormalYXRatio() / (1.f + k_topMargin + k_bottomMargin); }

  Context * context() { return &m_context; }
  ContinuousFunctionStore * functionStore() const { return &m_store; }

  // InteractiveCurveViewRangeDelegate
  bool defaultRangeIsNormalized() const override { return functionStore()->displaysNonCartesianFunctions(); }
  void interestingRanges(InteractiveCurveViewRange * range) override { DefaultInterestingRanges(range, context(), functionStore(), Ratio()); }
  float addMargin(float x, float range, bool isVertical, bool isMin) override { return DefaultAddMargin(x, range, isVertical, isMin, k_topMargin, k_bottomMargin, k_leftMargin, k_rightMargin); }
  void updateZoomButtons() override {}

private:
  mutable GlobalContext m_context;
  mutable ContinuousFunctionStore m_store;
};

bool float_equal(float a, float b, float tolerance = 10.f * FLT_EPSILON) {
  return IsApproximatelyEqual(a, b, tolerance, 0.);
}

template <size_t N>
void assert_best_range_is(const char * const (&definitions)[N], ContinuousFunction::PlotType const (&plotTypes)[N], float targetXMin, float targetXMax, float targetYMin, float targetYMax, Poincare::Preferences::AngleUnit angleUnit = Radian) {
  assert(std::isfinite(targetXMin) && std::isfinite(targetXMax) && std::isfinite(targetYMin) && std::isfinite(targetYMax)
      && targetXMin < targetXMax && targetYMin < targetYMax);

  Preferences::sharedPreferences()->setAngleUnit(angleUnit);

  AdHocGraphController graphController;
  InteractiveCurveViewRange graphRange(&graphController);

  for (size_t i = 0; i < N; i++) {
    addFunction(definitions[i], plotTypes[i], graphController.functionStore(), graphController.context());
  }
  graphRange.setDefault();
  float xMin = graphRange.xMin();
  float xMax = graphRange.xMax();
  float yMin = graphRange.yMin();
  float yMax = graphRange.yMax();
  quiz_assert(float_equal(xMin, targetXMin) && float_equal(xMax, targetXMax) && float_equal(yMin, targetYMin) && float_equal(yMax, targetYMax));

  graphController.functionStore()->removeAll();
}

void assert_best_cartesian_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin, float targetYMax, Poincare::Preferences::AngleUnit angleUnit = Radian, ContinuousFunction::PlotType plotType = Cartesian) {
  const char * definitionArray[1] = { definition };
  ContinuousFunction::PlotType plotTypeArray[1] = { plotType };
  assert_best_range_is(definitionArray, plotTypeArray, targetXMin, targetXMax, targetYMin, targetYMax, angleUnit);
}

QUIZ_CASE(graph_ranges_single_function) {
  assert_best_cartesian_range_is("undef", -10, 10, -5.81249952, 4.81249952);
  assert_best_cartesian_range_is("x!", -10, 10, -5.81249952, 4.81249952);

  assert_best_cartesian_range_is("0", -10, 10, -5.81249952, 4.81249952);
  assert_best_cartesian_range_is("1", -10, 10, -4.81249952, 5.81249952);
  assert_best_cartesian_range_is("-100", -10, 10, -105.8125, -95.1875);
  assert_best_cartesian_range_is("0.01", -10, 10, -5.81249952, 4.81249952);

  assert_best_cartesian_range_is("x", -10, 10, -5.66249943, 4.96249962);
  assert_best_cartesian_range_is("x+1", -12, 10, -6.19374943, 5.49374962);
  assert_best_cartesian_range_is("-x+5", -6, 17, -6.55937433, 5.65937471);
  assert_best_cartesian_range_is("x/2+2", -15, 7, -6.19374943, 5.49374962);


  assert_best_cartesian_range_is("x^2", -10, 10, -1.31249952, 9.3125);
  assert_best_cartesian_range_is("x^3", -10, 10, -5.16249943, 5.46249962);
  assert_best_cartesian_range_is("-2x^6", -10, 10, -16000, 2000);
  assert_best_cartesian_range_is("3x^2+x+10", -12, 11, 7.84062624, 20.0593758);

  assert_best_cartesian_range_is("1/x", -4.51764774, 4.51764774, -2.60000014, 2.20000005);
  assert_best_cartesian_range_is("1/(1-x)", -3.51176548, 5.71176529, -2.60000014, 2.29999995);
  assert_best_cartesian_range_is("1/(x^2+1)", -3.4000001, 3.4000001, -0.200000003, 1.10000002);

  assert_best_cartesian_range_is("sin(x)", -15, 15, -1.39999998, 1.20000005, Radian);
  assert_best_cartesian_range_is("cos(x)", -1000, 1000, -1.39999998, 1.20000005, Degree);
  assert_best_cartesian_range_is("tan(x)", -1000, 1000, -3.9000001, 3.4000001, Gradian);
  assert_best_cartesian_range_is("tan(x-100)", -1200, 1200, -4, 3.5, Gradian);

  assert_best_cartesian_range_is("ℯ^x", -10, 10, -1.71249962, 8.91249943);
  assert_best_cartesian_range_is("ℯ^x+4", -10, 10, 2.28750038, 12.9124994);
  assert_best_cartesian_range_is("ℯ^(-x)", -10, 10, -1.71249962, 8.91249943);
  assert_best_cartesian_range_is("(1-x)ℯ^(1/(1-x))", -1.8, 2.9, -3, 5.1);

  assert_best_cartesian_range_is("ln(x)", -2.85294199, 8.25294113, -3.5, 2.4000001);
  assert_best_cartesian_range_is("log(x)", -0.900000036, 3.20000005, -1.23906231, 0.939062357);

  assert_best_cartesian_range_is("√(x)", -3, 10, -2.10312462, 4.80312443);
  assert_best_cartesian_range_is("√(x^2+1)-x", -10, 10, -1.26249981, 9.36249924);
  assert_best_cartesian_range_is("root(x^3+1,3)-x", -2, 2.5, -0.445312381, 1.94531238);
}

QUIZ_CASE(graph_ranges_several_functions) {
  {
    const char * definitions[] = {"ℯ^x", "ln(x)"};
    ContinuousFunction::PlotType types[] = {Cartesian, Cartesian};
    assert_best_range_is(definitions, types, -10, 10, -2.81249952, 7.81249952);
  }
  {
    const char * definitions[] = {"x/2+2", "-x+5"};
    ContinuousFunction::PlotType types[] = {Cartesian, Cartesian};
    assert_best_range_is(definitions, types, -16, 17, -9.21562386, 8.31562424);
  }
  {
    const char * definitions[] = {"sin(θ)", "cos(θ)"};
    ContinuousFunction::PlotType types[] = {Polar, Polar};
    assert_best_range_is(definitions, types, -1.63235319, 2.13235331, -0.800000011, 1.20000005);
  }
}

void assert_zooms_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio, bool zoomIn) {
  float ratio = zoomIn ? 1.f / ZoomCurveViewController::k_zoomOutRatio : ZoomCurveViewController::k_zoomOutRatio;

  InteractiveCurveViewRange graphRange;
  graphRange.setXMin(xMin);
  graphRange.setXMax(xMax);
  graphRange.setYMin(yMin);
  graphRange.setYMax(yMax);

  float xCenter = (xMax + xMin) / 2.f;
  float yCenter = (yMax + yMin) / 2.f;

  graphRange.zoom(ratio, xCenter, yCenter);

  quiz_assert(float_equal(graphRange.xMin(), targetXMin) && float_equal(graphRange.xMax(), targetXMax) && float_equal(graphRange.yMin(), targetYMin) && float_equal(graphRange.yMax(), targetYMax));
  quiz_assert(float_equal((yMax - yMin) / (xMax - xMin), (targetYMax - targetYMin) / (targetXMax - targetXMin)) == conserveRatio);
}

void assert_zooms_in_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio) {
  assert_zooms_to(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax, conserveRatio, true);
}

void assert_zooms_out_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio) {
  assert_zooms_to(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax, conserveRatio, false);
}

QUIZ_CASE(graph_ranges_zoom) {
  assert_zooms_in_to(
      -12, 12, -12, 12,
      -8, 8, -8, 8,
      true);

  assert_zooms_in_to(
      -3, 3, 0, 1e-4,
      -3, 3, 0, 1e-4,
      true);

  assert_zooms_out_to(
      -10, 10, -10, 10,
      -15, 15, -15, 15,
      true);

  assert_zooms_out_to(
      -1, 1, 9e7, 1e8,
      -1.5, 1.5, 87500000, 1e8,
      false);
}

void assert_orthonormality(float xMin, float xMax, float yMin, float yMax, bool orthonormal) {
  InteractiveCurveViewRange graphRange;
  graphRange.setXMin(xMin);
  graphRange.setXMax(xMax);
  graphRange.setYMin(yMin);
  graphRange.setYMax(yMax);

  quiz_assert(graphRange.isOrthonormal() == orthonormal);
}

void assert_is_orthonormal(float xMin, float xMax, float yMin, float yMax) {
  assert_orthonormality(xMin, xMax, yMin, yMax, true);
}

void assert_is_not_orthonormal(float xMin, float xMax, float yMin, float yMax) {
  assert_orthonormality(xMin, xMax, yMin, yMax, false);
}

QUIZ_CASE(graph_ranges_orthonormal) {
  assert_is_orthonormal(-10, 10, -5.8125, 4.8125);
  assert_is_orthonormal(11.37037, 17.2963, 7.529894, 10.67804);
  assert_is_orthonormal(-1.94574, -1.165371, -2.476379, -2.061809);
  assert_is_orthonormal(0, 1000000, 0, 531250);
  assert_is_orthonormal(-3.2e-3f, 3.2e-3f, -1.7e-3f, 1.7e-3f);
  assert_is_not_orthonormal(-10, 10, -10, 10);
  assert_is_not_orthonormal(-10, 10, -5.8125, 4.8126);
  assert_is_not_orthonormal(1234548, 1234568, 1234556, 1234568);

  /* The ratio is 0.55 instead of 0.53125, but depending on the magnitude of
   * the bounds, it can land inside the margin of error. */
  assert_is_not_orthonormal(0, 20, 0, 11);
  assert_is_orthonormal(1e6, 1e6 + 20, 1e6, 1e6 + 11);

  /* The ration is the desired 0.53125, but if the bounds are near equal
   * numbers of large magnitude, the loss odf precision leaves us without any
   * significant bits. */
  assert_is_orthonormal(0, 3.2, 0, 1.7);
  assert_is_not_orthonormal(1e7, 1e7 + 3.2, 0, 1.7);
}

}
