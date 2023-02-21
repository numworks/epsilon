#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_ion_import) {
  // Test "from ion import *"
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_fails(env, "keydown(KEY_LEFT)");
  assert_command_execution_succeeds(env, "from ion import *");
  assert_command_execution_succeeds(env, "keydown(KEY_LEFT)");
  deinit_environment();

  // "import ion"
  env = init_environement();
  assert_command_execution_fails(env, "ion.keydown(ion.KEY_LEFT)");
  assert_command_execution_succeeds(env, "import ion");
  assert_command_execution_succeeds(env, "ion.keydown(ion.KEY_LEFT)");
  deinit_environment();
}

QUIZ_CASE(python_ion_keydown) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from ion import *");
  assert_command_execution_succeeds(env, "keydown(KEY_LEFT)", "False\n");
  deinit_environment();
}
