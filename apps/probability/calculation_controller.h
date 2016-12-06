#ifndef PROBABILITY_CALCULATION_CONTROLLER_H
#define PROBABILITY_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "law.h"
#include "law_curve_view.h"
#include "image_table_view.h"

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
    void drawRect(KDContext * ctx, KDRect rect) const override;
    LawCurveView * lawCurveView();
    ImageTableView * imageTableView();
  private:
    constexpr static KDCoordinate k_textFieldWidth = 50;
    constexpr static KDCoordinate k_charWidth = 7;
    constexpr static KDCoordinate k_textMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    LawCurveView m_lawCurveView;
    ImageTableView m_imageTableView;
    PointerTextView m_text[3];
    EditableTextCell m_calculationCell[3];
  };
  ContentView m_contentView;
  Law * m_law;
};

}

#endif
