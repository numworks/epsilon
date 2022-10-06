#ifndef ESCHER_FUNCTION_CELL_H
#define ESCHER_FUNCTION_CELL_H

#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>

#include <apps/i18n.h>

namespace Graph {

class FunctionCell : public Escher::EvenOddCell {
public:
  FunctionCell();

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  Poincare::Layout layout() const override { return m_expressionView.layout(); }

  // EvenOddCell
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void updateSubviewsBackgroundAfterChangingState() override;

  // - Expression View
  void setTextColor(KDColor textColor) {
    m_expressionView.setTextColor(textColor);
  }
  void setLayout(Poincare::Layout layout) {
    m_expressionView.setLayout(layout);
  }

  // - Ellipsis View
  void setParameterSelected(bool selected);

  // - Message Text View
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }

  // - Color indicator
  void setColor(KDColor color) { m_functionColor = color; }

private:
  // View
  bool displayPlotType() const { return m_messageTextView.text() != nullptr && m_messageTextView.text()[0] != 0; }
  int numberOfSubviews() const override { return 2 + displayPlotType(); }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  constexpr static KDCoordinate k_colorIndicatorThickness = 3;
  constexpr static KDCoordinate k_margin = Escher::Metric::BigCellMargin;
  constexpr static KDCoordinate k_messageMargin =
      Escher::Metric::CellVerticalElementMargin;
  constexpr static KDCoordinate k_parametersColumnWidth =
      Escher::Metric::EllipsisCellWidth;
  Escher::ExpressionView m_expressionView;
  Escher::MessageTextView m_messageTextView;
  Escher::EllipsisView m_ellipsisView;
  KDColor m_functionColor;
  KDColor m_expressionBackground;
  KDColor m_ellipsisBackground;
  bool m_parameterSelected;
};

}  // namespace Graph

#endif
