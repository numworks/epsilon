#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "hideable_even_odd_editable_text_cell.h"
#include "store_separator_cell.h"

namespace Shared {

class StoreCell : public HideableEvenOddEditableTextCell, public StoreSeparatorCell {
public:
  StoreCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr) :
    HideableEvenOddEditableTextCell(parentResponder, delegate, draftTextBuffer),
    StoreSeparatorCell()
  {}
  void setSeparatorRight(bool separator) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
};

}

#endif
