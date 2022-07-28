#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_GRAPH_CELL_H

#include "../../shared/labeled_curve_view.h"
#include "vector_model.h"
#include "illustration_cell.h"

namespace Calculation {

class VectorGraphView : public Shared::LabeledCurveView {
public:
  VectorGraphView(VectorModel * model) : LabeledCurveView(model), m_model(model) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  constexpr static float k_arcRadiusInPixels = 24; // Ad hoc
private:
  constexpr static float k_arrowSizeInPixels = 8; // Ad hoc
  VectorModel * m_model;
};

class VectorGraphCell : public IllustrationCell {
public:
  VectorGraphCell(VectorModel * model) : m_view(model) {}
private:
  void reloadCell() override;
  View * view() override { return &m_view; }
  VectorGraphView m_view;
};

}

#endif
