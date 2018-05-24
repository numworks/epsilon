#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/palette.h>

namespace Shared {

class HideableEvenOddEditableTextCell : public EvenOddEditableTextCell {
public:
  HideableEvenOddEditableTextCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr) :
    EvenOddEditableTextCell(parentResponder, delegate, draftTextBuffer),
    m_hide(false)
  {}
  KDColor backgroundColor() const override;
  static KDColor hideColor() { return Palette::WallScreenDark; }
  void setHide(bool hide);
private:
  bool m_hide;
};

}

#endif
