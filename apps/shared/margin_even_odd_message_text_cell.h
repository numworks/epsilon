#ifndef APPS_SHARED_MARGIN_EVEN_ODD_MESSAGE_TEXT_CELL_H
#define APPS_SHARED_MARGIN_EVEN_ODD_MESSAGE_TEXT_CELL_H

#include <escher/even_odd_message_text_cell.h>

namespace Shared {

class MarginEvenOddMessageTextCell : public EvenOddMessageTextCell {
public:
  MarginEvenOddMessageTextCell(const KDFont * font = KDFont::SmallFont) :
    EvenOddMessageTextCell(font)
  {}
  void layoutSubviews() override;
private:
  constexpr static KDCoordinate k_rightMargin = 2;
};

}

#endif
