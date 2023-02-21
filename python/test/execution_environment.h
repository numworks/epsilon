#include <apps/code/app.h>
#include <python/port/port.h>

class TestExecutionEnvironment : public MicroPython::ExecutionEnvironment {
 public:
  TestExecutionEnvironment() : m_printTextIndex(0) {}
  void printText(const char* text, size_t length) override;
  const char* lastPrintedText() const { return m_printTextBuffer; }

  constexpr static int s_pythonHeapSize = Code::App::k_pythonHeapSize;
  static char s_pythonHeap[s_pythonHeapSize];

 private:
  constexpr static size_t k_maxPrintedTextSize = 256;
  char m_printTextBuffer[k_maxPrintedTextSize];
  size_t m_printTextIndex;
};

TestExecutionEnvironment init_environement();
void deinit_environment();

void assert_script_execution_succeeds(const char* script,
                                      const char* outputText = nullptr);
void assert_script_execution_fails(const char* script);
void assert_command_execution_succeeds(TestExecutionEnvironment env,
                                       const char* line,
                                       const char* outputText = nullptr);
void assert_command_execution_fails(TestExecutionEnvironment env,
                                    const char* line);
