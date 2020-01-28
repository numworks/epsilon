#include <quiz.h>
#include <assert.h>
#include <python/port/port.h>


class TestExecutionEnvironment : public MicroPython::ExecutionEnvironment {
public:
  void printText(const char * text, size_t length) override;
};

void TestExecutionEnvironment::printText(const char * text, size_t length) {
  quiz_print(text);
}

static constexpr int k_pythonHeapSize = 16384;
static char s_pythonHeap[k_pythonHeapSize*100];

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
print('ok')
)";

QUIZ_CASE(python_mandelbrot) {
  MicroPython::init(s_pythonHeap, s_pythonHeap + k_pythonHeapSize);
  TestExecutionEnvironment env;
  env.runCode(s_mandelbrotScript);
  MicroPython::deinit();
}
