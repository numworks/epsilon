#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

class StoreCell : public HideableEvenOddEditableTextCell {
public:
  StoreCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr) :
    HideableEvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate),
    m_separatorLeft(false)
  {}
  void setSeparatorLeft(bool separator);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
private:
  static constexpr KDCoordinate k_rightMargin = 2;
  bool m_separatorLeft;
};

}

#endif
