#include "execution_environment.h"
#include <quiz.h>
#include <assert.h>
#include <string.h>

char TestExecutionEnvironment::s_pythonHeap[TestExecutionEnvironment::s_pythonHeapSize];

void TestExecutionEnvironment::printText(const char * text, size_t length) {
  assert(m_printTextIndex + length < k_maxPrintedTextSize);
  m_printTextIndex += strlcpy(m_printTextBuffer + m_printTextIndex, text, length + 1);
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

bool execute_input(const char * input, bool singleCommandLine, const char * outputText = nullptr) {
  constexpr size_t bufferSize = 500;
  char buffer[bufferSize];
  if (!singleCommandLine) {
    inlineToBeSingleInput(buffer, bufferSize, input);
    input = buffer;
  }
  MicroPython::init(TestExecutionEnvironment::s_pythonHeap, TestExecutionEnvironment::s_pythonHeap + TestExecutionEnvironment::s_pythonHeapSize);
  TestExecutionEnvironment env;
  bool executionResult = env.runCode(input);
  MicroPython::deinit();
  if (outputText) {
    quiz_assert(strcmp(outputText, env.lastPrintedText()) == 0);
  }
  return executionResult;
}

void assert_script_execution_succeeds(const char * script, const char * outputText) {
  quiz_assert(execute_input(script, false, outputText));
}

void assert_script_execution_fails(const char * script) {
  quiz_assert(!execute_input(script, false));
}

void assert_command_execution_succeeds(const char * line, const char * outputText) {
  quiz_assert(execute_input(line, true, outputText));
}
