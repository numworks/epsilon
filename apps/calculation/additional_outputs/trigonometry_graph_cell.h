#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_GRAPH_CELL_H

#include "../../shared/curve_view.h"
#include "trigonometry_model.h"
#include "illustration_cell.h"

namespace Calculation {

class TrigonometryGraphView : public Shared::CurveView {
public:
  TrigonometryGraphView(TrigonometryModel * model);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  TrigonometryModel * m_model;
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

