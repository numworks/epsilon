#pragma once

#include "data_view_controller.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleDataViewController : public DataViewController {
 public:
  using DataViewController::DataViewController;
  virtual MultipleDataView* multipleDataView() = 0;

  // DataViewController
  DataView* dataView() override { return multipleDataView(); }
};

}  // namespace Statistics
