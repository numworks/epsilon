#include "execution_environment.h"
#include <quiz.h>
#include <assert.h>
#include <string.h>

char TestExecutionEnvironment::s_pythonHeap[TestExecutionEnvironment::s_pythonHeapSize];

void TestExecutionEnvironment::printText(const char * text, size_t length) {
  quiz_print(text);
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

void assert_script_execution_succeeds(const char * script) {
  constexpr size_t bufferSize = 500;
  char buffer[bufferSize];
  inlineToBeSingleInput(buffer, bufferSize, script);
  MicroPython::init(TestExecutionEnvironment::s_pythonHeap, TestExecutionEnvironment::s_pythonHeap + TestExecutionEnvironment::s_pythonHeapSize);
  TestExecutionEnvironment env;
  env.runCode(buffer);
  MicroPython::deinit();
}
