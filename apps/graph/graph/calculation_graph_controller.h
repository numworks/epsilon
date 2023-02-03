#ifndef GRAPH_CALCULATION_GRAPH_CONTROLLER_H
#define GRAPH_CALCULATION_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/simple_interactive_curve_view_controller.h>
#include <apps/shared/function_banner_delegate.h>
#include <poincare/point_of_interest.h>

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
  float cursorBottomMarginRatio() const override { return cursorBottomMarginRatioForBannerHeight(m_bannerView->minimalSizeForOptimalDisplay().height()); }
  BannerView * bannerView() override { return m_bannerView; }
  void reloadBannerView() override;
  int numberOfSignificantDigits(bool capped = false) const override {
    /* Always cap number of significant digits since calculations are not
     * precise enough to display many significant digits. */
    return Shared::FunctionBannerDelegate::numberOfSignificantDigits(true);
  }

  Shared::ContinuousFunctionStore * functionStore() const;
  Poincare::Coordinate2D<double> computeNewPointOfInterestFromAbscissa(double start, OMG::HorizontalDirection direction);
  virtual Poincare::Solver<double>::Interest specialInterest() const { return Poincare::Solver<double>::Interest::None; }
  virtual Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) { return computeAtLeastOnePointOfInterest(start, max, context).xy(); }
  Poincare::PointOfInterest computeAtLeastOnePointOfInterest(double start, double max, Poincare::Context * context);

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
  // Prevent horizontal panning to preserve search interval
  float cursorRightMarginRatio() const override { return 0.0f; }
  float cursorLeftMarginRatio() const override { return 0.0f; }
};

}

#endif
