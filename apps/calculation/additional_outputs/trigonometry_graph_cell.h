#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H

#include "trigonometry_model.h"
#include "illustration_cell.h"
#include <apps/shared/plot_view_policies.h>

namespace Calculation {

class TrigonometryGraphPolicy : public Shared::PlotPolicy::WithCurves {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  TrigonometryModel * m_model;
};

class TrigonometryGraphView : public Shared::PlotView<Shared::PlotPolicy::PolarGrid, TrigonometryGraphPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  TrigonometryGraphView(TrigonometryModel * model);
};

class TrigonometryGraphCell : public IllustrationCell {
public:
  TrigonometryGraphCell(TrigonometryModel * model) : m_view(model) {}
private:
  View * view() override { return &m_view; }
  TrigonometryGraphView m_view;
};

}

#endif

