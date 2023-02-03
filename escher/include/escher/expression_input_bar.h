#ifndef ESCHER_EXPRESSION_INPUT_BAR_H
#define ESCHER_EXPRESSION_INPUT_BAR_H

#include <escher/expression_field.h>
#include <escher/metric.h>

namespace Escher {

class ExpressionInputBar : public ExpressionField {
public:
  ExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, LayoutFieldDelegate * layoutFieldDelegate);

  using ExpressionField::layoutSubviews;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

private:
  constexpr static KDCoordinate k_minimalHeight = 37;
  constexpr static KDCoordinate k_maximalHeight = 0.6*Ion::Display::Height;
  constexpr static KDCoordinate k_horizontalMargin = 5;
  constexpr static KDCoordinate k_verticalMargin = 5;
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  KDCoordinate inputViewHeight() const override;
};

}

#endif
