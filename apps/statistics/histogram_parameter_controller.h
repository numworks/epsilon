#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/float_parameter_controller.h"
#include "../shared/discard_pop_up_controller.h"
#include "store.h"

namespace Statistics {

class HistogramParameterController : public Shared::FloatParameterController<double> {
public:
  HistogramParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegateApp, Store * store);
  void viewWillAppear() override;
  const char * title() override;
  int numberOfRows() const override { return 1+k_numberOfCells; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_numberOfCells = 2;
  double extractParameterAtIndex(int index);
  bool handleEvent(Ion::Events::Event event) override;
  double parameterAtIndex(int index) override;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool setParameterAtIndex(int parameterIndex, double f) override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override { return k_numberOfCells; }
  void buttonAction() override;
  MessageTableCellWithEditableText m_cells[k_numberOfCells];
  Store * m_store;
  Shared::DiscardPopUpController m_confirmPopUpController;
  // Temporary parameters
  double m_tempBarWidth;
  double m_tempFirstDrawnBarAbscissa;
};

}

#endif
