#ifndef PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H
#define PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H

#include <escher/selectable_table_view.h>

#include "input_categorical_view.h"
#include <shared/parameter_text_field_delegate.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "probability/models/statistic/chi2_test.h"

namespace Probability {

/* This is the common Controller between Homogeneity and Goodness input controllers. It parses
 * significance level input and own the content view. */
class InputCategoricalController : public Escher::ViewController,
                                   public Shared::ParameterTextFieldDelegate,
                                   public Escher::SelectableTableViewDelegate {
public:
  InputCategoricalController(Escher::StackViewController * parent,
                             Escher::ViewController * resultsController,
                             Chi2Test * statistic,
                             Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);

  virtual TableViewController * tableViewController() = 0;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // Responder
  void didEnterResponderChain(Responder * previousResponder) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  static bool ButtonAction(void * c, void * s);

  // ViewController
  Escher::View * view() override { return contentView(); }
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::DisplayLastTitle;
  }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection) override;

protected:
  bool handleEditedValue(int i, double p, Escher::TextField * textField, Ion::Events::Event event, int indexIdUp, int indexOtherwise);
  virtual InputCategoricalView * contentView() = 0;

  Chi2Test * m_statistic;
  Escher::ViewController * m_resultsController;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H */
