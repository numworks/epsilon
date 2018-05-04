#include "serial_number_controller.h"
#include <ion.h>

namespace HardwareTest {

SerialNumberController::SerialNumberController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_barCodeView()
{
}

void SerialNumberController::viewWillAppear() {
  static char serialNumber[24];
  Ion::getSerialNumber(serialNumber);
  m_barCodeView.setData(serialNumber);
}

bool SerialNumberController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OnOff || event == Ion::Events::OK) {
    return false;
  }
  return true;
}

}
