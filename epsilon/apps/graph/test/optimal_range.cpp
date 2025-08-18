#include <apps/graph/graph/optimal_range.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/memoized_curve_view_range.h>
#include <poincare/include/poincare/range.h>
#include <poincare/print.h>
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
    std::initializer_list<Range1D<float>> intervals = {}) {
  assert(intervals.size() == 0 || intervals.size() == equations.size());
  GlobalContext context;
  ContinuousFunctionStore store;
  int i = 0;
  for (const char* equation : equations) {
    ContinuousFunction* function = addFunction(equation, &store, context);
    if (intervals.size() > 0) {
      Range1D<float> interval = *(intervals.begin() + i);
      function->setTAuto(false);
      function->setTMin(interval.min());
      function->setTMax(interval.max());
    }
    i++;
  }
  Range2D<float> range =
      Graph::OptimalRange(true, true, MemoizedCurveViewRange::k_defaultRange,
                          &store, false, &context);

  // A great precision is not expected in this test.
  float precision = 0.01f;
  bool xMinEqual =
      roughly_equal<float>(range.xMin(), optimalRange.xMin(), precision);
  bool xMaxEqual =
      roughly_equal<float>(range.xMax(), optimalRange.xMax(), precision);
  bool yMinEqual =
      roughly_equal<float>(range.yMin(), optimalRange.yMin(), precision);
  bool yMaxEqual =
      roughly_equal<float>(range.yMax(), optimalRange.yMax(), precision);
  if (xMinEqual && xMaxEqual && yMinEqual && yMaxEqual) {
    return;
  }
  constexpr size_t bufferSize = 200;
  char buffer[bufferSize] = "OptimalRange failure : ";
  for (const char* equation : equations) {
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %s", buffer,
                                  equation);
  }
  quiz_print(buffer);
  if (!xMinEqual) {
    Poincare::Print::CustomPrintf(
        buffer, bufferSize, "\txMin : %*.*ed instead of %*.*ed", range.xMin(),
        Preferences::PrintFloatMode::Decimal, 7, optimalRange.xMin(),
        Preferences::PrintFloatMode::Decimal, 7);
    quiz_print(buffer);
  }
  if (!xMaxEqual) {
    Poincare::Print::CustomPrintf(
        buffer, bufferSize, "\txMax : %*.*ed instead of %*.*ed", range.xMax(),
        Preferences::PrintFloatMode::Decimal, 7, optimalRange.xMax(),
        Preferences::PrintFloatMode::Decimal, 7);
    quiz_print(buffer);
  }
  if (!yMinEqual) {
    Poincare::Print::CustomPrintf(
        buffer, bufferSize, "\tyMin : %*.*ed instead of %*.*ed", range.yMin(),
        Preferences::PrintFloatMode::Decimal, 7, optimalRange.yMin(),
        Preferences::PrintFloatMode::Decimal, 7);
    quiz_print(buffer);
  }
  if (!yMaxEqual) {
    Poincare::Print::CustomPrintf(
        buffer, bufferSize, "\tyMax : %*.*ed instead of %*.*ed", range.yMax(),
        Preferences::PrintFloatMode::Decimal, 7, optimalRange.yMax(),
        Preferences::PrintFloatMode::Decimal, 7);
    quiz_print(buffer);
  }
  quiz_assert(false);
}

QUIZ_CASE(graph_optimal_range) {
  quiz_assert_optimal_range_is({"y=35000*1.06^(x-1)", "y=40000+2500(x-1)"},
                               Range2D<float>(-15.0, 12.7, -37.4, 69400));
}

}  // namespace Graph
