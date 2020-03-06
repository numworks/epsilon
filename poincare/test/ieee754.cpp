#include <poincare/ieee754.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_next_and_previous_IEEE754_is(T a, T b) {
  T next = IEEE754<T>::next(a);
  T previous = IEEE754<T>::previous(b);
  quiz_assert((std::isnan(next) && std::isnan(b)) || next == b);
  quiz_assert((std::isnan(previous) && std::isnan(a)) || previous == a);
}

QUIZ_CASE(ieee754_next_and_previous) {
  assert_next_and_previous_IEEE754_is<float>(0.0f, 1.4E-45f);
  assert_next_and_previous_IEEE754_is<float>(INFINITY, INFINITY);
  assert_next_and_previous_IEEE754_is<float>(NAN, NAN);
  assert_next_and_previous_IEEE754_is<float>(2.2566837E-10f, 2.2566839E-10f);
  assert_next_and_previous_IEEE754_is<float>(-INFINITY, -INFINITY);
  assert_next_and_previous_IEEE754_is<float>(-0.0f, 0.0f);
  assert_next_and_previous_IEEE754_is<float>(-1.4E-45f, -0.0f);
  assert_next_and_previous_IEEE754_is<float>(-3.4359738E10f, -3.43597363E10f);
  quiz_assert(IEEE754<float>::next(3.4028235E38f) == INFINITY);
  quiz_assert(IEEE754<float>::previous(-3.4028235E38f) == -INFINITY);

  assert_next_and_previous_IEEE754_is<double>(0.0f, 4.94065645841246544176568792868E-324);
  assert_next_and_previous_IEEE754_is<double>(INFINITY, INFINITY);
  assert_next_and_previous_IEEE754_is<double>(NAN, NAN);
  assert_next_and_previous_IEEE754_is<double>(1.936766735060658315512927142E-282, 1.93676673506065869777770528092E-282);
  assert_next_and_previous_IEEE754_is<double>(-INFINITY, -INFINITY);
  assert_next_and_previous_IEEE754_is<double>(-0.0, 0.0);
  assert_next_and_previous_IEEE754_is<double>(-4.94065645841246544176568792868E-324, -0.0);
  assert_next_and_previous_IEEE754_is<double>(-1.38737906372912431085182213247E201, -1.38737906372912403890916981028E201);
  quiz_assert(IEEE754<double>::next(1.79769313486231570814527423732E308) == INFINITY);
  quiz_assert(IEEE754<double>::previous(-1.79769313486231570814527423732E308) == -INFINITY);

}
