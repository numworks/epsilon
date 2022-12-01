#include "graph_view.h"
#include "../model/model.h"
#include "../app.h"
#include <apps/apps_container_helper.h>
#include <poincare/context.h>
#include <assert.h>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Regression {

// RegressionPlotPolicy

static Coordinate2D<float> evaluateRegression(float x, void * model, void * context) {
  Model * regression = reinterpret_cast<Model *>(model);
  double * coeffs = reinterpret_cast<double *>(context);
  return Poincare::Coordinate2D<float>(x, regression->evaluate(coeffs, x));
}

void RegressionPlotPolicy::drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  Context * globalContext = AppsContainerHelper::sharedAppsContainerGlobalContext();
  int selectedSeries = App::app()->graphController()->selectedSeriesIndex();
  for (int s = 0; s <= Store::k_numberOfSeries; s++) {
    // Draw the selected series last
    if (s == selectedSeries) {
      continue;
    }
    int series = s;
    if (s == Store::k_numberOfSeries) {
      series = selectedSeries;
    }
    if (!m_store->seriesIsValid(series)) {
      continue;
    }
    assert(series < Palette::numberOfDataColors());
    KDColor color = Palette::DataColor[series];
    // - Draw regression curve
    Model * seriesModel = m_store->modelForSeries(series);
    CurveDrawing plot(Curve2D(evaluateRegression, seriesModel), m_store->coefficientsForSeries(series, globalContext), m_store->xMin(), m_store->xMax(), plotView->pixelWidth(), color);
    plot.draw(plotView, ctx, rect);
    // - Draw data points
    int numberOfPairs = m_store->numberOfPairsOfSeries(series);
    for (int i = 0; i < numberOfPairs; i++) {
      plotView->drawDot(ctx, rect, Dots::Size::Tiny, Coordinate2D<float>(m_store->get(series, 0, i), m_store->get(series, 1, i)), color);
    }
    //   Mean point is hidden in scatter plots
    if (m_store->seriesRegressionType(series) != Model::Type::None) {
      Coordinate2D<float> mean(m_store->meanOfColumn(series, 0), m_store->meanOfColumn(series, 1));
      plotView->drawDot(ctx, rect, Dots::Size::Medium, mean, color);
      plotView->drawDot(ctx, rect, Dots::Size::Tiny, mean, KDColorWhite);
    }
  }
}

// GraphView

GraphView::GraphView(Store * store, CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView) :
  PlotView(store)
{
  // RegressionPlotPolicy
  m_store = store;
  // WithBanner
  m_banner = bannerView;
  // WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

}
