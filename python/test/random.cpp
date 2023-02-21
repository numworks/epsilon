#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_random) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_fails(env, "random()");
  assert_command_execution_succeeds(env, "from random import *");
  assert_command_execution_succeeds(env, "random()");
  assert_command_execution_succeeds(env, "getrandbits(23)");
  deinit_environment();
}
