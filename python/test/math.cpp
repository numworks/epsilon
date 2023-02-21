#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_math) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from math import *");
  assert_command_execution_succeeds(env, "e", "2.718281828459045\n");
  assert_command_execution_succeeds(env, "gamma(3)", "2.0\n");
  deinit_environment();
}

QUIZ_CASE(python_cmath) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from cmath import *");
  assert_command_execution_succeeds(env, "cos(0)", "(1+-0j)\n");
  deinit_environment();
}
