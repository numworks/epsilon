#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/palette.h>
#include "hideable.h"

namespace Shared {

class HideableEvenOddEditableTextCell : public Escher::EvenOddEditableTextCell, public Hideable {
public:
  HideableEvenOddEditableTextCell(Escher::Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * delegate = nullptr) :
    Escher::EvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate, KDFont::Size::Small),
    Hideable()
  {}
  KDColor backgroundColor() const override;
  void setHide(bool hide) override;
  void reinit() override { editableTextCell()->textField()->setText(""); }
};

}

#endif
