#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "../../shared/curve_view.h"
#include "../../constant.h"
#include "../function_store.h"
#include "../../shared/interactive_curve_view_range.h"

namespace Graph {

class GraphView : public Shared::CurveView {
public:
  GraphView(FunctionStore * functionStore, Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Poincare::Context * context);
  Poincare::Context * context() const;
private:
  char * label(Axis axis, int index) const override;
  float evaluateModelWithParameter(Model * expression, float abscissa) const override;
  char m_xLabels[k_maxNumberOfXLabels][Poincare::Complex::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  char m_yLabels[k_maxNumberOfYLabels][Poincare::Complex::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  FunctionStore * m_functionStore;
  Poincare::Context * m_context;
};

}

#endif
