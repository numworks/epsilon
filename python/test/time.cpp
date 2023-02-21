#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_time) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_fails(env, "monotonic()");
  assert_command_execution_succeeds(env, "from time import *");
  assert_command_execution_succeeds(env, "monotonic()");
  assert_command_execution_succeeds(env, "sleep(23)");
  deinit_environment();
}
