#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H

#include <apps/probability/gui/horizontal_or_vertical_layout.h>
#include <apps/shared/button_with_separator.h>
#include <escher/buffer_table_cell.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/scroll_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include <escher/view.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include <kandinsky/coordinate.h>
#include <shared/parameter_text_field_delegate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/input_categorical_view.h"
#include "probability/gui/input_goodness_table_view.h"
#include "probability/gui/page_controller.h"
#include "probability/models/statistic/goodness_statistic.h"
#include "results_controller.h"

using namespace Escher;

namespace Probability {

class InputGoodnessController : public Page,
                                public ButtonDelegate,
                                public Shared::ParameterTextFieldDelegate,
                                public Escher::SelectableTableViewDelegate {
public:
  InputGoodnessController(StackViewController * parent,
                          ResultsController * resultsController,
                          GoodnessStatistic * statistic,
                          InputEventHandlerDelegate * inputEventHandlerDelegate);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }
  const char * title() override;
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  void buttonAction() override;

  // TextFieldDelegate
  // TODO factor with InputController
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

private:
  constexpr static int k_titleBufferSize = 30;
  char m_titleBuffer[k_titleBufferSize];
  GoodnessStatistic * m_statistic;
  ResultsController * m_resultsController;
  InputGoodnessTableView m_inputTableView;
  InputCategoricalView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_GOODNESS_CONTROLLER_H */
