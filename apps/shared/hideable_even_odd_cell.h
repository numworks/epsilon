#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_CELL_H

#include <escher/even_odd_cell.h>
#include "hideable.h"

namespace Shared {

class HideableEvenOddCell : public EvenOddCell, public Hideable {
public:
  HideableEvenOddCell() :
    EvenOddCell(),
    Hideable()
  {}
  KDColor backgroundColor() const override;
  void setHide(bool hide) override;
};

}

#endif
