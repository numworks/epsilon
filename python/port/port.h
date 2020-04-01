#ifndef PYTHON_PORT_H
#define PYTHON_PORT_H

extern "C" {
#include <stddef.h>
}
#include <escher/view_controller.h>

namespace MicroPython {

class ScriptProvider {
public:
  virtual const char * contentOfScript(const char * name) = 0;
};

class ExecutionEnvironment {
public:
  ExecutionEnvironment() {}
  static ExecutionEnvironment * currentExecutionEnvironment();
  void runCode(const char * );
  virtual const char * inputText(const char * prompt) { return nullptr; }

  // Sandbox
  void displaySandbox() { displayViewController(sandbox()); }
  virtual ViewController * sandbox() { return nullptr; }
  virtual void resetSandbox() {}

  // Generic View Controller
  virtual void displayViewController(ViewController * controller) {}
  virtual void hideAnyDisplayedViewController() {}

  virtual void printText(const char * text, size_t length) {}
  virtual void refreshPrintOutput() {}
  void interrupt();
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);
void collectRootsAtAddress(char * address, int len);

}

#endif
