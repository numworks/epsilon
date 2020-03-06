#ifndef CODE_SANDBOX_CONTROLLER_H
#define CODE_SANDBOX_CONTROLLER_H

#include <escher/responder.h>
#include <escher/solid_color_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <python/port/port.h>

namespace Code {

class SandboxController : public ViewController {
public:
  SandboxController(Responder * parentResponder, MicroPython::ExecutionEnvironment * executionEnvironment);
  StackViewController * stackViewController();
  void reset();
  void hide();

  // ViewController
  View * view() override { return &m_solidColorView; }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }

private:
  void redrawWindow();
  SolidColorView m_solidColorView;
  MicroPython::ExecutionEnvironment * m_executionEnvironment;
};
}

#endif
