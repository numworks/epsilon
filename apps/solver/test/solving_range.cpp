#include <quiz.h>

#include "helpers.h"

QUIZ_CASE(solver_solving_range) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solving_range_is("cos(x)=0", -15.5654296875, 15.5654296875);
  assert_solving_range_is("ln(x)=11", 54120.2578125, 66269.203125);
  assert_solving_range_is("(x+24543)^2+cos(x)-0.5^x=123456789123457890",
                          318647616, 390680384);
  assert_solving_range_is("x^(1/3)=23456", 11642386513920, 14351806234624);
}
