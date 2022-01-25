#ifndef SOLVER_SIMPLE_INTEREST_CONTROLLER_H
#define SOLVER_SIMPLE_INTEREST_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/button_delegate.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/message_table_cell_with_sublabel_and_dropdown.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view_controller.h>
#include "data.h"
#include "finance_result_controller.h"
#include "two_messages_popup_data_source.h"

namespace Solver {

class InterestController : public Escher::SelectableListViewController,
                           public Escher::ButtonDelegate,
                           public Escher::TextFieldDelegate,
                           public Escher::DropdownCallback {
public:
  InterestController(Escher::StackViewController * parent, Escher::InputEventHandlerDelegate * handler, FinanceResultController * financeResultController, InterestData * data);
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int typeAtIndex(int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  // Confirm cell plus all parameters but the unknown one
  int numberOfRows() const override { return m_data->numberOfParameters(); }
  bool buttonAction() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }

  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override { return false; }
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  // Escher::DropdownCallback
  void onDropdownSelected(int selectedRow) override;

  void setData(InterestData * data) { m_data = data; }

private:
  uint8_t paramaterAtIndex(int index) const;

  // Dropdown cell is right after all double parameters but the unknown one
  int indexOfDropdown() const { return m_data->numberOfDoubleValues() - 1; }

  constexpr static int k_inputCellType = 0;
  constexpr static int k_dropdownCellType = 1;
  constexpr static int k_confirmCellType = 2;

  // Max number of visible cells
  constexpr static int k_numberOfReusableInputs = 5;

  TwoMessagesPopupDataSource m_dropdownDataSource;
  Escher::MessageTableCellWithEditableTextWithMessage m_cells[k_numberOfReusableInputs];
  Escher::MessageTableCellWithSublabelAndDropdown m_dropdownCell;
  Shared::ButtonWithSeparator m_next;

  // KDFont::SmallFont->glyphSize().width() = 7
  static constexpr int k_titleBufferSize = 1 + Ion::Display::Width / 7;
  char m_titleBuffer[k_titleBufferSize];

  FinanceResultController * m_financeResultController;
  InterestData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_SIMPLE_INTEREST_CONTROLLER_H */
