#ifndef ESCHER_EXPRESSION_INPUT_BAR_H
#define ESCHER_EXPRESSION_INPUT_BAR_H

#include <escher/expression_field.h>
#include <escher/metric.h>
#include <escher/solid_color_view.h>

namespace Escher {

class ExpressionInputBar : public ExpressionField {
public:
  ExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, LayoutFieldDelegate * layoutFieldDelegate);

  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override { return 1 + ExpressionField::numberOfSubviews(); }
  View * subviewAtIndex(int index) override;
  KDSize minimalSizeForOptimalDisplay() const override;

private:
  constexpr static KDCoordinate k_maximalHeight = 0.6*Ion::Display::Height;
  constexpr static KDCoordinate k_margin = Metric::ExpressionViewMargin;
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  KDCoordinate inputViewHeight() const override;
  SolidColorView m_line;
};

}

#endif
