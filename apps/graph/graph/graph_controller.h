#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "../../shared/function_graph_controller.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphController : public Shared::FunctionGraphController {
public:
  GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, HeaderViewController * header);
  const char * emptyMessage() override;
private:
  BannerView * bannerView() override;
  void reloadBannerView() override;
  bool moveCursorHorizontally(int direction) override;
  void initCursorParameters() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override;
  CartesianFunctionStore * functionStore() const override;
  GraphView * functionGraphView() override;
  CurveParameterController * curveParameterController() override;
  BannerView m_bannerView;
  GraphView m_view;
  Shared::InteractiveCurveViewRange m_graphRange;
  CurveParameterController m_curveParameterController;
  CartesianFunctionStore * m_functionStore;
};

}

#endif
