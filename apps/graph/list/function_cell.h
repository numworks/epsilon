#ifndef ESCHER_FUNCTION_CELL_H
#define ESCHER_FUNCTION_CELL_H

#include <apps/i18n.h>
#include <apps/shared/expression_model_list_controller.h>
#include <apps/shared/with_expression_cell.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>

namespace Graph {

class AbstractFunctionCell : public Escher::EvenOddCell {
 public:
  AbstractFunctionCell();
  virtual Escher::HighlightCell* mainCell() = 0;
  const Escher::HighlightCell* mainCell() const {
    return const_cast<Escher::HighlightCell*>(
        const_cast<AbstractFunctionCell*>(this)->mainCell());
  }

  // View
  KDSize minimalSizeForOptimalDisplay() const override;

  // EvenOddCell
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // - Message Text View
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }

  // - Color indicator
  void setColor(KDColor color) { m_functionColor = color; }

 protected:
  // View
  bool displayFunctionType() const;
  int numberOfSubviews() const override { return 2 + displayFunctionType(); }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  constexpr static KDCoordinate k_colorIndicatorThickness =
      Escher::Metric::VerticalColorIndicatorThickness;
  constexpr static KDCoordinate k_margin =
      Shared::ExpressionModelListController::k_defaultVerticalMargin;
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

template <typename T>
class TemplatedFunctionCell : public AbstractFunctionCell, public T {
 public:
  TemplatedFunctionCell() : AbstractFunctionCell(), T() {}

 private:
  Escher::HighlightCell* mainCell() override { return T::expressionCell(); }
};

class FunctionCell
    : public TemplatedFunctionCell<Shared::WithNonEditableExpressionCell> {
 public:
  FunctionCell()
      : TemplatedFunctionCell<Shared::WithNonEditableExpressionCell>(),
        m_parameterSelected(false) {}
  // - Ellipsis View
  void setParameterSelected(bool selected);

 private:
  void updateSubviewsBackgroundAfterChangingState() override;
  bool m_parameterSelected;
};

}  // namespace Graph

#endif
