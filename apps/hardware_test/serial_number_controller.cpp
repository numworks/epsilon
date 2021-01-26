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
  Ion::serialNumber(m_barCodeView.data());
  m_barCodeView.reloadData();
}

bool SerialNumberController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OnOff) {
    return false;
  }
  return true;
}

}
