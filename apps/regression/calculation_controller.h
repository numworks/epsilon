#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher.h>

namespace Regression {

class CalculationController : public ViewController, public AlternateEmptyViewDelegate {

public:
  CalculationController(Responder * parentResponder);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
private:
  Responder * tabController() const;
  SolidColorView m_view;
};

}


#endif