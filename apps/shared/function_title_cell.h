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
  virtual void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  Orientation m_orientation;
private:
  KDColor m_functionColor;
};

}

#endif
