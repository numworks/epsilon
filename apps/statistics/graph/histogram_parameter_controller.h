#ifndef STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H
#define STATISTICS_HISTOGRAM_PARAMETER_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <apps/shared/pop_up_controller.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>

#include "../store.h"

namespace Statistics {

class HistogramParameterController
    : public Shared::FloatParameterController<double> {
 public:
  HistogramParameterController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegateApp,
      Store* store);
  void viewWillAppear() override;
  const char* title() override;
  int numberOfRows() const override { return 1 + k_numberOfCells; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  constexpr static int k_numberOfCells = 2;
  double extractParameterAtIndex(int index);
  bool handleEvent(Ion::Events::Event event) override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override { return k_numberOfCells; }
  void buttonAction() override;
  bool authorizedParameters(double tempBarWidth,
                            double tempFirstDrawnBarAbscissa);
  Escher::MessageTableCellWithEditableTextWithMessage m_cells[k_numberOfCells];
  Store* m_store;
  Shared::MessagePopUpController m_confirmPopUpController;
  // Temporary parameters
  double m_tempBarWidth;
  double m_tempFirstDrawnBarAbscissa;
};

}  // namespace Statistics

#endif
