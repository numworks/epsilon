#ifndef STATISTICS_DATA_CONTROLLER_H
#define STATISTICS_DATA_CONTROLLER_H

#include <escher.h>
#include "data.h"
#include "../data_controller.h"

namespace Statistics {

class DataController : public ::DataController {
public:
  DataController(Responder * parentResponder, Data * m_data);
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
};

}

#endif
