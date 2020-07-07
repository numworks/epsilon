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

TestExecutionEnvironment init_environement();
void deinit_environment();

void assert_script_execution_succeeds(const char * script, const char * outputText = nullptr);
void assert_script_execution_fails(const char * script);
void assert_command_execution_succeeds(TestExecutionEnvironment env, const char * line, const char * outputText = nullptr);
void assert_command_execution_fails(TestExecutionEnvironment env, const char * line);

void assert_color_from_mp_obj_t(mp_obj_t obj, KDColor color, MicroPython::ColorParser::ColorMode colorMode = MicroPython::ColorParser::ColorMode::MaxIntensity255);
void assert_color_from_str(const char * str, KDColor color, MicroPython::ColorParser::ColorMode colorMode = MicroPython::ColorParser::ColorMode::MaxIntensity255);
mp_obj_t floatTuple(mp_obj_t r, mp_obj_t g, mp_obj_t b);
