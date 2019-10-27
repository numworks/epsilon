#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/palette.h>
#include "hideable.h"

namespace Shared {

class HideableEvenOddEditableTextCell : public EvenOddEditableTextCell, public Hideable {
public:
  HideableEvenOddEditableTextCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr) :
    EvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate, KDFont::SmallFont),
    Hideable()
  {}
  KDColor backgroundColor() const override;
  void setHide(bool hide) override;
  void reinit() override { editableTextCell()->textField()->setText(""); }
};

}

#endif
