#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "data.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "prediction_parameter_controller.h"
#include "../curve_view_with_banner_and_cursor_controller.h"

namespace Regression {

class GraphController : public CurveViewWindowWithBannerAndCursorController {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data);
  ViewController * initialisationParameterController() override;

  bool isEmpty() override;
  const char * emptyMessage() override;
private:
  bool handleEnter() override;
  GraphView m_view;
  Data * m_data;
  InitialisationParameterController m_initialisationParameterController;
  PredictionParameterController m_predictionParameterController;
};

}


#endif