#pragma once

#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/text_format.h>
#include <escher/theme/components.h>

namespace Escher {

class EvenOddCell : public HighlightCell {
 public:
  constexpr static ThemeComponent k_hideColor = ThemeComponent::WallScreenDark;
  EvenOddCell();
  virtual void setEven(bool even);
  void setVisible(bool visible) override;
  void setHighlighted(bool highlighted) override;
  /* The background color defines one color for even cells, one for odd cells
   * and one for invisible cells. The highlight color is resolved separately at
   * the drawing step by calling the Highlight method. */
  virtual ThemeComponent backgroundColor() const;
  /* By default, an EvenOddCell is filled with its even / odd / hide background
   * color, applying highlight */
  void drawRect(KDContext* ctx, KDRect rect) const override;

  constexpr static KDCoordinate k_horizontalMargin = Metric::SmallCellMargin;
  constexpr static TextFormat k_smallCellDefaultFormat = {
      .style = {.font = KDFont::Size::Small},
      .horizontalAlignment = KDGlyph::k_alignRight};

 protected:
  virtual ThemeComponent baseColor() const {
    return m_even ? ThemeComponent::CellEven : ThemeComponent::CellOdd;
  }
  virtual void updateSubviewsBackgroundAfterChangingState() {}
  bool m_even;
};

}  // namespace Escher
