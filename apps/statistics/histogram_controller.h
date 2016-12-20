#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher.h>
#include "data.h"
#include "histogram_view.h"
#include "../curve_view.h"

namespace Statistics {

class HistogramController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {

public:
  HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Data * m_data);
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
  HistogramView m_view;
  Button m_settingButton;
  int m_selectedBin;
  Data * m_data;
};

}


#endif
