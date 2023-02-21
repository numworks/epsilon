#include <float.h>
#include <poincare/erf_inv.h>
#include <poincare/test/helper.h>
#include <quiz.h>

#include <cmath>

using namespace Poincare;

QUIZ_CASE(erf_inv) {
  quiz_assert(erfInv(0.0) == 0.0);
  quiz_assert(std::isinf(erfInv(1.0)) && erfInv(1.0) > 0.0);
  quiz_assert(std::isinf(erfInv(-1.0)) && erfInv(-1.0) < 0.0);
  assert_roughly_equal(
      erfInv(0.5), 0.476936276204469873381418353643130559808969749059470644703,
      Float<double>::EpsilonLax());
  assert_roughly_equal(
      erfInv(0.25), 0.225312055012178104725014013952277554782118447807246757600,
      Float<double>::EpsilonLax());
  assert_roughly_equal(
      erfInv(0.999999),
      3.458910737279500022150927635957569519915669808042886747076,
      1E4 * Float<double>::EpsilonLax());
  assert_roughly_equal(
      erfInv(0.123456),
      0.109850294001424923867673480939041914394684494884310054922,
      Float<double>::EpsilonLax());
}
