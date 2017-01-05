#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "prediction_parameter_controller.h"
#include "../curve_view_with_banner_and_cursor_controller.h"

namespace Regression {

class GraphController : public CurveViewWindowWithBannerAndCursorController {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store);
  ViewController * initialisationParameterController() override;
  void didBecomeFirstResponder() override;
  bool isEmpty() override;
  const char * emptyMessage() override;
private:
  bool handleEnter() override;
  GraphView m_view;
  Store * m_store;
  uint32_t m_storeVersion;
  InitialisationParameterController m_initialisationParameterController;
  PredictionParameterController m_predictionParameterController;
};

}


#endif