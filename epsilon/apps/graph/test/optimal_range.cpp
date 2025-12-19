#include <apps/graph/graph/optimal_range.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/global_store.h>
#include <apps/shared/memoized_curve_view_range.h>
#include <poincare/include/poincare/range.h>
#include <poincare/print.h>
#include <poincare/symbol_context.h>
#include <poincare/test/float_helper.h>
#include <quiz.h>

#include <cmath>
#include <initializer_list>

#include "helper.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

void quiz_assert_optimal_range_is(
    std::initializer_list<const char*> equations, Range2D<float> optimalRange,
    std::initializer_list<Range1D<float>> intervals = {}, bool computeX = true,
    bool computeY = true,
    Range2D<float> originalRange = MemoizedCurveViewRange::k_defaultRange) {
  assert(intervals.size() == 0 || intervals.size() == equations.size());
  int i = 0;
  for (const char* equation : equations) {
    ContinuousFunction* function = AddFunction(equation);
    if (intervals.size() > 0) {
      const Range1D<float>& interval = intervals.begin()[i];
      function->setTAuto(false);
      function->setTMin(interval.min());
      function->setTMax(interval.max());
    }
    i++;
  }
  Range2D<float> range =
      Graph::OptimalRange(computeX, computeY, originalRange,
                          &GlobalContextAccessor::ContinuousFunctionContext(),
                          false, GlobalContextAccessor::Context());

  struct testCaseResult {
    const char* name;
    float observed;
    float expected;
    bool isEqual;
  };
  testCaseResult results[4] = {
      {"xMin", range.xMin(), optimalRange.xMin(), false},
      {"xMax", range.xMax(), optimalRange.xMax(), false},
      {"yMin", range.yMin(), optimalRange.yMin(), false},
      {"yMax", range.yMax(), optimalRange.yMax(), false},
  };
  // Reset function store
  GlobalContextAccessor::ContinuousFunctionStore().removeAll();

  bool success = true;
  // A great precision is not expected in this test.
  float precision = 0.01f;
  for (int i = 0; i < 4; i++) {
    results[i].isEqual = roughly_equal<float>(results[i].observed,
                                              results[i].expected, precision);
    success &= results[i].isEqual;
  }
  if (success) {
    return;
  }
  constexpr size_t bufferSize = 200;
  char buffer[bufferSize] = "OptimalRange failure : ";
  for (const char* equation : equations) {
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %s", buffer,
                                  equation);
  }
  quiz_print(buffer);
  for (int i = 0; i < 4; i++) {
    if (!results[i].isEqual) {
      Poincare::Print::CustomPrintf(
          buffer, bufferSize, "\t%s : %*.*ed instead of %*.*ed",
          results[i].name, results[i].observed,
          Preferences::PrintFloatMode::Decimal, 7, results[i].expected,
          Preferences::PrintFloatMode::Decimal, 7);
      quiz_print(buffer);
    }
  }
  quiz_assert(false);
}

