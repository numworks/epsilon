#ifndef APPS_PROBABILITY_CONTROLLERS_INTERVAL_INPUT_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INTERVAL_INPUT_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "results_controller.h"

namespace Probability {

class IntervalInputController : public Shared::FloatParameterController<float> {
public:
  IntervalInputController(Escher::StackViewController * stack, ResultsController * resultsController,
                          Escher::InputEventHandlerDelegate * handler);
  int numberOfRows() const override { return k_numberOfParams + 2; }
  int typeAtIndex(int index) override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }
  const char * title() override { return "z-interval blabla..."; }

protected:
  float parameterAtIndex(int i) override { return m_params[i]; }

private:
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  bool setParameterAtIndex(int i, float p) override { m_params[i] = p; return true; }
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;

  constexpr static int k_confidenceCellType = 2;
  constexpr static int k_numberOfParams = 3;
  float m_params[k_numberOfParams + 1 /*confidence*/];
  // TODO fix sublabel wrapping
  Escher::MessageTableCellWithEditableTextWithMessage m_paramCells[k_numberOfParams];
  Escher::MessageTableCellWithEditableText m_confidenceCell;

  ResultsController * m_resultsController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INTERVAL_INPUT_CONTROLLER_H */
