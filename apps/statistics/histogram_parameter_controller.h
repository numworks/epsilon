#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "store.h"

namespace Statistics {

class HistogramParameterController : public Shared::FloatParameterController {
public:
  HistogramParameterController(Responder * parentResponder, Store * store);
  const char * title() const override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  char m_draftTextBuffer[PointerTableCellWithEditableText::k_bufferLength];
  PointerTableCellWithEditableText m_binWidthCell;
  PointerTableCellWithEditableText m_minValueCell;
  Store * m_store;
};

}

#endif
