#include <quiz.h>
#include "execution_environment.h"

QUIZ_CASE(python_basics) {
  TestExecutionEnvironment env = init_environnement();
  assert_command_execution_succeeds(env, "2+3","5\n");
  deinit_environment();
}

// "base" scripts to test.
QUIZ_CASE(python_template) {

}
