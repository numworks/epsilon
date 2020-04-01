#include <python/port/port.h>
#include <apps/code/app.h>

class TestExecutionEnvironment : public MicroPython::ExecutionEnvironment {
public:
  void printText(const char * text, size_t length) override;
  static constexpr int s_pythonHeapSize = Code::App::k_pythonHeapSize;
  static char s_pythonHeap[s_pythonHeapSize];
};


// TODO: this will be obsolete when runCode will take a parameter to choose the input type
void inlineToBeSingleInput(char * buffer, size_t bufferSize, const char * script);

void assert_script_execution_succeeds(const char * script);
