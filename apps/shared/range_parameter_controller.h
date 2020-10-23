#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "float_parameter_controller.h"
#include "discard_pop_up_controller.h"

namespace Shared {

class RangeParameterController : public FloatParameterController<float> {
public:
  RangeParameterController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);
  const char * title() override;
  int numberOfRows() const override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setRange(InteractiveCurveViewRange * range);
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Range");
private:
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  void buttonAction() override;
  constexpr static int k_numberOfTextCell = 4;
  InteractiveCurveViewRange * m_interactiveRange;
  InteractiveCurveViewRange m_tempInteractiveRange;
  Escher::MessageTableCellWithEditableText m_rangeCells[k_numberOfTextCell];
  DiscardPopUpController m_confirmPopUpController;
};

}

#endif
