#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_GRAPH_CELL_H

#include "../../shared/labeled_curve_view.h"
#include "complex_model.h"
#include "illustration_cell.h"

namespace Calculation {

class ComplexGraphView : public Shared::LabeledCurveView {
public:
  ComplexGraphView(ComplexModel * complexModel);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  // '-' + significant digits + ".E-" + 2 digits (the represented dot is a float, so it is bounded by 1E38 and 1E-38
  size_t labelMaxGlyphLengthSize() const override { return 1 + Poincare::Preferences::VeryShortNumberOfSignificantDigits + 3 + 2; }
  ComplexModel * m_complex;
};

class ComplexGraphCell : public IllustrationCell {
public:
  ComplexGraphCell(ComplexModel * complexModel) : m_view(complexModel) {}
  void reload() { m_view.reload(); }
private:
  View * view() override { return &m_view; }
  ComplexGraphView m_view;
};

}

#endif

