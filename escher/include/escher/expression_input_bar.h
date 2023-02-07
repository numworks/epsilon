#ifndef ESCHER_EXPRESSION_INPUT_BAR_H
#define ESCHER_EXPRESSION_INPUT_BAR_H

#include <escher/expression_field.h>
#include <escher/metric.h>
#include <escher/solid_color_view.h>

namespace Escher {

class AbstractExpressionInputBar : public View {
public:
  AbstractExpressionInputBar();

  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  virtual const ExpressionField * expressionField() const = 0;
  virtual ExpressionField * expressionField() = 0;

protected:
  constexpr static KDCoordinate k_margin = Metric::ExpressionViewMargin;

private:
  constexpr static KDCoordinate k_maximalHeight = 0.6*Ion::Display::Height;
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  KDCoordinate inputViewHeight() const;
  SolidColorView m_line;
};

template <class T>
class TemplatedExpressionInputBar : public AbstractExpressionInputBar {
public:
  TemplatedExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, LayoutFieldDelegate * layoutFieldDelegate) :
    AbstractExpressionInputBar(),
    m_field(parentResponder, inputEventHandler, layoutFieldDelegate)
    {
      expressionField()->setMargins(k_margin);
    }

  const ExpressionField * expressionField() const override { return &m_field; }
  ExpressionField * expressionField() override { return &m_field; }

private:
  T m_field;
};

using ExpressionInputBar = TemplatedExpressionInputBar<ExpressionField>;

}

#endif
