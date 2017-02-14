#ifndef GRAPH_GRAPH_GOTO_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_GOTO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "../../shared/float_parameter_controller.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"

namespace Graph {
class GoToParameterController : public Shared::FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setFunction(Function * function);
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_abscisseCell;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  Function * m_function;
};

}

#endif
