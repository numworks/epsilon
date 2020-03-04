#ifndef SEQUENCE_GRAPH_CONTROLLER_H
#define SEQUENCE_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "../../shared/xy_banner_view.h"
#include "curve_parameter_controller.h"
#include "curve_view_range.h"
#include "term_sum_controller.h"
#include "../../shared/function_graph_controller.h"
#include "../../shared/cursor_view.h"
#include "../sequence_store.h"

namespace Sequence {

class GraphController final : public Shared::FunctionGraphController {
public:
  GraphController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, CurveViewRange * graphRange, Shared::CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header);
  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  TermSumController * termSumController() { return &m_termSumController; }
  // InteractiveCurveViewRangeDelegate
  float interestingXMin() const override;
  float interestingXHalfRange() const override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
private:
  Shared::XYBannerView * bannerView() override { return &m_bannerView; }
  bool handleEnter() override;
  bool moveCursorHorizontally(int direction, bool fast = false) override;
  double defaultCursorT(Ion::Storage::Record record) override;
  CurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  SequenceStore * functionStore() const override { return static_cast<SequenceStore *>(Shared::FunctionGraphController::functionStore()); }
  GraphView * functionGraphView() override { return &m_view; }
  CurveParameterController * curveParameterController() override { return &m_curveParameterController; }
  Shared::CursorView m_cursorView;
  Shared::XYBannerView m_bannerView;
  GraphView m_view;
  CurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  TermSumController m_termSumController;
};


}

#endif
