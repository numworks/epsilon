#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_math) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "from math import *");
  assert_command_execution_succeeds(env, "e", "2.718281828459045\n");
  // clang-format off
  /* TODO: Restore all this tests after unifying all maths.h functions across
           platforms.
  assert_command_execution_succeeds(env, "pi", "3.141592653589793\n");
  assert_command_execution_succeeds(env, "acos(0.5)", "1.047197551196598\n");
  assert_command_execution_succeeds(env, "acosh(2)", "1.316957896924817\n");
  assert_command_execution_succeeds(env, "asin(0.5)", "0.5235987755982988\n");
  assert_command_execution_succeeds(env, "asinh(2)", "1.44363547517881\n");
  assert_command_execution_succeeds(env, "atan(0.5)", "0.4636476090008061\n");
  assert_command_execution_succeeds(env, "atan2(1,2)", "0.4636476090008061\n");
  assert_command_execution_succeeds(env, "atanh(0.5)", "0.5493061443340549\n");
  assert_command_execution_succeeds(env, "ceil(0.3)", "1\n");
  assert_command_execution_succeeds(env, "copysign(-3,5)", "3.0\n");
  assert_command_execution_succeeds(env, "cos(0.5)", "0.8775825618903728\n");
  assert_command_execution_succeeds(env, "cosh(2)", "3.762195691083631\n");
  assert_command_execution_succeeds(env, "degrees(pi/2)", "90.0\n");
  assert_command_execution_succeeds(env, "erf(1)", "0.8427007929497147\n");
  assert_command_execution_succeeds(env, "erfc(1)", "0.1572992070502852\n");
  assert_command_execution_succeeds(env, "exp(3)", "20.08553692318767\n");
  assert_command_execution_succeeds(env, "expm1(3)", "19.08553692318767\n");
  assert_command_execution_succeeds(env, "fabs(-3.4)", "3.4\n");
  assert_command_execution_succeeds(env, "factorial(3)", "6\n");
  assert_command_execution_succeeds(env, "floor(0.3)", "0\n");
  assert_command_execution_succeeds(env, "fmod(6.4,2)", "0.4000000000000004\n");
  assert_command_execution_succeeds(env, "frexp(0.4)", "(0.8, -1)\n");
  assert_command_execution_succeeds(env, "gamma(3)", "2.0\n");
  assert_command_execution_succeeds(env, "isfinite(3)", "True\n");
  assert_command_execution_succeeds(env, "isinf(3)", "False\n");
  assert_command_execution_succeeds(env, "isnan(3)", "False\n");
  assert_command_execution_succeeds(env, "ldexp(0.8,-1)", "0.4\n");
  assert_command_execution_succeeds(env, "lgamma(3)", "0.6931471805599454\n");
  assert_command_execution_succeeds(env, "log(e**3,e)", "3.0\n");
  assert_command_execution_succeeds(env, "log10(100)", "2.0\n");
  assert_command_execution_succeeds(env, "log2(16)", "4.0\n");
  assert_command_execution_succeeds(env, "modf(5.4)",
                                    "(0.4000000000000004, 5.0)\n");
  assert_command_execution_succeeds(env, "pow(2,4)", "16.0\n");
  assert_command_execution_succeeds(env, "radians(90)", "1.570796326794897\n");
  assert_command_execution_succeeds(env, "sin(0.5)", "0.479425538604203\n");
  assert_command_execution_succeeds(env, "sinh(2)", "3.626860407847019\n");
  assert_command_execution_succeeds(env, "sqrt(9)", "3.0\n");
  assert_command_execution_succeeds(env, "tan(0.5)", "0.5463024898437904\n");
  assert_command_execution_succeeds(env, "tanh(2)", "0.9640275800758168\n");
  assert_command_execution_succeeds(env, "trunc(4.5)", "4\n"); */
  // clang-format on

  assert_command_execution_succeeds(env, "gcd(18, 12)", "6\n");
  assert_command_execution_succeeds(env, "gcd(-4, -2)", "2\n");
  assert_command_execution_succeeds(
      env, "gcd(987654321987654321987654321, 123456789123456789123456789)",
      "9000000009000000009\n");
  assert_command_execution_succeeds(env, "gcd()", "0\n");
  deinit_environment();
}

QUIZ_CASE(python_cmath) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "from cmath import *");
  assert_command_execution_succeeds(env, "cos(0)", "(1+-0j)\n");
  // clang-format off
  /* TODO: Restore all this tests after unifying all maths.h functions across
           platforms.
  assert_command_execution_succeeds(env, "cos(4)",
                                    "(-0.6536436208636119+0j)\n");
  assert_command_execution_succeeds(
      env, "exp(5+4j)", "(-97.00931469961551-112.3194491453625j)\n");
  assert_command_execution_succeeds(env, "phase(5+4j)", "0.6747409422235526\n");
  assert_command_execution_succeeds(
      env, "polar(5+4j)", "(6.403124237432849, 0.6747409422235526)\n");
  assert_command_execution_succeeds(
      env, "log(5+4j)", "(1.856786033352154+0.6747409422235526j)\n");
  // FIXME
  // assert_command_execution_succeeds(env, "log(5+4j, 2)", ???);
  assert_command_execution_succeeds(env, "rect(5,pi/4)",
                                    "(3.535533905932738+3.535533905932737j)\n");
  assert_command_execution_succeeds(env, "sin(4)",
                                    "(-0.7568024953079282+-0j)\n");
  assert_command_execution_succeeds(
      env, "sqrt(5+4j)", "(2.387794404616198+0.8375930507808816j)\n");
  */
  // clang-format on
  deinit_environment();
}
