#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_store_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1+42>A", "43");
  assert_parsed_expression_evaluates_to<double>("0.123+I>B", "0.123+I");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("A.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("B.exp").destroy();
}

QUIZ_CASE(poincare_store_simplify) {
  assert_parsed_expression_simplify_to("1+2>A", "A");
  assert_parsed_expression_simplify_to("1+2>x", "x");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("A.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}
