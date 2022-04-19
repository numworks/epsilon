#ifndef SEQUENCE_GRAPH_CONTROLLER_H
#define SEQUENCE_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "../../shared/xy_banner_view.h"
#include "curve_parameter_controller.h"
#include "curve_view_range.h"
#include "term_sum_controller.h"
#include "../../shared/function_graph_controller.h"
#include "../../shared/cursor_view.h"
#include "../../shared/sequence_store.h"

namespace Sequence {

class GraphController final : public Shared::FunctionGraphController {
public:
  GraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::SequenceStore * sequenceStore, CurveViewRange * graphRange, Shared::CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * rangeVersion, Escher::ButtonRowController * header);
  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  TermSumController * termSumController() { return &m_termSumController; }
  // InteractiveCurveViewRangeDelegate
  float interestingXMin() const override;
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;
private:
  class SequenceSelectionController : public Shared::FunctionGraphController::FunctionSelectionController {
  public:
    SequenceSelectionController(GraphController * graphController) : Shared::FunctionGraphController::FunctionSelectionController(graphController) {}
    CurveSelectionCell * reusableCell(int index, int type) override { assert(index >= 0 && index < Shared::SequenceStore::k_maxNumberOfSequences); return m_cells + index; }
    int reusableCellCount(int type) override { return Shared::SequenceStore::k_maxNumberOfSequences; }
  private:
    Poincare::Layout nameLayoutAtIndex(int j) const override;
    CurveSelectionCell m_cells[Shared::SequenceStore::k_maxNumberOfSequences];
  };

  Shared::XYBannerView * bannerView() override { return &m_bannerView; }
  bool openMenuForCurveAtIndex(int index) override;
  bool moveCursorHorizontally(int direction, int scrollSpeed = 1) override;
  double defaultCursorT(Ion::Storage::Record record) override;
  CurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::SequenceStore * functionStore() const override { return static_cast<Shared::SequenceStore *>(Shared::FunctionGraphController::functionStore()); }
  GraphView * functionGraphView() override { return &m_view; }
  CurveParameterController * curveParameterController() override { return &m_curveParameterController; }
  SequenceSelectionController * curveSelectionController() const override { return const_cast<SequenceSelectionController *>(&m_sequenceSelectionController); }
  Shared::CursorView m_cursorView;
  Shared::XYBannerView m_bannerView;
  GraphView m_view;
  CurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  SequenceSelectionController m_sequenceSelectionController;
  TermSumController m_termSumController;
};


}

#endif
