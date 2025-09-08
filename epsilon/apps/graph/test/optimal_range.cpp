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
  quiz_assert_optimal_range_is({"y=35000*1.06^(x-1)", "y=40000+2500(x-1)"},
                               Range2D<float>(-15.0, 12.7, -37.4, 69400));

  /* Following tests have been imported from screenshot testing */

  // grapher_autozoom_1
  quiz_assert_optimal_range_is({"f(x)=x"},
                               Range2D<float>(-18.8235, 18.8235, -10, 10));

  // grapher_autozoom_2
  quiz_assert_optimal_range_is({"y=1000", "y=1001", "y=1003"},
                               Range2D<float>(-10, 10, 1000, 1003));

  // grapher_autozoom_3
  quiz_assert_optimal_range_is({"y=e^(x)", "y=x", "y=ln(x)"},
                               Range2D<float>(-1.7963, 5.79181, 0, 4.03118));

  // grapher_autozoom_4
  quiz_assert_optimal_range_is({"y=sin(x)", "y=cos(x)"},
                               Range2D<float>(-7.85742, 7.85742, -1, 1));

  // grapher_autozoom_5
  quiz_assert_optimal_range_is({"y=x", "x^2+y^2+x×y+x+y=0", "r1(θ)=cos(θ)"},
                               Range2D<float>(-1.88353, 1.88271, -1.00082, 1));

  // grapher_autozoom_6
  quiz_assert_optimal_range_is({"y=x^3"},
                               Range2D<float>(-10, 10, -163.115, 163.116));

  // grapher_autozoom_7
  quiz_assert_optimal_range_is(
      {"y=√(x)", "y=tan(x)"},
      Range2D<float>(-4.7168, 7.39379, -3.46041, 2.97334));

  // grapher_autozoom_8
  quiz_assert_optimal_range_is(
      {"y=(x-2)^2+3", "y=(-x+9)^2-5"},
      Range2D<float>(2.00148, 11.2441, -4.99998, 54.6362));

  // grapher_autozoom_9
  quiz_assert_optimal_range_is({"y=x^2+1", "y=x^2-2"},
                               Range2D<float>(-4.71031, 4.71031, -2, 3.0047));

  // grapher_autozoom_10
  quiz_assert_optimal_range_is({"f(t)=(2×t+1,t^3-2×t+5)"},
                               Range2D<float>(-5.23051, 6.23051, 0, 6.08866),
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
      {"y=35×x+75"}, Range2D<float>(-12.1445, 7.85547, -350.059, 349.941));

  // grapher_autozoom_14
  quiz_assert_optimal_range_is(
      {"y=1/35×x+75"}, Range2D<float>(-2637.55, -2617.55, -0.358452, 0.212975));

  // grapher_autozoom_15
  quiz_assert_optimal_range_is(
      {"y=x^3+6×x+5"}, Range2D<float>(-10.7609, 9.23909, -397.16, 212.637));

  // grapher_autozoom_16
  quiz_assert_optimal_range_is({"y=x", "y=√(x)-1", "y=x^2+2.3"},
                               Range2D<float>(0, 7.39197, -1, 36.6899));

  // grapher_autozoom_piecewise_1
  quiz_assert_optimal_range_is({"f(x)=piecewise(-x+102,x<99,3,x<101,x-97)"},
                               Range2D<float>(93.0712, 103.465, 3, 8.52197));

  // grapher_autozoom_piecewise_2
  quiz_assert_optimal_range_is(
      {"f(x)=1+piecewise(2,x<100,4)"},
      Range2D<float>(93.478, 103.059, 1.45516, 6.54484));

  // grapher_autozoom_trig_intersections
  quiz_assert_optimal_range_is({"y=-10×cos(x/12)+13", "y=10×cos(x)+13"},
                               Range2D<float>(-113.191, 113.191, 3, 23));

  // grapher_autozoom_large_intersection
  quiz_assert_optimal_range_is(
      {"y=x^3-3×x^2-3×x+8", "y=1000"},
      Range2D<float>(-1.66992, 11.1953, -2.65685, 1000));

  // grapher_autozoom_forced_x_range
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-10, 10, 0, 10.625));
  // Force x in [-5,5]
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-5, 5, 0, 25), {},
                               false, true, Range2D<float>(-5, 5, 0, 10.625));
  // Force x in [-20,5]
  quiz_assert_optimal_range_is({"y=x^2"}, Range2D<float>(-20, 5, 0, 400), {},
                               false, true, Range2D<float>(-20, 5, 0, 25));
}

}  // namespace Graph
