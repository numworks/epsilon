#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_VIEW_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_VIEW_H

#include "../../shared/curve_view.h"
#include "complex_model.h"

namespace Calculation {

class ComplexGraphView : public Shared::CurveView {
public:
  ComplexGraphView(ComplexModel * complexModel);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override { return nullptr; }
};

}

#endif

