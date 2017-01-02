#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "data.h"
#include "graph_view.h"
#include "../window_parameter_controller.h"
#include "../zoom_parameter_controller.h"

namespace Regression {

class GraphController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;

  ViewController * windowParameterController();
  ViewController * zoomParameterController();
private:
  Responder * tabController() const;
  StackViewController * stackController() const;
  GraphView m_view;
  WindowParameterController m_windowParameterController;
  ZoomParameterController m_zoomParameterController;
  Button m_windowButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
  Data * m_data;
};

}


#endif