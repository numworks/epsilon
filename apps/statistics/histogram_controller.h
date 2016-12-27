#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher.h>

namespace Statistics {

class HistogramController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {

public:
  HistogramController(Responder * parentResponder, HeaderViewController * headerViewController);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
private:
  Responder * tabController() const;
  SolidColorView m_view;
  Button m_settingButton;
  int m_selectedBin;
};

}


#endif
