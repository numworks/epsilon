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
  ExecutionEnvironment() : m_displayedViewController(nullptr) {}
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
  void viewControllerWillAppear(ViewController * vc) { m_displayedViewController = vc; }
  void viewControllerDidDisappear(ViewController * vc);

  virtual void printText(const char * text, size_t length) {}
  virtual void refreshPrintOutput() {}
  void interrupt();
protected:
  bool viewControllerIsDisplayed(ViewController * vc) const { return m_displayedViewController == vc; }
  ViewController * m_displayedViewController;
};

class ExecutionViewControllerHelper {
public:
  ExecutionViewControllerHelper(ExecutionEnvironment * executionEnvironment) : m_executionEnvironment(executionEnvironment) {}
  void viewWillAppear(ViewController * vc);
  void viewDidDisappear(ViewController * vc);
private:
  ExecutionEnvironment * m_executionEnvironment;
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);
void collectRootsAtAddress(char * address, int len);

}

#endif
