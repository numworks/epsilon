#ifndef ESCHER_FUNCTION_CELL_H
#define ESCHER_FUNCTION_CELL_H

#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>

#include <apps/i18n.h>

namespace Graph {

class AbstractFunctionCell : public Escher::EvenOddCell {
public:
  AbstractFunctionCell();

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  Poincare::Layout layout() const override { return expressionView()->layout(); }

  // EvenOddCell
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void updateSubviewsBackgroundAfterChangingState() override;

  // - Expression View
  void setTextColor(KDColor textColor) {
    expressionView()->setTextColor(textColor);
  }
  void setLayout(Poincare::Layout layout) {
    expressionView()->setLayout(layout);
  }

  // - Ellipsis View
  void setParameterSelected(bool selected);

  // - Message Text View
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }

  // - Color indicator
  void setColor(KDColor color) { m_functionColor = color; }

protected:
  // View
  bool displayFunctionType() const { return m_messageTextView.text() != nullptr && m_messageTextView.text()[0] != 0; }
  int numberOfSubviews() const override { return 2 + displayFunctionType(); }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  virtual const Escher::ExpressionView * expressionView() const = 0;
  virtual Escher::ExpressionView * expressionView() = 0;
  virtual Escher::View * mainView() = 0;

  constexpr static KDCoordinate k_colorIndicatorThickness = 3;
  constexpr static KDCoordinate k_margin = Escher::Metric::BigCellMargin;
  constexpr static KDCoordinate k_messageMargin =
      Escher::Metric::CellVerticalElementMargin;
  constexpr static KDCoordinate k_parametersColumnWidth =
      Escher::Metric::EllipsisCellWidth;
  Escher::MessageTextView m_messageTextView;
  Escher::EllipsisView m_ellipsisView;
  KDColor m_functionColor;
  KDColor m_expressionBackground;
  KDColor m_ellipsisBackground;
  bool m_parameterSelected;
};

class FunctionCell : public AbstractFunctionCell {
private:
  const Escher::ExpressionView * expressionView() const override { return &m_expressionView; }
  Escher::ExpressionView * expressionView() override { return &m_expressionView; }
  Escher::ExpressionView * mainView() override { return &m_expressionView; }
  Escher::ExpressionView m_expressionView;
};

}

#endif
