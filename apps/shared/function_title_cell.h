#ifndef SHARED_FUNCTION_TITLE_CELL_H
#define SHARED_FUNCTION_TITLE_CELL_H

#include <escher.h>

namespace Shared {

class FunctionTitleCell : public EvenOddCell {
public:
  enum class Orientation {
    HorizontalIndicator,
    VerticalIndicator
  };
  FunctionTitleCell(Orientation orientation = Orientation::VerticalIndicator);
  void setOrientation(Orientation orientation);
  virtual void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  virtual const KDFont * font() const = 0;
protected:
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  Orientation m_orientation;
private:
  KDColor m_functionColor;
};

}

#endif
