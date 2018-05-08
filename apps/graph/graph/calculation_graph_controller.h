#ifndef GRAPH_CALCULATION_GRAPH_CONTROLLER_H
#define GRAPH_CALCULATION_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"
#include "../../shared/function_banner_delegate.h"
#include "../cartesian_function.h"

namespace Graph {

class CalculationGraphController : public ViewController, public Shared::FunctionBannerDelegate {
public:
  CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, I18n::Message defaultMessage);
  View * view() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(CartesianFunction * function);
protected:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  BannerView * bannerView() override { return m_bannerView; }
  virtual void reloadBannerView();
  bool moveCursor(int direction);
  CartesianFunction::Point computeNewPointOfInteresetFromAbscissa(double start, int direction);
  virtual CartesianFunction::Point computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) = 0;
  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  CartesianFunction * m_function;
  MessageTextView m_defaultBannerView;
  bool m_isActive;
};

}

#endif
