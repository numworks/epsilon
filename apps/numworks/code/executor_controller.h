#ifndef CODE_EXECUTOR_CONTROLLER_H
#define CODE_EXECUTOR_CONTROLLER_H

#include <escher.h>
#include "program.h"
#include "python_stdout_catcher.h"

namespace Code {

class ExecutorController : public ViewController {
public:
  ExecutorController(Program * program);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  class ContentView : public View, PythonStdoutCatcher {
  public:
    ContentView(Program * program);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void print(const char * str) const override;
  private:
    void runPython() const;
    void clearScreen(KDContext * ctx) const;
    Program * m_program;
    mutable KDPoint m_printLocation;
  };
private:
  ContentView m_view;
};

}

#endif

