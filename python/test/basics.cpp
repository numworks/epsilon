#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_basics) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "2+3", "5\n");
  deinit_environment();
}

QUIZ_CASE(python_template) {
  assert_script_execution_succeeds(Code::ScriptTemplate::Squares()->content());
  assert_script_execution_succeeds(
      Code::ScriptTemplate::Mandelbrot()->content());
  assert_script_execution_succeeds(
      Code::ScriptTemplate::Polynomial()->content());
  assert_script_execution_succeeds(Code::ScriptTemplate::Parabola()->content());
}
