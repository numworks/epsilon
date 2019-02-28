#ifndef SEQUENCE_TERM_SUM_CONTROLLER_H
#define SEQUENCE_TERM_SUM_CONTROLLER_H

#include <escher.h>
#include <poincare/layout.h>
#include "graph_view.h"
#include "curve_view_range.h"
#include "../../shared/storage_sum_graph_controller.h"

namespace Sequence {

class TermSumController : public Shared::StorageSumGraphController {
public:
  TermSumController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, CurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
private:
  bool moveCursorHorizontallyToPosition(double position) override;
  I18n::Message legendMessageAtStep(Step step) override;
  double cursorNextStep(double position, int direction) override;
  Poincare::Layout createFunctionLayout(Shared::ExpiringPointer<Shared::StorageFunction> function) override;
};

}

#endif
