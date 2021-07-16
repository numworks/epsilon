#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_CATEGORICAL_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_CATEGORICAL_CONTROLLER_H

#include <escher/selectable_table_view.h>

#include "button_delegate.h"
#include "probability/controllers/results_controller.h"
#include "probability/gui/input_categorical_view.h"
#include "probability/gui/page_controller.h"
#include "probability/models/statistic/chi2_statistic.h"

namespace Probability {

class InputCategoricalController : public Page,
                                   public Shared::ParameterTextFieldDelegate,
                                   public ButtonDelegate {
public:
  InputCategoricalController(StackViewController * parent,
                             Page * resultsController,
                             Chi2Statistic * statistic,
                             InputEventHandlerDelegate * inputEventHandlerDelegate);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // Responder
  void didBecomeFirstResponder() override;
  // ButtonDelegate
  void buttonAction() override;

  // ViewController
  View * view() override { return &m_contentView; }
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }

protected:
  Chi2Statistic * m_statistic;
  Page * m_resultsController;
  InputCategoricalView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_CATEGORICAL_CONTROLLER_H */
