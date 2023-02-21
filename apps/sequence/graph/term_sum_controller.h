#ifndef SEQUENCE_TERM_SUM_CONTROLLER_H
#define SEQUENCE_TERM_SUM_CONTROLLER_H

#include <poincare/layout.h>

#include "../../shared/sum_graph_controller.h"
#include "curve_view_range.h"
#include "graph_view.h"

namespace Sequence {

class TermSumController : public Shared::SumGraphController {
 public:
  TermSumController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      GraphView* graphView, CurveViewRange* graphRange,
      Shared::CurveViewCursor* cursor);
  const char* title() override;

 private:
  bool moveCursorHorizontallyToPosition(double position) override;
  I18n::Message legendMessageAtStep(Step step) override;
  double cursorNextStep(double position,
                        OMG::HorizontalDirection direction) override;
  Poincare::Layout createFunctionLayout() override;
};

}  // namespace Sequence

#endif
