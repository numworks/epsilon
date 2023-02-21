#include <quiz.h>

#include "execution_environment.h"

QUIZ_CASE(python_matplotlib_pyplot_import) {
  // Test "from matplotlib.pyplot import *"
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_fails(env, "arrow(2,3,4,5)");
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "arrow(2,3,4,5)");
  deinit_environment();

  // "from matplotlib import *"
  env = init_environement();
  assert_command_execution_fails(env, "pyplot.arrow(2,3,4,5)");
  assert_command_execution_succeeds(env, "from matplotlib import *");
  assert_command_execution_succeeds(env, "pyplot.arrow(2,3,4,5)");
  deinit_environment();

  // "import matplotlib"
  env = init_environement();
  assert_command_execution_fails(env, "matplotlib.pyplot.arrow(2,3,4,5)");
  assert_command_execution_succeeds(env, "import matplotlib");
  assert_command_execution_succeeds(env, "matplotlib.pyplot.arrow(2,3,4,5)");
  deinit_environment();

  // "import matplotlib.pyplot"
  env = init_environement();
  assert_command_execution_fails(env, "matplotlib.pyplot.arrow(2,3,4,5)");
  assert_command_execution_succeeds(env, "import matplotlib.pyplot");
  assert_command_execution_succeeds(env, "matplotlib.pyplot.arrow(2,3,4,5)");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_arrow) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "arrow(2,3,4,5)");
  assert_command_execution_fails(env, "arrow(2,3,4,5, 0.1)");
  assert_command_execution_fails(env, "arrow(2,3,4,5, \"width\")");
  assert_command_execution_fails(env, "arrow(2,3,4,5, 0.1, \"#FF00FF\")");
  assert_command_execution_succeeds(env, "arrow(2,3,4,5,head_width=0.3)");
  assert_command_execution_succeeds(env, "arrow(2,3,4,5,color=\"red\")");
  assert_command_execution_succeeds(env, "show()");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_axis) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "axis((2,3,4,5))");
  assert_command_execution_succeeds(env, "axis([2,3,4,5])");
  assert_command_execution_succeeds(env, "axis()", "(2.0, 3.0, 4.0, 5.0)\n");
  assert_command_execution_succeeds(env, "show()");
  assert_command_execution_fails(env, "axis(2,3,4,5)");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_bar) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "bar([0,2,3],[10,12,23])");
  assert_command_execution_succeeds(env, "bar(10,[0,2,3])");
  assert_command_execution_succeeds(env, "bar([0,2,3],10)");
  assert_command_execution_succeeds(env, "bar([1,2,3],[1,2,3],2,3)");
  assert_command_execution_succeeds(env,
                                    "bar([1,2,3],[1,2,3],[1,2,3],[1,2,3])");
  assert_command_execution_succeeds(
      env, "bar([1,2,3],[1,2,3],[1,2,3],[1,2,3], color=\"orange\")");
  assert_command_execution_succeeds(env, "show()");
  assert_command_execution_succeeds(env, "bar([],[])");
  assert_command_execution_succeeds(env, "bar([],[1,2,3])");
  assert_command_execution_succeeds(env, "bar(1,[1,2,3],[1,2],[1,2,3,4])");
  assert_command_execution_fails(env, "bar([1,2,3],[1,2,3,4],[1,2,3],[1,2,3])");
  assert_command_execution_fails(env, "bar(1,[])");
  assert_command_execution_fails(env, "bar(1,[1,2,3],[])");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_grid) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "grid(True)");
  assert_command_execution_succeeds(env, "grid()");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_hist) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "hist([2,3,4,5,6])");
  assert_command_execution_succeeds(env, "hist([2,3,4,5,6],23)");
  assert_command_execution_succeeds(env, "hist([2,3,4,5,6],[0,2,3])");
  assert_command_execution_succeeds(env, "hist([2,3,4,5,6],[0,2,3, 4,5,6,7])");
  assert_command_execution_succeeds(
      env, "hist([2,3,4,5,6],[0,2,3, 4,5,6,7], color=(0,255,0))");
  assert_command_execution_succeeds(env, "show()");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_plot) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "plot([2,3,4,5,6])");
  assert_command_execution_succeeds(env, "plot(2,3)");
  assert_command_execution_succeeds(env, "plot([2,3,4,5,6],[3,4,5,6,7])");
  assert_command_execution_succeeds(
      env, "plot([2,3,4,5,6],[3,4,5,6,7], color=\"g\")");
  assert_command_execution_succeeds(env, "show()");
  assert_command_execution_fails(env, "plot([2,3,4,5,6],2)");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_scatter) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "scatter(2,3)");
  assert_command_execution_succeeds(env, "scatter([2,3,4,5,6],[3,4,5,6,7])");
  assert_command_execution_succeeds(
      env, "scatter([2,3,4,5,6],[3,4,5,6,7], color=(0,0,255))");
  assert_command_execution_succeeds(env, "show()");
  assert_command_execution_fails(env, "scatter([2,3,4,5,6],2)");
  assert_command_execution_fails(env, "scatter(2)");
  assert_command_execution_succeeds(env, "scatter(2,3,4)");
  deinit_environment();
}

QUIZ_CASE(python_matplotlib_pyplot_text) {
  TestExecutionEnvironment env = init_environement();
  assert_command_execution_succeeds(env, "from matplotlib.pyplot import *");
  assert_command_execution_succeeds(env, "text(2,3,'hello')");
  assert_command_execution_succeeds(env, "show()");
  deinit_environment();
}
