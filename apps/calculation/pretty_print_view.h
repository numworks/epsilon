#ifndef CALCULATION_PRETTY_PRINT_VIEW_H
#define CALCULATION_PRETTY_PRINT_VIEW_H

#include <escher.h>

namespace Calculation {

class PrettyPrintView : public ScrollView, public Responder {
public:
  PrettyPrintView(Responder * parentResponder);
  void setExpression(ExpressionLayout * expressionLayout);
  void layoutSubviews() override;
  void setBackgroundColor(KDColor backgroundColor);
  bool handleEvent(Ion::Events::Event event) override;
  KDSize minimalSizeForOptimalDisplay() override;
private:
  ExpressionView m_expressionView;
  KDCoordinate m_manualScrolling;
};

}

#endif
