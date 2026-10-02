#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_kandinsky_import) {
  // Test "from kandinsky import *"
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_fails(env, "get_pixel(0,0)");
  assert_command_execution_succeeds(env, "from kandinsky import *");
  assert_command_execution_succeeds(env, "get_pixel(0,0)");
  deinit_environment();

  // "import kandinsky"
  env = init_environment();
  assert_command_execution_fails(env, "kandinsky.get_pixel(0,0)");
  assert_command_execution_succeeds(env, "import kandinsky");
  assert_command_execution_succeeds(env, "kandinsky.get_pixel(0,0)");
  deinit_environment();
}

QUIZ_CASE(python_kandinsky_basics) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "from kandinsky import *");
  assert_command_execution_succeeds(env, "set_pixel(0,0,color(12,12,12))");
  assert_command_execution_succeeds(env, "get_pixel(0,0)");
  assert_command_execution_succeeds(env, "fill_rect(0,0,10,10,color(2,3,2))");
  assert_command_execution_succeeds(env, "draw_string('hello',0,0)");
  assert_command_execution_succeeds(
      env, "fill_triangle(0,0,10,0,0,10,color(2,3,2))");
  assert_command_execution_fails(env, "fill_triangle(0,0,10,0,0,10)");
  deinit_environment();
}

QUIZ_CASE(python_kandinsky_fill_triangle) {
  TestExecutionEnvironment env = init_environment();
  assert_command_execution_succeeds(env, "from kandinsky import *");
  assert_command_execution_succeeds(env, "red = color(255,0,0)");
  assert_command_execution_succeeds(env,
                                    "fill_triangle(10,10,40,10,10,40,red)");
  // Vertex order does not matter
  assert_command_execution_succeeds(env,
                                    "fill_triangle(10,40,10,10,40,10,red)");
  // Flat and degenerate triangles
  assert_command_execution_succeeds(env, "fill_triangle(5,5,50,5,20,5,red)");
  assert_command_execution_succeeds(env, "fill_triangle(5,5,5,5,5,5,red)");
  // Partially or fully off-screen triangles
  assert_command_execution_succeeds(
      env, "fill_triangle(-100,-100,500,50,50,900,red)");
  assert_command_execution_succeeds(
      env, "fill_triangle(-50,-50,-20,-10,-30,-5,red)");
  // Wrong number of arguments
  assert_command_execution_fails(env, "fill_triangle(0,0,10,0,0,10)");
  deinit_environment();
}
