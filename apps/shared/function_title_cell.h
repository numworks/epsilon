#ifndef SHARED_FUNCTION_TITLE_CELL_H
#define SHARED_FUNCTION_TITLE_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/metric.h>

namespace Shared {

class FunctionTitleCell : public Escher::EvenOddCell {
public:
  enum class Orientation {
    HorizontalIndicator,
    VerticalIndicator
  };
  FunctionTitleCell(Orientation orientation = Orientation::HorizontalIndicator) :
    EvenOddCell(),
    m_orientation(orientation),
    m_baseline(-1),
    m_functionColor(KDColorBlack)
  {}
  virtual void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBaseline(KDCoordinate baseline);
  virtual KDFont::Size font() const = 0;
protected:
  constexpr static KDCoordinate k_separatorThickness = Escher::Metric::CellSeparatorThickness;
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  virtual KDRect subviewFrame() const;
  const Orientation m_orientation;
  KDCoordinate m_baseline;
  KDColor m_functionColor;
};

}

#endif
