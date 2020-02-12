#include <quiz.h>
#include <assert.h>
#include <string.h>
#include <python/port/port.h>
#include <apps/code/app.h>


class TestExecutionEnvironment : public MicroPython::ExecutionEnvironment {
public:
  void printText(const char * text, size_t length) override;
};

void TestExecutionEnvironment::printText(const char * text, size_t length) {
  quiz_print(text);
}

static constexpr int s_pythonHeapSize = Code::App::k_pythonHeapSize;
static char s_pythonHeap[s_pythonHeapSize];

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

// TODO: this will be obsolete when runCode will take a parameter to choose the input type

void inlineToBeSingleInput(char * buffer, size_t bufferSize, const char * script) {
  static const char * openExec = "exec(\"";
  static const char * closeExec = "\")";
  assert(strlen(script) + strlen(openExec) + strlen(closeExec) < bufferSize);
  char * bufferChar = buffer;
  bufferChar += strlcpy(buffer, openExec, bufferSize);
  const char * scriptChar = script;
  while (*scriptChar != 0) {
    assert(bufferChar - buffer + 2 < bufferSize - 1);
    if (*scriptChar == '\n') {
      // Turn carriage return in {'\', 'n'} to be processed by exec
      *bufferChar++ = '\\';
      *bufferChar++ = 'n';
    } else {
      *bufferChar++ = *scriptChar;
    }
    scriptChar++;
  }
  bufferChar += strlcpy(bufferChar, closeExec, buffer + bufferSize - bufferChar);
  assert(bufferChar - buffer < bufferSize);
  *bufferChar = 0;
}

QUIZ_CASE(python_mandelbrot) {
  constexpr size_t bufferSize = 500;
  char buffer[bufferSize];
  inlineToBeSingleInput(buffer, bufferSize, s_mandelbrotScript);
  MicroPython::init(s_pythonHeap, s_pythonHeap + s_pythonHeapSize);
  TestExecutionEnvironment env;
  env.runCode(buffer);
  MicroPython::deinit();
}
