#include "graph_view.h"
#include "model/model.h"
#include <apps/apps_container.h>
#include <poincare/context.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Regression {

GraphView::GraphView(Store * store, CurveViewCursor * cursor, BannerView * bannerView, Shared::CursorView * cursorView) :
  LabeledCurveView(store, cursor, bannerView, cursorView),
  m_store(store)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  Poincare::Context * globContext = AppsContainer::sharedAppsContainer()->globalContext();
  for (size_t series = 0; series < Store::k_numberOfSeries; series++) {
    if (m_store->seriesIsValid(series)) {
      assert(series < Palette::numberOfDataColors());
      KDColor color = Palette::DataColor[series];
      Model * seriesModel = m_store->modelForSeries(series);
      drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, [](float abscissa, void * model, void * context) {
          Model * regressionModel = static_cast<Model *>(model);
          double * regressionCoefficients = static_cast<double *>(context);
          return Poincare::Coordinate2D<float>(abscissa, (float)regressionModel->evaluate(regressionCoefficients, abscissa));
          },
          seriesModel, m_store->coefficientsForSeries(series, globContext), color);
      for (int index = 0; index < m_store->numberOfPairsOfSeries(series); index++) {
        drawDot(ctx, rect, m_store->get(series, 0, index), m_store->get(series, 1, index), color);
      }
      // Hide mean points in scatter plots
      if (m_store->seriesRegressionType(series) != Model::Type::None) {
        drawDot(ctx, rect, m_store->meanOfColumn(series, 0), m_store->meanOfColumn(series, 1), color, Size::Medium);
        drawDot(ctx, rect, m_store->meanOfColumn(series, 0), m_store->meanOfColumn(series, 1), KDColorWhite);
      }
    }
  }
}

}
