#ifndef HARDWARE_TEST_COLOR_CONTROLLER_H
#define HARDWARE_TEST_COLOR_CONTROLLER_H

#include <escher.h>
#include "color_view.h"

namespace HardwareTest {

class ColorController : public ViewController {
public:
  ColorController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  static KDColor nextColor(KDColor color);
  ColorView m_view;
};

}

#endif

