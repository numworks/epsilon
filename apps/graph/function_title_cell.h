#ifndef GRAPH_FUNCTION_TITLE_CELL_H
#define GRAPH_FUNCTION_TITLE_CELL_H

#include <escher.h>

namespace Graph {
class FunctionTitleCell : public EvenOddCell {
public:
  enum class Orientation {
    HorizontalIndicator,
    VerticalIndicator
  };
	FunctionTitleCell();
  void setColor(KDColor color);
  void setOrientation(Orientation orientation);
  void setText(const char * textContent);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reloadCell() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
 private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  KDColor m_functionColor;
  BufferTextView m_bufferTextView;
  Orientation m_orientation;
};

}

#endif
