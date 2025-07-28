#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_random) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_fails(env, "random()");
  assert_command_execution_succeeds(env, "from random import *");
  assert_command_execution_succeeds(env, "seed(4)");
  assert_command_execution_succeeds(env, "choice([3,6,1])", "3\n");
  assert_command_execution_succeeds(env, "getrandbits(23)", "1603556\n");
  assert_command_execution_succeeds(env, "random()", "0.2300363681499624\n");
  assert_command_execution_succeeds(env, "randint(3,4)", "4\n");
  assert_command_execution_succeeds(env, "randrange(0,5)", "0\n");
  assert_command_execution_succeeds(env, "uniform(0.3,2.1)",
                                    "1.509734470828188\n");
  deinit_environment();
}
