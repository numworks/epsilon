#ifndef APPS_SHARED_STORE_CELL_H
#define APPS_SHARED_STORE_CELL_H

#include "separable.h"
#include <escher/even_odd_editable_text_cell.h>

namespace Shared {

class StoreCell : public Escher::EvenOddEditableTextCell, public Separable {
public:
  StoreCell(Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * delegate = nullptr) :
    EvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate, KDFont::Size::Small),
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
