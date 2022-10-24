#include "helper.h"
#include <poincare/helpers.h>
#include <poincare/range.h>

using namespace Poincare;

QUIZ_CASE(poincare_range_1d_validity) {
  quiz_assert(Range1D(-10, 10).isValid());
  quiz_assert(!Range1D(NAN, NAN).isValid());

  quiz_assert(Range1D(0, 0).isEmpty());
  quiz_assert(!Range1D(123, 456).isEmpty());
}

void assert_length_and_center_are(Range1D r, float expextedLength, float expextedCenter) {
  float l = r.length();
  float c = r.center();
  quiz_assert(Helpers::EqualOrBothNan(l, expextedLength));
  quiz_assert(Helpers::EqualOrBothNan(c, expextedCenter));
}

QUIZ_CASE(poincare_range_1d_geometry) {
  assert_length_and_center_are(Range1D(NAN, NAN), NAN, NAN);
  assert_length_and_center_are(Range1D(0, 0), 0, 0);
  assert_length_and_center_are(Range1D(-123, -123), 0, -123);
  assert_length_and_center_are(Range1D(-10, 10), 20, 0);
  assert_length_and_center_are(Range1D(-4567, 89), 4656, -2239);
}

void assert_range_extends_to(Range1D r1, float p, Range1D r2) {
  r1.extend(p);
  quiz_assert(Helpers::EqualOrBothNan(r1.min(), r2.min()));
  quiz_assert(Helpers::EqualOrBothNan(r1.max(), r2.max()));
}

QUIZ_CASE(poincare_range_1d_extend) {
  assert_range_extends_to(Range1D(NAN, NAN), NAN, Range1D(NAN, NAN));
  assert_range_extends_to(Range1D(-1, 3), NAN, Range1D(-1, 3));
  assert_range_extends_to(Range1D(NAN, NAN), 0.123, Range1D(0.123, 0.123));
  assert_range_extends_to(Range1D(4, 4), 5.67, Range1D(4, 5.67));
  assert_range_extends_to(Range1D(4, 4), 1.23, Range1D(1.23, 4));
  assert_range_extends_to(Range1D(-12, -11.5), -11.7, Range1D(-12, -11.5));
  assert_range_extends_to(Range1D(-100, 100), 200, Range1D(-100, 200));
  assert_range_extends_to(Range1D(-100, 200), -200, Range1D(-200, 200));
}

void assert_range_zooms_to(Range1D r1, float r, float c, Range1D r2) {
  r1.zoom(r, c);
  quiz_assert(Helpers::EqualOrBothNan(r1.min(), r2.min()));
  quiz_assert(Helpers::EqualOrBothNan(r1.max(), r2.max()));
}

QUIZ_CASE(poincare_range_1d_zoom) {
  assert_range_zooms_to(Range1D(NAN, NAN), NAN, NAN, Range1D(NAN, NAN));
  assert_range_zooms_to(Range1D(NAN, NAN), 2, 1.23, Range1D(NAN, NAN));
  assert_range_zooms_to(Range1D(0, 0), 2, 0, Range1D(0, 0));
  assert_range_zooms_to(Range1D(0, 0), 2, 1, Range1D(-1, -1));
  assert_range_zooms_to(Range1D(-1, 1), 2, 0, Range1D(-2, 2));
  assert_range_zooms_to(Range1D(-1, 1), 2, -1, Range1D(-1, 3));
  assert_range_zooms_to(Range1D(-1, 1), 2, 1, Range1D(-3, 1));
  assert_range_zooms_to(Range1D(-8, 6), 0.5, 0, Range1D(-4, 3));
  assert_range_zooms_to(Range1D(-25, 2), 0.333333333333, -1, Range1D(-9, 0));
}
