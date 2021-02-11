#ifndef ESCHER_TABLE_CELL_H
#define ESCHER_TABLE_CELL_H

#include <escher/bordered.h>
#include <escher/highlight_cell.h>

namespace Escher {

class TableCell : public Bordered, public HighlightCell {
public:
  TableCell();
  virtual const View * labelView() const { return nullptr; }
  virtual const View * subLabelView() const { return nullptr; }
  virtual const View * accessoryView() const { return nullptr; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  static KDCoordinate minimalHeightForOptimalDisplay(const View * label, const View * subLabel, const View * accessory, KDCoordinate width);
  static constexpr KDCoordinate k_minimalLargeFontCellHeight = Metric::CellSeparatorThickness + Metric::CellTopMargin + 18 + Metric::CellTopMargin; // KDFont::LargeFont->glyphSize().height() = 18
  static constexpr KDCoordinate k_minimalSmallFontCellHeight = Metric::CellSeparatorThickness + Metric::CellTopMargin + 14 + Metric::CellTopMargin; // KDFont::SmallFont->glyphSize().height() = 14
protected:
  virtual KDColor backgroundColor() const { return KDColorWhite; }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  virtual bool isAccessoryAlignedRight() const { return true; }
};

}
#endif
