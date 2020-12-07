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

}
