#ifndef FINANCE_DATA_CONTROLLER_H
#define FINANCE_DATA_CONTROLLER_H

#include "data.h"

namespace Finance {

class DataController {
protected:
  InterestData * interestData() const;
  void setModel(bool selectedModel);
};

}


#endif