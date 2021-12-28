#include <quiz.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/test/helper.h>
#include <float.h>
#include <cmath>

using namespace Poincare;

void assert_regularized_incomplete_beta_function_is(double a, double b, double x, double result) {
  double r = RegularizedIncompleteBetaFunction(a, b, x);
  assert_roughly_equal(r, result, 10E-6);
}

QUIZ_CASE(regularized_incomplete_beta_function) {
  assert_regularized_incomplete_beta_function_is(1.0, 2.0, 0.0, 0.0);
  assert_regularized_incomplete_beta_function_is(1.0, 2.0, 1.0, 1.0);
  assert_regularized_incomplete_beta_function_is(1.7, 0.9, 0.3, 0.114276013523787293056995598423812417112640756984394176432);
  assert_regularized_incomplete_beta_function_is(7.3, 3.9, 0.4, 0.042393671346062170259328642902287422849467242046012782022);
  assert_regularized_incomplete_beta_function_is(128.4, 31.5, 0.8, 0.446264065069106243051390524472702916228137487657780205030);
}
