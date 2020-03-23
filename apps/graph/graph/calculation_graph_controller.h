#ifndef GRAPH_CALCULATION_GRAPH_CONTROLLER_H
#define GRAPH_CALCULATION_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "../../shared/simple_interactive_curve_view_controller.h"
#include "../../shared/function_banner_delegate.h"
#include "../continuous_function_store.h"

namespace Graph {

class App;

class CalculationGraphController : public Shared::SimpleInteractiveCurveViewController, public Shared::FunctionBannerDelegate {
public:
  CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, I18n::Message defaultMessage);
  void viewWillAppear() override;
  void setRecord(Ion::Storage::Record record);
protected:
  float cursorBottomMarginRatio() override { return 0.15f; }
  BannerView * bannerView() override { return m_bannerView; }
  void reloadBannerView() override;
  Poincare::Coordinate2D<double> computeNewPointOfInterestFromAbscissa(double start, int direction);
  ContinuousFunctionStore * functionStore() const;
  virtual Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) = 0;
  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Ion::Storage::Record m_record;
  MessageTextView m_defaultBannerView;
  bool m_isActive;
private:
  bool handleEnter() override;
  bool moveCursorHorizontally(int direction, bool fast = false) override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::CurveView * curveView() override { return m_graphView; }
};

}

#endif
