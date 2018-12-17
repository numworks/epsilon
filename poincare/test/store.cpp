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
  assert_parsed_expression_simplify_to("1+2>A", "1+2");
  assert_parsed_expression_simplify_to("1+2>x", "1+2");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("A.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(poincare_store_matrix) {
  assert_parsed_expression_evaluates_to<double>("[[7]]>a", "[[7]]");
  assert_parsed_expression_simplify_to("1+1>a", "1+1");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_store_that_should_fail) {
  // Create a helper function
  assert_parsed_expression_simplify_to("1>g(x)", "1");

  // Store only works on variables or functions
  assert_expression_not_parsable("2>f(2)");
  assert_expression_not_parsable("3>f(g(4))");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_store_overwrite) {
  assert_parsed_expression_simplify_to("2>g", "2");
  assert_parsed_expression_simplify_to("-1>g(x)", "-1");
  assert_parsed_expression_evaluates_to<double>("g(4)", "-1");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_store_do_not_overwrite) {
  assert_parsed_expression_simplify_to("-1>g(x)", "-1");
  assert_parsed_expression_simplify_to("1+g(x)>f(x)", "1+g(x)");
  assert_parsed_expression_simplify_to("2>g", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("g(4)", "-1");
  assert_parsed_expression_evaluates_to<double>("f(4)", "0");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}
