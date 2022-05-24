#ifndef PROBABILITY_CONTROLLERS_INPUT_SLOPE_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INPUT_SLOPE_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/slope_table_cell.h"
#include "inference/statistic/input_controller.h"
#include "shared/layout_field_delegate.h"
#include "shared/input_event_handler_delegate.h"

using namespace Escher;

namespace Inference {

class InputSlopeController : public InputCategoricalController, public Shared::LayoutFieldDelegate, public Shared::InputEventHandlerDelegate {
public:
  InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Poincare::Context * parentContext);

  // ViewController
  const char * title() override {
    InputController::InputTitle(this, m_statistic, m_titleBuffer, InputController::k_titleBufferSize);
    return m_titleBuffer;
  }

private:
  EditableCategoricalTableCell * categoricalTableCell() override { return &m_slopeTableCell; }
  int indexOfSignificanceCell() const override { return k_indexOfTableCell + 1; }

  char m_titleBuffer[InputController::k_titleBufferSize];
  Escher::MessageTableCellWithEditableTextWithMessage m_innerDegreeOfFreedomCell;
  SlopeTableCell m_slopeTableCell;
};

}  // namespace Inference

#endif /* PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H */
