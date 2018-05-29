#ifndef PYTHON_PORT_H
#define PYTHON_PORT_H

extern "C" {
#include <stddef.h>
}

namespace MicroPython {

class ScriptProvider {
public:
  virtual const char * contentOfScript(const char * name) = 0;
};

class ExecutionEnvironment {
public:
  ExecutionEnvironment();
  static ExecutionEnvironment * currentExecutionEnvironment();
  void runCode(const char * );
  virtual const char * inputText(const char * prompt) {
    return nullptr;
  }
  virtual void displaySandbox() {
  }
  virtual void printText(const char * text, size_t length) {
  }
protected:
  bool m_sandboxIsDisplayed;
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);

};

#endif
