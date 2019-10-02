#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "hideable_even_odd_editable_text_cell.h"
#include "separable.h"

namespace Shared {

class StoreCell : public HideableEvenOddEditableTextCell, public Separable {
public:
  StoreCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr) :
    HideableEvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate),
    Separable()
  {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
private:
  static constexpr KDCoordinate k_rightMargin = Metric::CellMargin;
  void didSetSeparator() override;
};

}

#endif
