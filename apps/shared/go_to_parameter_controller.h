#ifndef SHARED_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "float_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"
#include "function.h"

namespace Shared {

class GoToParameterController : public FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor);
  const char * title() const override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setFunction(Function * function);
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  char m_draftTextBuffer[PointerTableCellWithEditableText::k_bufferLength];
  PointerTableCellWithEditableText m_abscisseCell;
  InteractiveCurveViewRange * m_graphRange;
  CurveViewCursor * m_cursor;
  Function * m_function;
};

}

#endif
