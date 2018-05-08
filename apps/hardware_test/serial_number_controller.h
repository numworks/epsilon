#ifndef HARDWARE_TEST_SERIAL_NUMBER_CONTROLLER_H
#define HARDWARE_TEST_SERIAL_NUMBER_CONTROLLER_H

#include <escher.h>
#include "code_128b_view.h"

namespace HardwareTest {

class SerialNumberController : public ViewController {
public:
  SerialNumberController(Responder * parentResponder);
  View * view() override { return &m_barCodeView; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  Code128BView m_barCodeView;
};

}

#endif

