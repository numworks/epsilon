#include "rpi_controller.h"

namespace Rpi {

RpiController::RpiController() :
  ViewController(nullptr),
  m_rpiView()
{
}

View * RpiController::view() {
  return &m_rpiView;
}

}
