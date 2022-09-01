#ifndef FINANCE_DATA_CONTROLLER_H
#define FINANCE_DATA_CONTROLLER_H

#include "data.h"

namespace Finance {

class DataController {
protected:
  DataController(Data * data) : m_data(data) {}
  InterestData * interestData() const { return m_data->interestData(); }
  void setModel(bool selectedModel) { m_data->setModel(selectedModel); }

private:
  Data * m_data;
};

}


#endif