#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include "../law/erf_inv.h"

QUIZ_CASE(erf_inv) {
  assert(erfInv(0.0) == 0.0);
  assert(std::isinf(erfInv(1.0))  && erfInv(1.0) > 0.0);
  assert(std::isinf(erfInv(-1.0)) && erfInv(-1.0) < 0.0);
  assert(std::fabs(erfInv(0.5) - 0.476936276204469873381418353643130559808969749059470644703) < DBL_EPSILON);
  assert(std::fabs(erfInv(0.25) - 0.225312055012178104725014013952277554782118447807246757600) < DBL_EPSILON);
  assert(std::fabs(erfInv(0.999999) - 3.458910737279500022150927635957569519915669808042886747076) < 10E-6);
  assert(std::fabs(erfInv(0.123456) - 0.109850294001424923867673480939041914394684494884310054922) < DBL_EPSILON);

}

