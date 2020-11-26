#include <quiz.h>
#include "helper.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

class AdHocGraphController : public InteractiveCurveViewRangeDelegate {
public:
  static constexpr float k_topMargin = 0.068f;
  static constexpr float k_bottomMargin = 0.132948f;
  static constexpr float k_leftMargin = 0.04f;
  static constexpr float k_rightMargin = 0.04f;

  static float Ratio() { return InteractiveCurveViewRange::NormalYXRatio() / (1.f + k_topMargin + k_bottomMargin); }

  Context * context() { return &m_context; }
  ContinuousFunctionStore * functionStore() const { return &m_store; }

  // InteractiveCurveViewRangeDelegate
  bool defaultRangeIsNormalized() const override { return functionStore()->displaysNonCartesianFunctions(); }
  void interestingRanges(InteractiveCurveViewRange * range) override { InterestingRangesHelper(range, context(), functionStore(), Ratio()); }
  float addMargin(float x, float range, bool isVertical, bool isMin) override { return AddMarginHelper(x, range, isVertical, isMin, k_topMargin, k_bottomMargin, k_leftMargin, k_rightMargin); }
  void updateZoomButtons() override {}

private:
  mutable GlobalContext m_context;
  mutable ContinuousFunctionStore m_store;
};

bool float_equal(float a, float b, float tolerance = 10.f * FLT_EPSILON) {
  return std::fabs(a - b) <= tolerance * std::fabs(a + b);
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

  assert_best_cartesian_range_is("x", -20, 20, -11.124999, 10.124999);
  assert_best_cartesian_range_is("x+1", -23, 21, -12.187499, 11.187499);
  assert_best_cartesian_range_is("-x+5", -17.0882378, 28.0882378, -13, 11);
  assert_best_cartesian_range_is("x/2+2", -15, 7, -6.19374943, 5.49374962);


  assert_best_cartesian_range_is("x^2", -2, 2, -0.412499845, 1.71249986);
  assert_best_cartesian_range_is("x^3", -5, 5, -2.80624962, 2.5062499);
  assert_best_cartesian_range_is("-2x^6", -2, 2, -1.51249993, 0.612499833);
  assert_best_cartesian_range_is("3x^2+x+10", -1.30000007, 1, 9.58406258, 10.8059368);

  assert_best_cartesian_range_is("1/x", -4.51764774, 4.51764774, -2.60000014, 2.20000005);
  assert_best_cartesian_range_is("1/(1-x)", -3.51176548, 5.71176529, -2.60000014, 2.29999995);
  assert_best_cartesian_range_is("1/(x^2+1)", -3.4000001, 3.4000001, -0.200000003, 1.10000002);

  assert_best_cartesian_range_is("sin(x)", -15, 15, -1.39999998, 1.20000005, Radian);
  assert_best_cartesian_range_is("cos(x)", -1000, 1000, -1.39999998, 1.20000005, Degree);
  assert_best_cartesian_range_is("tan(x)", -1000, 1000, -3.9000001, 3.4000001, Gradian);
  assert_best_cartesian_range_is("tan(x-100)", -1200, 1200, -4, 3.5, Gradian);

  assert_best_cartesian_range_is("ℯ^x", -5, 5, -1.50624979, 3.80624962);
  assert_best_cartesian_range_is("ℯ^x+4", -5, 5, 2.59375024, 7.90625);
  assert_best_cartesian_range_is("ℯ^(-x)", -5, 5, -1.50624979, 3.80624962);

  assert_best_cartesian_range_is("ln(x)", -2.85294199, 8.25294113, -3.5, 2.4000001);
  assert_best_cartesian_range_is("log(x)", -0.900000036, 3.20000005, -1.23906231, 0.939062357);

  assert_best_cartesian_range_is("√(x)", -3, 10, -2.10312462, 4.80312443);
  assert_best_cartesian_range_is("√(x^2+1)-x", -5, 5, -1.50624979, 3.80624962);
  assert_best_cartesian_range_is("root(x^3+1,3)-x", -2, 2.5, -0.445312381, 1.94531238);
}

QUIZ_CASE(graph_ranges_several_functions) {
  {
    const char * definitions[] = {"ℯ^x", "ln(x)"};
    ContinuousFunction::PlotType types[] = {Cartesian, Cartesian};
    assert_best_range_is(definitions, types, -6.52941275, 8.52941322, -3.79999995, 4.20000029);
  }
  {
    const char * definitions[] = {"x/2+2", "-x+5"};
    ContinuousFunction::PlotType types[] = {Cartesian, Cartesian};
    assert_best_range_is(definitions, types, -17.0882378, 28.0882378, -13, 11);
  }
  {
    const char * definitions[] = {"sin(θ)", "cos(θ)"};
    ContinuousFunction::PlotType types[] = {Polar, Polar};
    assert_best_range_is(definitions, types, -1.63235319, 2.13235331, -0.800000011, 1.20000005);
  }
}

}
