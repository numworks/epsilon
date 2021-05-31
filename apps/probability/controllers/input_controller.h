#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/button_with_separator.h>
#include <apps/shared/cell_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/page_controller.h"
#include "probability/models/data.h"

namespace Probability {

class TestResults;

class InputController : public SelectableListViewPage, public ButtonDelegate {
public:
  InputController(Escher::StackViewController * parent, TestResults * resultsController,
                  InputParameters * inputParameters, Escher::InputEventHandlerDelegate * handler,
                  Escher::TextFieldDelegate * textFieldDelegate);
  int numberOfRows() const override {
    return m_inputParameters->numberOfParameters() + 1 /* significance */ + 1 /* button */;
  }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  int typeAtIndex(int i) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  void didBecomeFirstResponder() override;
  void buttonAction() override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  InputParameters * m_inputParameters;
  char m_titleBuffer[30];
  TestResults * m_resultsController;

  constexpr static int k_numberOfReusableCells = 8;  // TODO count
  constexpr static int k_parameterCellType = 0;
  constexpr static int k_significanceCellType = 1;
  constexpr static int k_buttonCellType = 2;

  Escher::MessageTableCellWithEditableTextWithMessage m_parameterCells[k_numberOfReusableCells];
  Escher::MessageTableCellWithEditableTextWithMessage m_significanceCell;
  Shared::ButtonWithSeparator m_next;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H */
