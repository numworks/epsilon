#include <quiz.h>

#include "helpers.h"

QUIZ_CASE(solver_solving_range) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_auto_solving_range_is("cos(x)=0", -15.5654296875, 15.5654296875);
  assert_solves_with_auto_solving_range(
      "cos(x)=0", {-14.137166941154073, -10.995574287564276,
                   -7.8539816324381064, -4.7123889787100008,
                   -1.5707963267886629, 1.5707963267957366, 4.7123889818623867,
                   7.8539816549519345, 10.995574295553068, 14.137166941154067});
  assert_solves_with_auto_solving_range("ln(x)=11", {59874.141715133868});
  assert_solves_with_auto_solving_range(
      "(x+24543)^2+cos(x)-0.5^x=123456789123457890", {351339639.64352596});
  assert_solves_with_auto_solving_range("x^(1/3)=23456", {12905114402814.777});
}
