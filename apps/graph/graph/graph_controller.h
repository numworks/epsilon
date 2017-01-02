#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "graph_window.h"
#include "curve_parameter_controller.h"
#include "initialisation_parameter_controller.h"
#include "../../curve_view_with_banner_and_cursor_controller.h"
#include "../function_store.h"

namespace Graph {
class GraphController : public CurveViewWindowWithBannerAndCursorController {
public:
  GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  View * view() override;
  void didBecomeFirstResponder() override;
  ViewController * initialisationParameterController() override;

  bool isEmpty() override;
  const char * emptyMessage() override;

private:
  bool handleEnter() override;
  GraphView m_view;
  GraphWindow m_graphWindow;
  InitialisationParameterController m_initialisationParameterController;
  CurveParameterController m_curveParameterController;
  FunctionStore * m_functionStore;
};

}

#endif
