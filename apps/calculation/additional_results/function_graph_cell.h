#ifndef CALCULATION_FUNCTION_GRAPH_CELL_H
#define CALCULATION_FUNCTION_GRAPH_CELL_H

#include <apps/shared/plot_view_policies.h>
#include <kandinsky/rect.h>
#include <poincare/preferences.h>

#include "function_model.h"
#include "illustration_cell.h"

namespace Calculation {

template <size_t N>
class FunctionAxis : public Shared::PlotPolicy::LabeledAxis<N> {
 public:
  FunctionAxis()
      : Shared::PlotPolicy::LabeledAxis<N>::LabeledAxis(),
        m_specialLabelRect(KDRectZero) {}
  void reloadAxis(Shared::AbstractPlotView* plotView, OMG::Axis axis) override;
  void drawAxis(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect, OMG::Axis axis) const;

 private:
  constexpr static int k_labelAvoidanceMargin = 2;
  constexpr static int k_labelsPrecision =
      Poincare::Preferences::VeryShortNumberOfSignificantDigits;
  constexpr static KDColor k_specialLabelsColor = Escher::Palette::Red;

  // AbstractLabeledAxis
  bool labelWillBeDisplayed(int i, KDRect labelRect) const override;

  // LabeledAxis
  size_t numberOfLabels() const override { return N + 1; }
  char* mutableLabel(int i) override {
    return i == N ? m_specialLabel
                  : Shared::PlotPolicy::LabeledAxis<N>::mutableLabel(i);
  }

  mutable KDRect m_specialLabelRect;
  char m_specialLabel
      [Shared::PlotPolicy::AbstractLabeledAxis::k_labelBufferMaxSize];
};

typedef FunctionAxis<
    Shared::PlotPolicy::AbstractLabeledAxis::k_maxNumberOfXLabels>
    FunctionHorizontalLabeledAxis;
typedef FunctionAxis<
    Shared::PlotPolicy::AbstractLabeledAxis::k_maxNumberOfYLabels>
    FunctionVerticalLabeledAxis;

typedef Shared::PlotPolicy::Axes<Shared::PlotPolicy::WithCartesianGrid,
                                 FunctionHorizontalLabeledAxis,
                                 FunctionVerticalLabeledAxis>
    FunctionTwoLabeledAxes;

class FunctionGraphPolicy : public Shared::PlotPolicy::WithCurves {
 protected:
  void drawPlot(const Shared::AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect) const;

  FunctionModel* m_model;

 private:
  constexpr static KDColor k_color = Escher::Palette::Red;
};

class FunctionGraphView
    : public Shared::PlotView<FunctionTwoLabeledAxes, FunctionGraphPolicy,
                              Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  FunctionGraphView(FunctionModel* model) : PlotView(model) { m_model = model; }
};

class FunctionGraphCell : public IllustrationCell {
 public:
  FunctionGraphCell(FunctionModel* model) : m_view(model) {}
  void reload() override { m_view.reload(); }

 private:
  View* view() override { return &m_view; }
  FunctionGraphView m_view;
};

}  // namespace Calculation

#endif