QUIZ_CASE(graph_optimal_range) {
  quiz_assert_optimal_range_is(
      {"y=35000*1.06^(x-1)", "y=40000+2500(x-1)"},
      Range2D<float>(-325.3636, 170.2285, -382182.6, 69400));

  /* Following tests have been imported from screenshot testing */

  // grapher_autozoom_1
  quiz_assert_optimal_range_is({"f(x)=x"},
                               Range2D<float>(-10, 10, -5.312499, 5.3125));

  // grapher_autozoom_2
  quiz_assert_optimal_range_is(
      {"y=1000", "y=1001", "y=1003"},
      Range2D<float>(-0.01371989, 0.01371989, 1000, 1003));

  // grapher_autozoom_3
  quiz_assert_optimal_range_is(
      {"y=e^(x)", "y=x", "y=ln(x)"},
      Range2D<float>(-8.413036, 12.40855, -0.7385762, 10.32289));

  // grapher_autozoom_4
  quiz_assert_optimal_range_is(
      {"y=sin(x)", "y=cos(x)"},
      Range2D<float>(-7.85742, 7.85742, -1.457738, 1.457738));

  // grapher_autozoom_5
  quiz_assert_optimal_range_is({"y=x", "x^2+y^2+x×y+x+y=0", "r1(θ)=cos(θ)"},
                               Range2D<float>(-1.88353, 1.88271, -1.00082, 1));

  // grapher_autozoom_6
  quiz_assert_optimal_range_is({"y=x^3"},
                               Range2D<float>(-10, 10, -29.49891, 29.49898));

  // grapher_autozoom_7
  quiz_assert_optimal_range_is(
      {"y=√(x)", "y=tan(x)"},
      Range2D<float>(-9.375309, 12.0523, -5.763433, 5.619982));

  // grapher_autozoom_8
  quiz_assert_optimal_range_is(
      {"y=(x-2)^2+3", "y=(-x+9)^2-5"},
      Range2D<float>(-6.058021, 19.30364, -4.99998, 58.18197));

  // grapher_autozoom_9
  quiz_assert_optimal_range_is(
      {"y=x^2+1", "y=x^2-2"},
      Range2D<float>(-3.885127, 3.885127, -2, 2.127947));

  // grapher_autozoom_10
  quiz_assert_optimal_range_is(
      {"f(t)=(2×t+1,t^3-2×t+5)"},
      Range2D<float>(-1096.591, 1098.591, -7999595, 7999605),
      {Range1D<float>(-200, 200)});

  // grapher_autozoom_11
  quiz_assert_optimal_range_is({"r1(θ)=cos(θ)"},
                               Range2D<float>(-0.441176, 1.44118, -0.5, 0.5));

  // grapher_autozoom_12
  quiz_assert_optimal_range_is({"f(x)=sin(x)"},
                               Range2D<float>(-20.1, -20, -0.957051, -0.903927),
                               {Range1D<float>(-20.1, -20)});

  // grapher_autozoom_13
  quiz_assert_optimal_range_is(
      {"y=35×x+75"}, Range2D<float>(-6.956811, 4.81228, -37.58789, 112.5));

  // grapher_autozoom_14
  quiz_assert_optimal_range_is(
      {"y=1/35×x+75"}, Range2D<float>(-3941.319, 1313.773, -71.9894, 146.8834));

  // grapher_autozoom_15
  quiz_assert_optimal_range_is(
      {"y=x^3+6×x+5"},
      Range2D<float>(-10.76091, 9.239093, -62.03339, 33.67242));

  // grapher_autozoom_16
  quiz_assert_optimal_range_is(
      {"y=x", "y=√(x)-1", "y=x^2+2.3"},
      Range2D<float>(-6.445719, 13.83769, -1, 9.775561));

  // grapher_autozoom_piecewise_1
  quiz_assert_optimal_range_is({"f(x)=piecewise(-x+102,x<99,3,x<101,x-97)"},
                               Range2D<float>(93.0712, 103.465, 3, 8.52197));

  // grapher_autozoom_piecewise_2
  quiz_assert_optimal_range_is(
      {"f(x)=1+piecewise(2,x<100,4)"},
      Range2D<float>(93.478, 103.059, 2.542262, 5.457738));

  // grapher_autozoom_trig_intersections
  quiz_assert_optimal_range_is(
      {"y=-10×cos(x/12)+13", "y=10×cos(x)+13"},
      Range2D<float>(-113.191, 113.191, -1.577379, 27.57738));

  // grapher_autozoom_large_intersection
  quiz_assert_optimal_range_is(
      {"y=x^3-3×x^2-3×x+8", "y=1000"},
      Range2D<float>(-12.89047, 22.41468, -2.65685, 1000));

  // grapher_autozoom_forced_x_range
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-10, 10, 0, 10.625));
  // Force x in [-5,5]
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-5, 5, 0, 25), {},
                               false, true, Range2D<float>(-5, 5, 0, 10.625));
  // Force x in [-20,5]
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-20, 5, 0, 400), {},
                               false, true, Range2D<float>(-20, 5, 0, 25));

  // grapher_autozoom_costly_algorithm
  quiz_assert_optimal_range_is({"y=int(1/a,a,1,x)"},
                               Range2D<float>(-10, 10, -5.3125, 5.3125));

  // Autozoom on interval
  quiz_assert_optimal_range_is(
      {"f(x)=√((4-7x)^2+1)"},
      Range2D<float>(-2.615966, 5.615966, 1.000002, 17.02939),
      {Range1D<float>(0, 3)});
  quiz_assert_optimal_range_is({"f(t)=(3/t,t^3*exp(-2t))"},
                               Range2D<float>(0.5, 3, -0.03682627, 0.2061848),
                               {Range1D<float>(1, 6)});
  quiz_assert_optimal_range_is({"f(t)=(2tan(t)+1,2sec(t)^2+3)"},
                               Range2D<float>(-3.037914, 6.093452, 5, 9.851038),
                               {Range1D<float>(-0.8, 1)});
  // TODO: Improve xMax
  quiz_assert_optimal_range_is(
      {"y=cos(x)"}, Range2D<float>(0, 5.372952e7, -1.68656, 1.228869),
      {Range1D<float>(0, INFINITY)});

  // Lines
  quiz_assert_optimal_range_is(
      {"y=6x+120"}, Range2D<float>(-64.95879, 44.93436, -60.21983, 180));
  quiz_assert_optimal_range_is(
      {"y=0.6x+120"}, Range2D<float>(-326.1865, 125.9607, -60.20319, 180));
  quiz_assert_optimal_range_is(
      {"y=x+90", "y=-x-15"}, Range2D<float>(-242.691, 152.6031, -67.5, 142.5));
  quiz_assert_optimal_range_is({"y=10x"}, Range2D<float>(-10, 10, -15, 15));
  quiz_assert_optimal_range_is({"y=0.01x"},
                               Range2D<float>(-10, 10, -0.1457738, 0.1457738));
}

}  // namespace Graph
