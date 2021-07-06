#include "serial_number_controller.h"
#include <ion.h>

using namespace Escher;

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
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  return true;
}

}
