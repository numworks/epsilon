#ifndef PROBABILITY_CALCULATION_CONTROLLER_H
#define PROBABILITY_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "law.h"
#include "law_curve_view.h"

namespace Probability {

class CalculationController : public ViewController {
public:
  CalculationController(Responder * parentResponder, Law * law);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, Law * law);
    void layoutSubviews() override;
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    LawCurveView m_lawCurveView;
  };
  ContentView m_contentView;
  Law * m_law;
};

}

#endif
