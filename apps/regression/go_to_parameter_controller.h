#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "../shared/curve_view_cursor.h"
#include "store.h"

namespace Regression {

class GoToParameterController : public Shared::FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, Store * store, Shared::CurveViewCursor * cursor);
  void setXPrediction(bool xPrediction);
  const char * title() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void viewWillAppear() override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float previousParameterAtIndex(int index) override;
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  void buttonAction() override;
  char m_draftTextBuffer[PointerTableCellWithEditableText::k_bufferLength];
  PointerTableCellWithEditableText m_abscisseCell;
  float m_previousParameter;
  Store * m_store;
  Shared::CurveViewCursor * m_cursor;
  bool m_xPrediction;
};

}

#endif
