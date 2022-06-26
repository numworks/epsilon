#include <quiz.h>
#include "execution_environment.h"

QUIZ_CASE(python_ulab) {
  TestExecutionEnvironment env = init_environnement();
  // Try to import ulab module and submodules
  assert_command_execution_succeeds(env, "import ulab");
  assert_command_execution_succeeds(env, "import ulab as ul");
  assert_command_execution_succeeds(env, "from ulab import *");
  assert_command_execution_succeeds(env, "from ulab import numpy");
  assert_command_execution_succeeds(env, "from ulab import numpy as np");
  assert_command_execution_succeeds(env, "from ulab import scipy");
  assert_command_execution_succeeds(env, "from ulab import scipy as sp");
  assert_command_execution_succeeds(env, "from ulab import scipy as spy");
  // NumPy tests
  assert_command_execution_succeeds(env, "np.array([1, 2, 3])");
  // Store an array in a variable and use it
  assert_command_execution_succeeds(env, "a = np.array([1, 2, 3])");
  assert_command_execution_succeeds(env, "a[0]");
  assert_command_execution_succeeds(env, "a[1]");
  assert_command_execution_succeeds(env, "a[2]");
  assert_command_execution_fails(env, "a[3]");
  // Test np.all
  assert_command_execution_succeeds(env, "np.all([1, 2, 3])");
  // SciPy tests
  // Test ulab.scipy.linalg using spy prefix
  assert_command_execution_succeeds(env, "spy.linalg.solve_triangular(np.array([[1, 2], [3, 4]]), np.array([5, 6]))");
  assert_command_execution_fails(env, "spy.linalg.solve_triangular([[1, 2], [3, 4]], [1, 2, 3])");
  // Test ulab.scipy.optimize using spy prefix
  assert_command_execution_succeeds(env, "spy.optimize.fmin(lambda x: x**2, 1)");
  assert_command_execution_fails(env, "spy.optimize.fmin(lambda x: x**2, 1, maxiter=0)");
  assert_command_execution_succeeds(env, "spy.optimize.fmin(lambda x: x**2, 1, maxiter=1)");
  assert_command_execution_fails(env, "spy.optimize.bisect(lambda x: x**2, 1, 2, maxiter=0)");
  assert_command_execution_succeeds(env, "spy.optimize.newton(lambda x: x**2, 1)");
  assert_command_execution_fails(env, "spy.optimize.newton(lambda x: x**2, 1, maxiter=0)");
  assert_command_execution_succeeds(env, "spy.optimize.newton(lambda x: x**2, 1, maxiter=1)");
  // Test ulab.scipy.signal using spy prefix
  // TODO: Find a way to test this, maybe in a future ulab release ?
  // assert_command_execution_succeeds(env, "spy.signal.sosfilt(np.array([1, 2, 3]), np.array([7, 8, 9]))");
  assert_command_execution_fails(env, "spy.signal.spectrogram(np.array([1, 2, 3]), np.array([7, 8, 9]))");
  // Test ulab.scipy.special using spy prefix
  assert_command_execution_succeeds(env, "spy.special.erf(1)");
  assert_command_execution_fails(env, "spy.special.erf(1, 2)");
  assert_command_execution_succeeds(env, "spy.special.erfc(1)");
  assert_command_execution_fails(env, "spy.special.erfc(1, 2)");
  assert_command_execution_succeeds(env, "spy.special.gamma(1)");
  assert_command_execution_fails(env, "spy.special.gamma(1, 2)");
  assert_command_execution_succeeds(env, "spy.special.gammaln(1)");
  assert_command_execution_fails(env, "spy.special.gammaln(1, 2)");
  deinit_environment();
}

