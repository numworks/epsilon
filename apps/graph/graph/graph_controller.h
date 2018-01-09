#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "graph_controller_helper.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "../../shared/function_graph_controller.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphController : public Shared::FunctionGraphController, public GraphControllerHelper {
public:
  GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Expression::AngleUnit * angleUnitVersion, ButtonRowController * header);
  const char * title() override;
  I18n::Message emptyMessage() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setType(GraphView::Type type);
  bool displayDerivativeInBanner() const;
  void setDisplayDerivativeInBanner(bool displayDerivative);
private:
  GraphView::Type type() const;
  BannerView * bannerView() override;
  void reloadBannerView() override;
  bool moveCursorHorizontally(int direction) override;
  void initCursorParameters() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override;
  CartesianFunctionStore * functionStore() const override;
  GraphView * functionGraphView() override;
  CurveParameterController * curveParameterController() override;
  StackViewController * stackController() const override;
  BannerView m_bannerView;
  GraphView m_view;
  Shared::InteractiveCurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  CartesianFunctionStore * m_functionStore;
  bool m_displayDerivativeInBanner;
};

}

#endif
