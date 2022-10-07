#ifndef GRAPH_CALCULATION_GRAPH_CONTROLLER_H
#define GRAPH_CALCULATION_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "../continuous_function_store.h"
#include <apps/shared/simple_interactive_curve_view_controller.h>
#include <apps/shared/function_banner_delegate.h>

namespace Graph {

class App;

class CalculationGraphController : public Shared::SimpleInteractiveCurveViewController, public Shared::FunctionBannerDelegate {
public:
  CalculationGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, I18n::Message defaultMessage);

  bool handleEvent(Ion::Events::Event event) override;
  bool handleEnter() override;
  void viewWillAppear() override;

  void setRecord(Ion::Storage::Record record);

protected:
  float cursorBottomMarginRatio() override { return cursorBottomMarginRatioForBannerHeight(bannerView()->minimalSizeForOptimalDisplay().height()); }
  BannerView * bannerView() override { return m_bannerView; }
  void reloadBannerView() override;
  int numberOfSignificantDigits() const override { return Poincare::Preferences::VeryLargeNumberOfSignificantDigits; }

  ContinuousFunctionStore * functionStore() const;
  Poincare::Coordinate2D<double> computeNewPointOfInterestFromAbscissa(double start, int direction);
  virtual Poincare::Solver<double>::Interest specialInterest() const { return Poincare::Solver<double>::Interest::None; }
  virtual Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context);

  GraphView * m_graphView;
  BannerView * m_bannerView;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Ion::Storage::Record m_record;
  Escher::MessageTextView m_defaultBannerView;
  bool m_isActive;

private:
  bool moveCursorHorizontally(int direction, int scrollSpeed = 1) override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::AbstractPlotView * curveView() override { return m_graphView; }
};

}

#endif
