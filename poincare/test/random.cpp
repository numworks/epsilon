#include <quiz.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/random.h>
#include <assert.h>
#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_random_simplify) {
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4", "1/random()+7/12");
  assert_parsed_expression_simplify_to("random()+random()", "random()+random()");
  assert_parsed_expression_simplify_to("random()-random()", "-random()+random()");
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4+1/random()", "1/random()+1/random()+7/12");
  assert_parsed_expression_simplify_to("random()×random()", "random()×random()");
  assert_parsed_expression_simplify_to("random()/random()", "random()/random()");
  assert_parsed_expression_simplify_to("3^random()×3^random()", "3^random()×3^random()");
  assert_parsed_expression_simplify_to("random()×ln(2)×3+random()×ln(2)×5", "5×ln(2)×random()+3×ln(2)×random()");
}

QUIZ_CASE(poincare_randint_simplify) {
  assert_parsed_expression_simplify_to("1/randint(3,10)+1/3+1/4", "1/randint(3,10)+7/12");
  assert_parsed_expression_simplify_to("randint(3,10)+randint(3,10)", "randint(3,10)+randint(3,10)");
  assert_parsed_expression_simplify_to("randint(3,10)-randint(3,10)", "-randint(3,10)+randint(3,10)");
  assert_parsed_expression_simplify_to("1/randint(3,10)+1/3+1/4+1/randint(3,10)", "1/randint(3,10)+1/randint(3,10)+7/12");
  assert_parsed_expression_simplify_to("randint(3,10)×randint(3,10)", "randint(3,10)×randint(3,10)");
  assert_parsed_expression_simplify_to("randint(3,10)/randint(3,10)", "randint(3,10)/randint(3,10)");
  assert_parsed_expression_simplify_to("3^randint(3,10)×3^randint(3,10)", "3^randint(3,10)×3^randint(3,10)");
  assert_parsed_expression_simplify_to("randint(3,10)×ln(2)×3+randint(3,10)×ln(2)×5", "5×ln(2)×randint(3,10)+3×ln(2)×randint(3,10)");
}
