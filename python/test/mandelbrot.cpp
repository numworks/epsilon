#include <quiz.h>
#include "execution_environment.h"

static const char * s_mandelbrotScript = R"(#
def mandelbrot(N_iteration):
  for x in range(320):
    for y in range(222):
      z = complex(0,0)
      c = complex(3.5*x/319-2.5, -2.5*y/221+1.25)
      i = 0
      while (i < N_iteration) and abs(z) < 2:
        i = i + 1
        z = z*z+c
      rgb = int(255*i/N_iteration)
      col = (int(rgb),int(rgb*0.75),int(rgb*0.25))
mandelbrot(2)
)";

QUIZ_CASE(python_mandelbrot) {
  assert_script_execution_succeeds(s_mandelbrotScript);
}
