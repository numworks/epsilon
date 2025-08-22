#pragma once

#include "elements_data_base.h"

namespace Elements {

class ElementsViewDelegate {
 public:
  virtual void selectedElementHasChanged() = 0;
  virtual void activeDataFieldHasChanged() = 0;
};

}  // namespace Elements
