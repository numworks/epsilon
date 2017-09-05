#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "apps/shared/function_graph_controller.h"
#include "apps/shared/curve_view_cursor.h"
#include "apps/shared/interactive_curve_view_range.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphController : public Shared::FunctionGraphController {
public:
  GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Expression::AngleUnit * angleUnitVersion, ButtonRowController * header);
  I18n::Message emptyMessage() override;
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
  Shared::InteractiveCurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  CartesianFunctionStore * m_functionStore;
};

}

#endif
