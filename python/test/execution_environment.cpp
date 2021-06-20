#include "execution_environment.h"
#include <quiz.h>
#include <assert.h>
#include <string.h>
extern "C" {
  #include <py/objtuple.h>
}

char TestExecutionEnvironment::s_pythonHeap[TestExecutionEnvironment::s_pythonHeapSize];

void TestExecutionEnvironment::printText(const char * text, size_t length) {
  assert(m_printTextIndex + length < k_maxPrintedTextSize);
  strlcpy(m_printTextBuffer + m_printTextIndex, text, length + 1);
  m_printTextIndex += length;
  m_printTextBuffer[m_printTextIndex] = 0;
}

// TODO: this will be obsolete when runCode will take a parameter to choose the input type

void inlineToBeSingleInput(char * buffer, size_t bufferSize, const char * script) {
  static const char * openExec = "exec(\"";
  static const char * closeExec = "\")";
  assert(strlen(script) + strlen(openExec) + strlen(closeExec) < bufferSize);
  char * bufferChar = buffer;
  bufferChar += strlcpy(buffer, openExec, bufferSize);
  const char * scriptChar = script;
  while (*scriptChar != 0) {
    assert(bufferChar - buffer + 2 < (int)bufferSize - 1);
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
  assert(bufferChar - buffer < (int)bufferSize);
  *bufferChar = 0;
}

bool execute_input(TestExecutionEnvironment env, bool singleCommandLine, const char * input, const char * outputText = nullptr) {
  constexpr size_t bufferSize = 1000;
  char buffer[bufferSize];
  if (!singleCommandLine) {
    inlineToBeSingleInput(buffer, bufferSize, input);
    input = buffer;
  }
  bool executionResult = env.runCode(input);
  if (outputText) {
    quiz_assert(strcmp(outputText, env.lastPrintedText()) == 0);
  }
  return executionResult;
}

TestExecutionEnvironment init_environement() {
  MicroPython::init(TestExecutionEnvironment::s_pythonHeap, TestExecutionEnvironment::s_pythonHeap + TestExecutionEnvironment::s_pythonHeapSize);
 return TestExecutionEnvironment();
}

void deinit_environment() {
  MicroPython::deinit();
}

void assert_script_execution_result(bool expectedResult, const char * script, const char * outputText = nullptr) {
  TestExecutionEnvironment env = init_environement();
  quiz_assert(expectedResult == execute_input(env, false, script, outputText));
  deinit_environment();
}

void assert_script_execution_succeeds(const char * script, const char * outputText) {
  assert_script_execution_result(true, script, outputText);
}

void assert_script_execution_fails(const char * script) {
  assert_script_execution_result(false, script);
}

void assert_command_execution_result(bool expectedResult, TestExecutionEnvironment env, const char * line, const char * outputText = nullptr) {
  quiz_assert(execute_input(env, true,line, outputText) == expectedResult);
}

void assert_command_execution_succeeds(TestExecutionEnvironment env, const char * line, const char * outputText) {
  assert_command_execution_result(true, env, line, outputText);
}

void assert_command_execution_fails(TestExecutionEnvironment env, const char * line) {
  assert_command_execution_result(false, env, line);
}

void assert_color_from_mp_obj_t(mp_obj_t obj, KDColor color, MicroPython::ColorParser::ColorMode colorMode){
  quiz_assert(MicroPython::ColorParser::ParseColor(obj, colorMode) == color);
}

void assert_color_from_str(const char * str, KDColor color, MicroPython::ColorParser::ColorMode colorMode){
  assert_color_from_mp_obj_t(mp_obj_new_str(str, strlen(str)), color, colorMode);
}

mp_obj_t floatTuple(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
  mp_obj_tuple_t * t = static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
  t->items[0] = r;
  t->items[1] = g;
  t->items[2] = b;
  return MP_OBJ_FROM_PTR(t);
}
