#ifndef CODE_EXECUTOR_CONTROLLER_H
#define CODE_EXECUTOR_CONTROLLER_H

#include <escher.h>
#include "program.h"

namespace Code {

class ExecutorController : public ViewController {
public:
  ExecutorController(Program * program);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public View {
  public:
    ContentView(Program * program);
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    void runPython() const;
    Program * m_program;
  };
  ContentView m_view;
};

}

#endif

