#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../store_controller.h"

namespace Statistics {

class StoreController : public ::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store);
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
};

}

#endif
