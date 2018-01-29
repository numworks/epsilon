#ifndef CALCULATION_OUTPUT_EXPRESSIONS_VIEW_H
#define CALCULATION_OUTPUT_EXPRESSIONS_VIEW_H

#include <escher.h>

namespace Calculation {

class OutputExpressionsView : public EvenOddCell, public Responder {
public:
  enum class SubviewType {
    ExactOutput,
    ApproximativeOutput
  };
  OutputExpressionsView(Responder * parentResponder);
  void setExpressions(Poincare::ExpressionLayout ** expressionsLayout);
  void setEqualMessage(I18n::Message equalSignMessage);
  KDColor backgroundColor() const override;
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  void reloadCell() override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  SubviewType selectedSubviewType();
  void setSelectedSubviewType(SubviewType subviewType);
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  constexpr static KDCoordinate k_digitHorizontalMargin = 10;
  ExpressionView m_approximateExpressionView;
  MessageTextView m_approximateSign;
  ExpressionView m_exactExpressionView;
  SubviewType m_selectedSubviewType;
};

}

#endif
