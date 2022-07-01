#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "hideable_even_odd_editable_text_cell.h"
#include "separable.h"

namespace Shared {

class StoreCell : public HideableEvenOddEditableTextCell, public Separable {
public:
  StoreCell(Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * delegate = nullptr) :
    HideableEvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate),
    Separable()
  {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_rightMargin = Escher::Metric::SmallCellMargin;
  void didSetSeparator() override;
};

}

#endif
