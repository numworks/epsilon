#include "storage_graph_view.h"
#include <float.h>

using namespace Shared;

namespace Graph {

StorageGraphView::StorageGraphView(StorageCartesianFunctionStore * functionStore, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  StorageFunctionGraphView<StorageCartesianFunction>(graphRange, cursor, bannerView, cursorView),
  m_functionStore(functionStore),
  m_tangent(false)
{
}

void StorageGraphView::reload() {
  if (m_tangent) {
    KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height()-m_bannerView->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  return StorageFunctionGraphView<StorageCartesianFunction>::reload();
}

void StorageGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  StorageFunctionGraphView<StorageCartesianFunction>::drawRect(ctx, rect);
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    StorageCartesianFunction f = m_functionStore->activeFunctionAtIndex(i);
    /* Draw function (color the area under curve of the selected function) */
    if (f == m_selectedFunction) {
      drawCurve(ctx, rect, [](float t, void * model, void * context) {
        StorageCartesianFunction * f = (StorageCartesianFunction *)model;
        Poincare::Context * c = (Poincare::Context *)context;
        return f->evaluateAtAbscissa(t, c);
      }, &f, context(), f.color(), true, m_highlightedStart, m_highlightedEnd);
    } else {
      drawCurve(ctx, rect, [](float t, void * model, void * context) {
        StorageCartesianFunction * f = (StorageCartesianFunction *)model;
        Poincare::Context * c = (Poincare::Context *)context;
        return f->evaluateAtAbscissa(t, c);
      }, &f, context(), f.color());
    }

    /* Draw tangent */
    if (m_tangent && f == m_selectedFunction) {
      float tangentParameter[2];
      tangentParameter[0] = f.approximateDerivative(m_curveViewCursor->x(), context());
      tangentParameter[1] = -tangentParameter[0]*m_curveViewCursor->x()+f.evaluateAtAbscissa(m_curveViewCursor->x(), context());
      drawCurve(ctx, rect, [](float t, void * model, void * context) {
          float * tangent = (float *)model;
          return tangent[0]*t+tangent[1];
        }, tangentParameter, nullptr, Palette::GreyVeryDark);
    }
  }
}

}
