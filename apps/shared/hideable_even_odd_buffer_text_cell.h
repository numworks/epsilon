#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_BUFFER_TEXT_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/palette.h>
#include "hideable.h"

namespace Shared {

class HideableEvenOddBufferTextCell : public Escher::EvenOddBufferTextCell, public Hideable {
public:
  HideableEvenOddBufferTextCell() :
    Escher::EvenOddBufferTextCell(),
    Hideable()
  {}
  KDColor backgroundColor() const override;
  void setHide(bool hide) override;
  void reinit() override { setText(""); }
};

}

#endif
