#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H

#include "../../shared/curve_view.h"
#include "trigonometry_model.h"

namespace Calculation {

class TrigonometryGraphView : public Shared::CurveView {
public:
  TrigonometryGraphView(TrigonometryModel * model);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override { return nullptr; }
  TrigonometryModel * m_angle;
};

class TrigonometryGraphCell : public HighlightCell {
public:
  TrigonometryGraphCell(TrigonometryModel * model) : m_view(model) {}
  void setHighlighted(bool highlight) override { return; }
private:
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override { m_view.setFrame(bounds(), force); }
  TrigonometryGraphView m_view;
};

}

#endif

