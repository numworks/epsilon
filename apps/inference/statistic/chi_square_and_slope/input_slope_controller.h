#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_SLOPE_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_SLOPE_CONTROLLER_H

#include "inference/statistic/chi_square_and_slope/categorical_controller.h"
#include "inference/statistic/chi_square_and_slope/slope_table_cell.h"
#include "inference/statistic/input_controller.h"
#include "shared/layout_field_delegate.h"
#include "shared/input_event_handler_delegate.h"
#include "slope_column_parameter_controller.h"

namespace Inference {

class InputSlopeController : public InputCategoricalController, public Shared::LayoutFieldDelegate, public Shared::InputEventHandlerDelegate {
public:
  InputSlopeController(Escher::StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Poincare::Context * parentContext);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char * title() override {
    InputController::InputTitle(this, m_statistic, m_titleBuffer, InputController::k_titleBufferSize);
    return m_titleBuffer;
  }
  ViewController::TitlesDisplay titlesDisplay() override {
    return m_statistic->subApp() == Inference::SubApp::Interval ? ViewController::TitlesDisplay::DisplayLastTitle : ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  void viewWillAppear() override;

private:
  class PrivateStackViewController: public Escher::StackViewController {
  public:
    using Escher::StackViewController::StackViewController;
    TitlesDisplay titlesDisplay() override { return m_titlesDisplay; }
    void setTitlesDisplay(TitlesDisplay titlesDisplay) { m_titlesDisplay = titlesDisplay; }
  private:
    TitlesDisplay m_titlesDisplay;
  };

  EditableCategoricalTableCell * categoricalTableCell() override { return &m_slopeTableCell; }
  int indexOfSignificanceCell() const override { return k_indexOfTableCell + 1; }
  Escher::StackViewController * stackController() const { return static_cast<Escher::StackViewController *>(parentResponder()); }

  char m_titleBuffer[InputController::k_titleBufferSize];
  Escher::MessageTableCellWithEditableTextWithMessage m_innerDegreeOfFreedomCell;
  SlopeTableCell m_slopeTableCell;
  /* This second stack view controller is used to make the banner of the store
   * parameter controller white, which deviates from the style of the main
   * stack view controller (gray scales). */
  PrivateStackViewController m_secondStackController;
  SlopeColumnParameterController m_storeParameterController;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_SLOPE_CONTROLLER_H */
