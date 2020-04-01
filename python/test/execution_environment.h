#include <python/port/port.h>
#include <apps/code/app.h>

class TestExecutionEnvironment : public MicroPython::ExecutionEnvironment {
public:
  TestExecutionEnvironment() : m_printTextIndex(0) {}
  void printText(const char * text, size_t length) override;
  const char * lastPrintedText() const { return m_printTextBuffer; }

  static constexpr int s_pythonHeapSize = Code::App::k_pythonHeapSize;
  static char s_pythonHeap[s_pythonHeapSize];
private:
  static constexpr size_t k_maxPrintedTextSize = 256;
  char m_printTextBuffer[k_maxPrintedTextSize];
  size_t m_printTextIndex;
};


// TODO: this will be obsolete when runCode will take a parameter to choose the input type
void inlineToBeSingleInput(char * buffer, size_t bufferSize, const char * script);

void assert_script_execution_succeeds(const char * script, const char * outputText = nullptr);
void assert_script_execution_fails(const char * script);
