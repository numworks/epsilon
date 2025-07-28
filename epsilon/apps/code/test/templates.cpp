#include <python/test/execution_environment.h>
#include <quiz.h>

QUIZ_CASE(code_template) {
  assert_script_execution_succeeds(Code::ScriptTemplate::Squares()->content());
  assert_script_execution_succeeds(
      Code::ScriptTemplate::Mandelbrot()->content());
  assert_script_execution_succeeds(
      Code::ScriptTemplate::Polynomial()->content());
  assert_script_execution_succeeds(Code::ScriptTemplate::Parabola()->content());
}
