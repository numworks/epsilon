#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>

namespace Regression {

class GraphController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController);
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
  Button m_windowButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
};

}


#endif