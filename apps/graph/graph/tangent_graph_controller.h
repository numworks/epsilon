#ifndef GRAPH_TANGENT_GRAPH_CONTROLLER_H
#define GRAPH_TANGENT_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "graph_controller_helper.h"
#include "../../shared/simple_interactive_curve_view_controller.h"
#include "../../shared/function_banner_delegate.h"
#include "../cartesian_function_store.h"

namespace Graph {

class TangentGraphController : public Shared::SimpleInteractiveCurveViewController, public Shared::FunctionBannerDelegate, public GraphControllerHelper {
public:
  TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  void viewWillAppear() override;
  void setFunction(CartesianFunction * function);
private:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.22f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::CurveView * curveView() override { return m_graphView; }
  BannerView * bannerView() override { return m_bannerView; };
  void reloadBannerView() override;
  bool moveCursorHorizontally(int direction) override;
  bool handleEnter() override;
  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  CartesianFunction * m_function;
};

}

#endif
