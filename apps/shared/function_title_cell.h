#ifndef SEQUENCE_FUNCTION_TITLE_CELL_H
#define SEQUENCE_FUNCTION_TITLE_CELL_H

#include <escher.h>

namespace Shared {

class FunctionTitleCell : public EvenOddCell {
public:
  enum class Orientation {
    HorizontalIndicator,
    VerticalIndicator
  };
  FunctionTitleCell(Orientation orientation, KDText::FontSize size = KDText::FontSize::Large);
  void setColor(KDColor color);
  void setText(const char * textContent);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  EvenOddBufferTextCell m_bufferTextView;
private:
  KDColor m_functionColor;
  Orientation m_orientation;
};

}

#endif
