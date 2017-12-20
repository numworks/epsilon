#ifndef PROBABILITY_CALCULATION_CONTROLLER_H
#define PROBABILITY_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "law/law.h"
#include "law_curve_view.h"
#include "calculation_view.h"
#include "calculation/calculation.h"
#include "../shared/parameter_text_field_delegate.h"

namespace Probability {

class CalculationController : public ViewController {
public:
  CalculationController(Responder * parentResponder, Law * law, Calculation * calculation);
  View * view() override;
  const char * title() override;
  void reload();
  void reloadLawCurveView();
  void setCalculationAccordingToIndex(int index, bool forceReinitialisation = false);
  void viewWillAppear() override;
  void didEnterResponderChain(Responder * previousResponder) override;
  void didBecomeFirstResponder() override;
private:
  void updateTitle();
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, CalculationController * calculationController, Calculation * Calculation, Law * law);
    LawCurveView * lawCurveView() {
      return &m_lawCurveView;
    }
    CalculationView * calculationView() {
      return &m_calculationView;
    }
  private:
    constexpr static KDCoordinate k_titleHeightMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_titleView;
    CalculationView m_calculationView;
    LawCurveView m_lawCurveView;
  };
  ContentView m_contentView;
  Calculation * m_calculation;
  Law * m_law;
  constexpr static int k_maxNumberOfTitleCharacters = 30;
  char m_titleBuffer[k_maxNumberOfTitleCharacters];
};

}

#endif
