#ifndef ESCHER_BOXES_DELEGATE_H
#define ESCHER_BOXES_DELEGATE_H

#include "pervasive_box.h"

namespace Escher {

class BoxesDelegate {
 public:
  virtual PervasiveBox* toolbox() = 0;
  virtual PervasiveBox* variableBox() = 0;
};

}  // namespace Escher

#endif
