#ifndef REGRESSION_RESIDUAL_PLOT_CONTROLLER_H
#define REGRESSION_RESIDUAL_PLOT_CONTROLLER_H

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/memoized_curve_view_range.h>
#include <apps/shared/round_cursor_view.h>
#include <escher/view_controller.h>

#include "residual_plot_banner_view.h"
#include "residual_plot_curve_view.h"
#include "residual_plot_range.h"

namespace Regression {

class ResidualPlotController : public Escher::ViewController {
 public:
  ResidualPlotController(Escher::Responder* parentResponder, Store* store);
  void setSeries(int series);
  double xAtIndex(int index) const {
    return m_store->get(m_selectedSeriesIndex, 0, index);
  }
  double yAtIndex(int index) const {
    return m_store->residualAtIndexForSeries(
        m_selectedSeriesIndex, index,
        AppsContainerHelper::sharedAppsContainerGlobalContext());
  }
  KDColor selectedSeriesColor() const {
    return Store::colorOfSeriesAtIndex(m_selectedSeriesIndex);
  }
  int numberOfResidualDots() const {
    return m_store->numberOfPairsOfSeries(m_selectedSeriesIndex);
  }

  // Escher::ViewController
  const char* title() override {
    return I18n::translate(I18n::Message::ResidualPlot);
  }
  Escher::View* view() override { return &m_curveView; }
  void viewWillAppear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::DisplayLastTitle;
  }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

  TELEMETRY_ID("ResidualPlotGraph");

 private:
  // Move cursor to selected and updates the banner
  void updateCursor();
  bool moveHorizontally(OMG::HorizontalDirection direction);

  Store* m_store;
  Shared::CurveViewCursor m_cursor;
  Shared::RingCursorView m_cursorView;
  ResidualPlotRange m_range;
  ResidualPlotBannerView m_bannerView;
  ResidualPlotCurveView m_curveView;
  int m_selectedDotIndex;
  int m_selectedSeriesIndex;
};

}  // namespace Regression

#endif
