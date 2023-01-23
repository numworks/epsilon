#ifndef SEQUENCE_GRAPH_CONTROLLER_H
#define SEQUENCE_GRAPH_CONTROLLER_H

#include "cobweb_controller.h"
#include "curve_parameter_controller.h"
#include "curve_view_range.h"
#include "graph_view.h"
#include "term_sum_controller.h"
#include <apps/shared/function_graph_controller.h>
#include <apps/shared/round_cursor_view.h>
#include <apps/shared/sequence_store.h>
#include <apps/shared/with_record.h>
#include <apps/shared/xy_banner_view.h>

namespace Sequence {

class GraphController final : public Shared::FunctionGraphController {
public:
  GraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, CurveViewRange * interactiveRange, Shared::CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, Shared::SequenceStore * sequenceStore);

  // ViewController
  void viewWillAppear() override;

  // AlternateEmptyViewDelegate
  I18n::Message emptyMessage() override;

  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  // InteractiveCurveViewRangeDelegate
  Poincare::Range2D optimalRange(bool computeX, bool computeY, Poincare::Range2D originalRange) const override;

  TermSumController * termSumController() { return &m_termSumController; }
  float interestingXMin() const;
  void moveToRank(int n);

private:

  class SequenceSelectionController : public Shared::FunctionGraphController::FunctionSelectionController {
  public:
    SequenceSelectionController(GraphController * graphController) : Shared::FunctionGraphController::FunctionSelectionController(graphController) {}
    CurveSelectionCellWithChevron * reusableCell(int index, int type) override { assert(index >= 0 && index < Shared::SequenceStore::k_maxNumberOfSequences); return m_cells + index; }
    int reusableCellCount(int type) override { return Shared::SequenceStore::k_maxNumberOfSequences; }
  private:
    Poincare::Layout nameLayoutAtIndex(int j) const override;
    CurveSelectionCellWithChevron m_cells[Shared::SequenceStore::k_maxNumberOfSequences];
  };

  // ZoomCurveViewController
  CurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }

  // SimpleInteractiveCurveViewController
  bool moveCursorHorizontally(int direction, int scrollSpeed = 1) override;

  // InteractiveCurveViewController
  bool openMenuForCurveAtIndex(int index) override;
  SequenceSelectionController * curveSelectionController() const override { return const_cast<SequenceSelectionController *>(&m_sequenceSelectionController); }

  // FunctionBannerDelegate
  Shared::XYBannerView * bannerView() override { return &m_bannerView; }

  // FunctionGraphController
  double defaultCursorT(Ion::Storage::Record record, bool ignoreMargins) override;
  Shared::SequenceStore * functionStore() const override { return static_cast<Shared::SequenceStore *>(Shared::FunctionGraphController::functionStore()); }
  GraphView * functionGraphView() override { return &m_view; }
  CurveParameterController * curveParameterController() override { return &m_curveParameterController; }
  Shared::WithRecord * curveParameterControllerWithRecord() override { return &m_curveParameterController; }

  Shared::RingCursorView m_cursorView;
  Shared::XYBannerView m_bannerView;
  GraphView m_view;
  CurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  SequenceSelectionController m_sequenceSelectionController;
  TermSumController m_termSumController;
  CobwebController m_cobwebController;
  Shared::SequenceStore * m_sequenceStore;
  float m_smallestRank;
};


}

#endif
