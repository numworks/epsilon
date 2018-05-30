#ifndef APPS_STATISTICS_CALCULATION_TITLE_CELL_H
#define APPS_STATISTICS_CALCULATION_TITLE_CELL_H

#include <escher/even_odd_message_text_cell.h>

namespace Statistics {

class CalculationTitleCell : public EvenOddMessageTextCell {
public:
  CalculationTitleCell(KDText::FontSize size = KDText::FontSize::Small) :
    EvenOddMessageTextCell(size)
  {}
  void layoutSubviews() override;
private:
  constexpr static KDCoordinate k_rightMargin = 2;
};

}

#endif
