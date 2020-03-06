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
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  KDRect subviewFrame() const;
  float verticalAlignment() const;
  Orientation m_orientation;
  KDCoordinate m_baseline;
private:
  constexpr static KDCoordinate k_equalWidthWithMargins = 10; // Ad hoc value
  virtual float verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const { assert(false); return 0; }
  KDColor m_functionColor;
};

}

#endif
