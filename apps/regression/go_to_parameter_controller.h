#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../float_parameter_controller.h"
#include "data.h"

namespace Regression {

class GoToParameterController : public FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, Data * data);
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
  Data * m_data;
  bool m_xPrediction;
};

}

#endif