#ifndef REGRESSION_COLUMN_TITLE_CELL_H
#define REGRESSION_COLUMN_TITLE_CELL_H

#include "even_odd_double_buffer_text_cell_with_separator.h"

namespace Regression {

class ColumnTitleCell : public EvenOddDoubleBufferTextCellWithSeparator {
public:
  ColumnTitleCell(Responder * parentResponder = nullptr) :
    EvenOddDoubleBufferTextCellWithSeparator(parentResponder, 0.5f, 0.5f),
    m_functionColor(Palette::Red)
  {
  }
  virtual void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  KDColor m_functionColor;
};

}

#endif
