#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_kandinsky_import) {
  // Test "from kandinsky import *"
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_fails(env, "get_pixel(0,0)");
  assert_command_execution_succeeds(env, "from kandinsky import *");
  assert_command_execution_succeeds(env, "get_pixel(0,0)");
  deinit_environment();

  // "import kandinsky"
  env = init_environement();
  assert_command_execution_fails(env, "kandinsky.get_pixel(0,0)");
  assert_command_execution_succeeds(env, "import kandinsky");
  assert_command_execution_succeeds(env, "kandinsky.get_pixel(0,0)");
  deinit_environment();
}

QUIZ_CASE(python_kandinsky_basics) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from kandinsky import *");
  assert_command_execution_succeeds(env, "set_pixel(0,0,color(12,12,12))");
  assert_command_execution_succeeds(env, "get_pixel(0,0)");
  assert_command_execution_succeeds(env, "fill_rect(0,0,10,10,color(2,3,2))");
  assert_command_execution_succeeds(env, "draw_string('hello',0,0)");
  deinit_environment();
}
