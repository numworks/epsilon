#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "store.h"

namespace Statistics {

class HistogramParameterController : public Shared::FloatParameterController {
public:
  HistogramParameterController(Responder * parentResponder, Store * store);
  const char * title() override;
  void viewWillAppear() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void unloadView() override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float parameterAtIndex(int index) override;
  float previousParameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  View * createView() override;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  constexpr static int k_numberOfCells = 2;
  MessageTableCellWithEditableText * m_cells[k_numberOfCells];
  float m_previousParameters[k_numberOfCells];
  Store * m_store;
};

}

#endif
