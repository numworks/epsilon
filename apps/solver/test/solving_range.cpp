#include <quiz.h>

#include "helpers.h"

QUIZ_CASE(solver_solving_range) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solving_range_is("cos(x)=0", -14.150390625, 14.150390625);
}
