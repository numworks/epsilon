#ifndef CODE_SANDBOX_CONTROLLER_H
#define CODE_SANDBOX_CONTROLLER_H

#include <escher/responder.h>
#include <escher/solid_color_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

namespace Code {

class SandboxController : public ViewController {
public:
  SandboxController(Responder * parentResponder);
  StackViewController * stackViewController();

  // ViewController
  View * view() override { return &m_solidColorView; }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }

private:
  SolidColorView m_solidColorView;
};
}

#endif
