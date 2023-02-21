#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H

#include <apps/shared/plot_view_policies.h>

#include "illustration_cell.h"
#include "trigonometry_model.h"

namespace Calculation {

class UnitCircle : Shared::PlotPolicy::Axes<Shared::PlotPolicy::WithPolarGrid,
                                            Shared::PlotPolicy::PlainAxis,
                                            Shared::PlotPolicy::PlainAxis> {
 public:
  using Axes::reloadAxes;
  void drawAxesAndGrid(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                       KDRect rect) const;

 private:
  constexpr static KDPoint k_minusOneFineTunePosition = {2, -6};
  constexpr static KDPoint k_oneFineTunePosition = {-1, -6};
};

class TrigonometryGraphPolicy : public Shared::PlotPolicy::WithCurves {
 protected:
  void drawPlot(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect) const;

  TrigonometryModel* m_model;
};

class TrigonometryGraphView
    : public Shared::PlotView<UnitCircle, TrigonometryGraphPolicy,
                              Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  TrigonometryGraphView(TrigonometryModel* model);
};

class TrigonometryGraphCell : public IllustrationCell {
 public:
  TrigonometryGraphCell(TrigonometryModel* model) : m_view(model) {}

 private:
  View* view() override { return &m_view; }
  TrigonometryGraphView m_view;
};

}  // namespace Calculation

#endif
