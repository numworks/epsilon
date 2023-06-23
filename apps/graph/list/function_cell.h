#ifndef ESCHER_FUNCTION_CELL_H
#define ESCHER_FUNCTION_CELL_H

#include <apps/i18n.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/layout_view.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>

namespace Graph {

class AbstractFunctionCell : public Escher::EvenOddCell {
 public:
  AbstractFunctionCell();

  constexpr static KDCoordinate k_colorIndicatorThickness =
      Escher::Metric::VerticalColorIndicatorThickness;

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  Poincare::Layout layout() const override { return layoutView()->layout(); }

  // EvenOddCell
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // - Layout View
  void setTextColor(KDColor textColor) {
    layoutView()->setTextColor(textColor);
  }
  void setLayout(Poincare::Layout layout) { layoutView()->setLayout(layout); }

  // - Message Text View
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }

  // - Color indicator
  void setColor(KDColor color) { m_functionColor = color; }

 protected:
  // View
  bool displayFunctionType() const {
    return m_messageTextView.text() != nullptr &&
           m_messageTextView.text()[0] != 0;
  }
  int numberOfSubviews() const override { return 2 + displayFunctionType(); }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  virtual const Escher::LayoutView* layoutView() const = 0;
  virtual Escher::LayoutView* layoutView() = 0;
  virtual Escher::View* mainView() = 0;

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
};

class FunctionCell : public AbstractFunctionCell {
 public:
  FunctionCell() : m_parameterSelected(false) {}
  // - Ellipsis View
  void setParameterSelected(bool selected);

 private:
  void updateSubviewsBackgroundAfterChangingState() override;
  const Escher::LayoutView* layoutView() const override {
    return &m_layoutView;
  }
  Escher::LayoutView* layoutView() override { return &m_layoutView; }
  Escher::LayoutView* mainView() override { return &m_layoutView; }
  Escher::LayoutView m_layoutView;
  bool m_parameterSelected;
};

}  // namespace Graph

#endif
