#ifndef ESCHER_TABLE_CELL_H
#define ESCHER_TABLE_CELL_H

#include <escher/bordered.h>
#include <escher/highlight_cell.h>

class TableCell : public Bordered, public HighlightCell {
public:
  /* Layout enum class determines the way subviews are layouted.
   * We can split the cell vertically or horizontally.
   * We can choose which subviews frames are optimized (if there is not enough
   * space for all subviews, which one is cropped). This case happens so far only
   * for horizontally splitted cell, so we distinguish only these sub cases.
   * TODO: implement VerticalTopOverlap, VerticalBottomlap? */
  enum class Layout {
    Vertical,
    HorizontalLeftOverlap, // Label overlaps on SubAccessory which overlaps on Accessory
    HorizontalRightOverlap, // Reverse
  };
  TableCell(Layout layout = Layout::HorizontalLeftOverlap);
  virtual View * labelView() const;
  virtual View * accessoryView() const;
  virtual View * subAccessoryView() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  virtual KDCoordinate labelMargin() const { return Metric::TableCellHorizontalMargin; }
  virtual KDCoordinate accessoryMargin() const { return Metric::TableCellHorizontalMargin; }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  constexpr static KDCoordinate k_verticalMargin = Metric::TableCellVerticalMargin;
  constexpr static KDCoordinate k_horizontalMargin = Metric::TableCellHorizontalMargin;
private:
  Layout m_layout;
};

#endif

