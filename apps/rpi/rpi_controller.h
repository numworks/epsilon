#ifndef APPS_RPI_CONTROLLER_H
#define APPS_RPI_CONTROLLER_H

#include <escher.h>
#include "rpi_view.h"

namespace Rpi {

class RpiController : public ViewController {
public:
  RpiController();
  View * view() override;
private:
  RpiView m_rpiView;
};

}

#endif
