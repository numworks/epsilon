#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "store.h"

namespace Statistics {

class HistogramParameterController : public Shared::FloatParameterController<double> {
public:
  HistogramParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegateApp, Store * store);
  const char * title() override;
  int numberOfRows() override { return 1+k_numberOfCells; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_numberOfCells = 2;
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override { return k_numberOfCells; }
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  MessageTableCellWithEditableText m_cells[k_numberOfCells];
  Store * m_store;
};

}

#endif
