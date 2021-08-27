#ifndef APPS_PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H
#define APPS_PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H

#include <escher/selectable_table_view.h>

#include "button_delegate.h"
#include "input_categorical_view.h"
#include "probability/gui/page_controller.h"
#include "probability/models/statistic/chi2_statistic.h"

namespace Probability {

/* This is the common Controller between Homogeneity and Goodness input controllers. It parses
 * significance level input and own the content view. */
class InputCategoricalController : public Page,
                                   public Shared::ParameterTextFieldDelegate,
                                   public ButtonDelegate,
                                   public Escher::SelectableTableViewDelegate {
public:
  InputCategoricalController(StackViewController * parent,
                             Page * resultsController,
                             Chi2Statistic * statistic,
                             InputEventHandlerDelegate * inputEventHandlerDelegate);

  virtual TableViewController * tableViewController() = 0;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  // ButtonDelegate
  bool buttonAction() override;

  // ViewController
  View * view() override { return &m_contentView; }
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection) override;

protected:
  Chi2Statistic * m_statistic;
  Page * m_resultsController;
  InputCategoricalView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_INPUT_CATEGORICAL_CONTROLLER_H */
