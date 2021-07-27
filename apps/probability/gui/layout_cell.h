#ifndef APPS_PROBABILITY_GUI_LAYOUT_CELL_H
#define APPS_PROBABILITY_GUI_LAYOUT_CELL_H

#include <escher/expression_view.h>
#include <escher/table_cell.h>

namespace Probability {

class LayoutCell : public Escher::TableCell {
public:
  const View * labelView() const override { return &m_expressionView; }

  void setLayout(Poincare::Layout layout);
  void setHighlighted(bool highlight) override;

protected:
  KDColor currentBackgroundColor();

private:
  Escher::ExpressionView m_expressionView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LAYOUT_CELL_H */
