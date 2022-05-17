#ifndef REGRESSION_RESIDUAL_PLOT_CONTROLLER_H
#define REGRESSION_RESIDUAL_PLOT_CONTROLLER_H

#include <escher/view_controller.h>
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <apps/shared/memoized_curve_view_range.h>
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/cursor_view.h>
#include "residual_plot_curve_view.h"
#include "xy_banner_view.h"

namespace Regression {

class ResidualPlotController : public Escher::ViewController {
public:
  ResidualPlotController(Escher::Responder * parentResponder, Store * store);
  void setSeries(int series);
  // TODO Hugo : Sort the indexes by values, add a context for y
  double xAtIndex(int index) const { return m_store->get(m_selectedSeriesIndex, 0, index); }
  double yAtIndex(int index) const { return m_store->residualAtIndexForSeries(m_selectedSeriesIndex, index, AppsContainer::sharedAppsContainer()->globalContext()); }
  KDColor selectedSeriesColor() const { return Store::colorOfSeriesAtIndex(m_selectedSeriesIndex); }
  int numberOfResidualDots() const { return m_store->numberOfPairsOfSeries(m_selectedSeriesIndex); }

  // Escher::ViewController
  const char * title() override { return I18n::translate(I18n::Message::Plot); } // TODO Hugo : Use correct Message
  Escher::View * view() override { return &m_curveView; }
  void viewWillAppear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return Escher::ViewController::TitlesDisplay::DisplayLastTitle; }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

  TELEMETRY_ID("ResidualPlotGraph");
private:
  static constexpr double k_relativeMargin = 0.1;

  // Move cursor to selected and updates the banner
  void updateCursor();
  bool moveHorizontally(int direction);

  Store * m_store;
  Shared::CurveViewCursor m_cursor;
  Shared::CursorView m_cursorView;
  Shared::MemoizedCurveViewRange m_range;
  XYBannerView m_bannerView;
  ResidualPlotCurveView m_curveView;
  int m_selectedDotIndex;
  int m_selectedSeriesIndex;
};

}

#endif
