#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../float_parameter_controller.h"
#include "data.h"

namespace Statistics {

class HistogramParameterController : public FloatParameterController {
public:
  HistogramParameterController(Responder * parentResponder, Data * data);
  ExpressionTextFieldDelegate * textFieldDelegate() override;
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setFunction(Function * function);
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_binWidthCell;
  EditableTextMenuListCell m_minValueCell;
  Data * m_data;
};

}

#endif
