#ifndef SHARED_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "float_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class GoToParameterController : public FloatParameterController<double> {
public:
  GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor);
  int numberOfRows() const override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  void setParameterName(I18n::Message message) { m_parameterCell.setMessage(message); }
  CurveViewCursor * m_cursor;
  InteractiveCurveViewRange * m_graphRange;
private:
  void buttonAction() override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  MessageTableCellWithEditableText m_parameterCell;
};

}

#endif
