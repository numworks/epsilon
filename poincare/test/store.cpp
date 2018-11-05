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
  assert_parsed_expression_simplify_to("1+2>A", "3");
  assert_parsed_expression_simplify_to("1+2>x", "3");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("A.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(poincare_store_user_variable) {
  // Fill variable
  assert_parsed_expression_simplify_to("1+2>Adadas", "3");
  assert_parsed_expression_simplify_to("Adadas", "3");

  // Fill f1
  assert_parsed_expression_simplify_to("1+x>f1(x)", "1+?");
  assert_parsed_expression_simplify_to("f1(4)", "5");
  assert_parsed_expression_simplify_to("f1(Adadas)", "4");

  // Fill f2
  assert_parsed_expression_simplify_to("x-1>f2(x)", "(-1)+?");
  assert_parsed_expression_simplify_to("f2(4)", "3");
  assert_parsed_expression_simplify_to("f2(Adadas)", "2");

  // Define fBoth with f1 and f2
  assert_parsed_expression_simplify_to("f1(x)+f2(x)>fBoth(x)", "f1(?)+f2(?)");
  assert_parsed_expression_simplify_to("fBoth(4)", "8");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "6");

  // Change f2
  assert_parsed_expression_simplify_to("x>f2(x)", "?");
  assert_parsed_expression_simplify_to("f2(4)", "4");
  assert_parsed_expression_simplify_to("f2(Adadas)", "3");

  // Make sure fBoth has changed
  assert_parsed_expression_simplify_to("fBoth(4)", "9");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "7");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("Adadas.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f1.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f2.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("funcBoth.func").destroy();
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
  assert_parsed_expression_simplify_to("-1>g(x)", "-1");
  assert_parsed_expression_simplify_to("1+g(x)>f(x)", "1+g(?)");
  assert_parsed_expression_simplify_to("2>g", "2");
  assert_parsed_expression_evaluates_to<double>("g(4)", "undef");
  assert_parsed_expression_evaluates_to<double>("f(4)", "undef");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.exp").destroy();
}
