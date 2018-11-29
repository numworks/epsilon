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
  FunctionTitleCell(Orientation orientation = Orientation::VerticalIndicator) :
    EvenOddCell(),
    m_orientation(orientation),
    m_baseline(-1),
    m_functionColor(KDColorBlack)
  {}
  virtual void setOrientation(Orientation orientation);
  virtual void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBaseline(KDCoordinate baseline);
  virtual const KDFont * font() const = 0;
protected:
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  Orientation m_orientation;
  KDCoordinate m_baseline;
private:
  KDColor m_functionColor;
};

}

#endif
