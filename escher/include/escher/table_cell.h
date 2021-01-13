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
protected:
  virtual KDColor backgroundColor() const { return KDColorWhite; }
  // virtual KDCoordinate labelMargin() const { return k_horizontalMargin; }
  // virtual KDCoordinate accessoryMargin() const { return k_horizontalMargin; }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  // constexpr static KDCoordinate k_verticalMargin = Metric::TableCellVerticalMargin;
  // constexpr static KDCoordinate k_horizontalMargin = Metric::TableCellHorizontalMargin;
};

}
#endif
