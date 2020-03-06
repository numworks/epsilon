#include "serial_number_controller.h"
#include <ion.h>

namespace HardwareTest {

SerialNumberController::SerialNumberController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_barCodeView()
{
}

void SerialNumberController::viewWillAppear() {
  m_barCodeView.setData(Ion::serialNumber());
}

bool SerialNumberController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OnOff) {
    return false;
  }
  return true;
}

}
