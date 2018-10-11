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

QUIZ_CASE(poincare_store_user_variable) {
  // Fill variable
  assert_parsed_expression_simplify_to("1+2>Adadas", "Adadas");
  assert_parsed_expression_simplify_to("Adadas", "3");

  // Fill f1
  assert_parsed_expression_simplify_to("1+x>f1(x)", "f1(x)");
  assert_parsed_expression_simplify_to("f1(4)", "5");
  assert_parsed_expression_simplify_to("f1(Adadas)", "4");

  // Fill f2
  assert_parsed_expression_simplify_to("x-1>f2(x)", "f2(x)");
  assert_parsed_expression_simplify_to("f2(4)", "3");
  assert_parsed_expression_simplify_to("f2(Adadas)", "2");

  // Define funcBoth with f1 and f2
  assert_parsed_expression_simplify_to("f1(x)+f2(x)>funcBoth(x)", "funcBoth(x)");
  assert_parsed_expression_simplify_to("funcBoth(4)", "4");
  assert_parsed_expression_simplify_to("funcBoth(Adadas)", "3");

  // Change f2
  assert_parsed_expression_simplify_to("x>f2(x)", "f2(x)");
  assert_parsed_expression_simplify_to("f2(4)", "4");
  assert_parsed_expression_simplify_to("f2(Adadas)", "3");

  // Make sure funcBoth has changed
  assert_parsed_expression_simplify_to("funcBoth(4)", "5");
  assert_parsed_expression_simplify_to("funcBoth(Adadas)", "4");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("Adadas.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f1.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f2.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("funcBoth.func").destroy();
}
