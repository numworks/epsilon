#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../float_parameter_controller.h"
#include "../curve_view_cursor.h"
#include "store.h"

namespace Regression {

class GoToParameterController : public FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, Store * store, CurveViewCursor * cursor);
  void setXPrediction(bool xPrediction);
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_abscisseCell;
  Store * m_store;
  CurveViewCursor * m_cursor;
  bool m_xPrediction;
};

}

#endif