#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_basics) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "5+3", "8\n");
  assert_command_execution_succeeds(env, "5*3", "15\n");
  assert_command_execution_succeeds(env, "5/3", "1.666666666666667\n");
  assert_command_execution_succeeds(env, "5%3", "2\n");
  assert_command_execution_succeeds(env, "5**3", "125\n");
  assert_command_execution_succeeds(env, "3&5", "1\n");
  assert_command_execution_succeeds(env, "3^5", "6\n");
  assert_command_execution_succeeds(env, "3|5", "7\n");
  deinit_environment();
}
QUIZ_CASE(python_built_in) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "abs(-27)", "27\n");
  assert_command_execution_succeeds(env, "bin(27)", "\'0b11011\'\n");
  assert_command_execution_succeeds(env, "complex(3,4)", "(3+4j)\n");
  assert_command_execution_succeeds(env, "eval(\"3+2\")", "5\n");
  assert_command_execution_succeeds(env, "float(3)", "3.0\n");
  assert_command_execution_succeeds(env, "hex(27)", "\'0x1b\'\n");
  assert_command_execution_succeeds(env, "int(3.4)", "3\n");
  assert_command_execution_succeeds(env, "len([3,4,5])", "3\n");
  assert_command_execution_succeeds(env, "len(range(3,9))", "6\n");
  assert_command_execution_succeeds(env, "max([3,6,1])", "6\n");
  assert_command_execution_succeeds(env, "min([3,6,1])", "1\n");
  assert_command_execution_succeeds(env, "oct(27)", "\'0o33\'\n");
  assert_command_execution_succeeds(env, "pow(2,4)", "16\n");
  assert_command_execution_succeeds(env, "print(\"Hello world !\")",
                                    "Hello world !\n");
  assert_command_execution_succeeds(env, "round(0.1254,2)", "0.13\n");
  assert_command_execution_succeeds(env, "sorted([3,6,1])", "[1, 3, 6]\n");
  assert_command_execution_succeeds(env, "sum([3,6,1])", "10\n");
  // List functions
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.append(2); print(a);",
                                    "[3, 6, 1, 2]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.clear(); print(a);",
                                    "[]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.count(3)", "1\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.index(1)", "2\n");
  assert_command_execution_succeeds(
      env, "a = [3,6,1]; a.insert(1,2); print(a);", "[3, 2, 6, 1]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.pop(); print(a);",
                                    "1\n[3, 6]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.remove(3); print(a);",
                                    "[6, 1]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.reverse(); print(a);",
                                    "[1, 6, 3]\n");
  assert_command_execution_succeeds(env, "a = [3,6,1]; a.sort(); print(a);",
                                    "[1, 3, 6]\n");
  deinit_environment();
}

QUIZ_CASE(python_fixes) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_fails(env, "'abcd'*2**62");
  deinit_environment();
}
