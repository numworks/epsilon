#ifndef CODE_SANDBOX_CONTROLLER_H
#define CODE_SANDBOX_CONTROLLER_H

#include <escher/responder.h>
#include <escher/solid_color_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <python/port/port.h>

namespace Code {

class SandboxController : public Escher::ViewController {
public:
  SandboxController(Escher::Responder * parentResponder);
  Escher::StackViewController * stackViewController();
  void reset();

  // ViewController
  Escher::View * view() override { return &m_solidColorView; }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event & event) override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return Escher::ViewController::TitlesDisplay::DisplayNoTitle; }

private:
  void redrawWindow();
  Escher::SolidColorView m_solidColorView;
};
}

#endif
