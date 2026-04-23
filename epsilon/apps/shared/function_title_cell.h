#pragma once

#include <escher/even_odd_cell.h>
#include <escher/metric.h>
#include <escher/theme/components.h>

namespace Shared {

class FunctionTitleCell : public Escher::EvenOddCell {
 public:
  using EvenOddCell::EvenOddCell;
  virtual void setColor(Escher::ThemeComponent color);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
  constexpr static KDCoordinate k_separatorThickness =
      Escher::Metric::CellSeparatorThickness;
  constexpr static KDCoordinate k_colorIndicatorThickness =
      Escher::Metric::HorizontalColorIndicatorThickness;

  virtual KDRect subviewFrame() const;

  KDCoordinate m_baseline = -1;
  Escher::ThemeComponent m_functionColor = Escher::ThemeComponent::DataRed;
};

}  // namespace Shared
