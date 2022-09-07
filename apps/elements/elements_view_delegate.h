#ifndef PERDIODIC_ELEMENTS_VIEW_DELEGATE_H
#define PERDIODIC_ELEMENTS_VIEW_DELEGATE_H

#include "elements_data_base.h"

namespace Elements {

class ElementsViewDelegate {
public:
  virtual void selectedElementHasChanged() = 0;
  virtual void activeDataFieldHasChanged() = 0;
};

}

#endif
