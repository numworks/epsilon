#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "axis_interval.h"
#include "axis_parameter_controller.h"
#include "initialisation_parameter_controller.h"
#include "zoom_parameter_controller.h"
#include "../function_store.h"

namespace Graph {
class GraphController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {
public:
  GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  ViewController * axisParameterController();
  ViewController * zoomParameterController();
  ViewController * initialisationParameterController();

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;

private:
  Responder * tabController() const;
  StackViewController * stackController() const;
  GraphView m_view;
  bool m_headerSelected;
  AxisInterval m_axisInterval;
  AxisParameterController m_axisParameterController;
  ZoomParameterController m_zoomParameterController;
  InitialisationParameterController m_initialisationParameterController;
  Button m_axisButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
  FunctionStore * m_functionStore;
};

}

#endif
