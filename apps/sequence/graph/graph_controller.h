#ifndef SEQUENCE_GRAPH_CONTROLLER_H
#define SEQUENCE_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "curve_view_range.h"
#include "term_sum_controller.h"
#include "../../shared/storage_function_graph_controller.h"
#include "../../shared/cursor_view.h"
#include "../sequence_store.h"

namespace Sequence {

class GraphController final : public Shared::StorageFunctionGraphController {
public:
  GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, CurveViewRange * graphRange, Shared::CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header);
  I18n::Message emptyMessage() override;
  TermSumController * termSumController() { return &m_termSumController; }
  // InteractiveCurveViewRangeDelegate
  float interestingXMin() const override;
  float interestingXHalfRange() const override;
protected:
  int numberOfCurves() const override { return functionStore()->numberOfModels(); }
private:
  BannerView * bannerView() override { return &m_bannerView; }
  bool handleEnter() override;
  bool moveCursorHorizontally(int direction) override;
  double defaultCursorAbscissa() override;
  CurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  SequenceStore * functionStore() const override { return static_cast<SequenceStore *>(Shared::StorageFunctionGraphController::functionStore()); }
  GraphView * functionGraphView() override { return &m_view; }
  View * cursorView() override {
    return &m_cursorView;
  }
  CurveParameterController * curveParameterController() override { return &m_curveParameterController; }
  Shared::CursorView m_cursorView;
  BannerView m_bannerView;
  GraphView m_view;
  CurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  TermSumController m_termSumController;
};


}

#endif
