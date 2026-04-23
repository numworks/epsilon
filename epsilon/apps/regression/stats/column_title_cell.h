#pragma once

#include <escher/theme/data_colors.h>

#include "even_odd_double_buffer_text_cell.h"

namespace Regression {

class ColumnTitleCell : public EvenOddDoubleBufferTextCell {
 public:
  ColumnTitleCell(Escher::Responder* parentResponder = nullptr)
      : EvenOddDoubleBufferTextCell(
            parentResponder, {.style = {.font = KDFont::Size::Small},
                              .horizontalAlignment = KDGlyph::k_alignCenter}) {}
  virtual void setColor(Escher::ThemeComponent color);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;

 private:
  constexpr static KDCoordinate k_colorIndicatorThickness =
      Escher::Metric::HorizontalColorIndicatorThickness;
  Escher::ThemeComponent m_functionColor = Escher::DataColorComponent(0);
};

}  // namespace Regression
