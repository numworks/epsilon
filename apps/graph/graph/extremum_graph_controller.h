#ifndef GRAPH_EXTREMUM_GRAPH_CONTROLLER_H
#define GRAPH_EXTREMUM_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"
#include "../../shared/function_banner_delegate.h"
#include "../cartesian_function.h"

namespace Graph {

class ExtremumGraphController : public ViewController, public Shared::FunctionBannerDelegate {
public:
  ExtremumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, I18n::Message defaultMessage);
  View * view() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(CartesianFunction * function);
protected:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  BannerView * bannerView() override { return m_bannerView; }
  void reloadBannerView();
  bool moveCursor(int direction);
  CartesianFunction::Point computeExtremumFromAbscissa(double start, int direction);
  virtual CartesianFunction::Point computeExtremum(double start, double step, double max, Poincare::Context * context) = 0;
  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  CartesianFunction * m_function;
  MessageTextView m_defaultBannerView;
  bool m_isActive;
};

class MinimumGraphController : public ExtremumGraphController {
public:
  MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
private:
  CartesianFunction::Point computeExtremum(double start, double step, double max, Poincare::Context * context) override;
};

}

#endif
