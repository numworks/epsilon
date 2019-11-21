#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H

#include "../../shared/curve_view.h"
#include "complex_model.h"

namespace Calculation {

class ComplexGraphView : public Shared::CurveView {
public:
  ComplexGraphView(ComplexModel * complexModel);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override { return nullptr; }
  ComplexModel * m_complex;
};

class ComplexGraphCell : public HighlightCell {
public:
  ComplexGraphCell(ComplexModel * complexModel) : m_view(complexModel) {}
  void setHighlighted(bool highlight) override { return; }
private:
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override { m_view.setFrame(bounds(), force); }
  ComplexGraphView m_view;
};

}

#endif

