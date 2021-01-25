#ifndef ESCHER_TABLE_CELL_H
#define ESCHER_TABLE_CELL_H

#include <escher/bordered.h>
#include <escher/highlight_cell.h>

namespace Escher {

class TableCell : public Bordered, public HighlightCell {
public:
  TableCell();
  virtual View * labelView() const { return nullptr; }
  virtual View * subLabelView() const { return nullptr; }
  virtual View * accessoryView() const { return nullptr; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  static KDCoordinate minimalHeightForOptimalDisplay(View * label, View * subLabel, View * accessory, KDCoordinate width);
  static constexpr KDCoordinate k_minimalLargeFontCellHeight = 35; // = Metric::CellSeparatorThickness + Metric::CellTopMargin + KDFont::LargeFont->glyphSize().height() + Metric::CellTopMargin;
  static constexpr KDCoordinate k_minimalSmallFontCellHeight = 31; // = Metric::CellSeparatorThickness + Metric::CellTopMargin + KDFont::SmallFont->glyphSize().height() + Metric::CellTopMargin;
protected:
  virtual KDColor backgroundColor() const { return KDColorWhite; }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  virtual bool isAcessoryAlignedRight() const { return true; }
};

}
#endif
