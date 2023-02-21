#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H

#include <apps/shared/plot_view_policies.h>

#include "complex_model.h"
#include "illustration_cell.h"

namespace Calculation {

class ComplexGraphPolicy : public Shared::PlotPolicy::WithCurves {
 protected:
  void drawPlot(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect) const;

  ComplexModel* m_complex;
};

class ComplexGraphView
    : public Shared::PlotView<Shared::PlotPolicy::TwoLabeledAxes,
                              ComplexGraphPolicy, Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  ComplexGraphView(ComplexModel* complexModel);
};

class ComplexGraphCell : public IllustrationCell {
 public:
  ComplexGraphCell(ComplexModel* complexModel) : m_view(complexModel) {}
  void reload() { m_view.reload(); }

 private:
  Escher::View* view() override { return &m_view; }
  ComplexGraphView m_view;
};

}  // namespace Calculation

#endif
