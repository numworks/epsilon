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

  // TODO_PCJ: unhandled dependency: dep(-11+Ln(a),{nonNull(a)})
  //   assert_solves_with_auto_solving_range("ln(x)=11", {59874.141715133868});

  assert_solves_with_auto_solving_range(
      "(x+24543)^2+cos(x)-0.5^x=123456789123457890", {351339639.64352596});
  assert_solves_with_auto_solving_range(
      "12sin(7x)+11=3+4sin(7x)",
      {-8.3027805794545024, -7.405182689682185, -6.5075846999145446,
       -5.6099870040946067, -4.7123889402234793, -3.8147910548038975,
       -2.9171931899796668, -2.0195952368820635, -1.1219973820046962,
       -0.22439947705439328});
  assert_solves_with_auto_solving_range("x^(1/3)=23456", {12905114402814.777});
  assert_solves_with_auto_solving_range(
      "int((e^x+x),x,x,3)=2", {-6.7207635596742206, 2.9097502093514773});

  // TODO_PCJ: unhandled dependency: dep(-13 - 8×Ln(-1 + 2×a), {nonNull(2×a -
  // 1×1)})
  //   assert_solves_with_auto_solving_range("1-8*ln(2x-1)=14",
  //                                         {0.59845583599312557});

  assert_solves_with_auto_solving_range("2^(3-8x)-7=11",
                                        {-0.14624062522874318});
  assert_auto_solving_range_is("x^(1/3)=234567", -10, 10);
}
