#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

class StoreCell : public HideableEvenOddEditableTextCell {
public:
  StoreCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr) :
    HideableEvenOddEditableTextCell(parentResponder, delegate, draftTextBuffer),
    m_separatorRight(false)
  {}
  void setSeparatorRight(bool separator);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
private:
  static constexpr KDCoordinate k_separatorThickness = 2;
  bool m_separatorRight;
};

}

#endif
