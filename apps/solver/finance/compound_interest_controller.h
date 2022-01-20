#ifndef SOLVER_COMPOUND_INTEREST_CONTROLLER_H
#define SOLVER_COMPOUND_INTEREST_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <apps/shared/page_controller.h>
#include <escher/button_delegate.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view_controller.h>
#include "data.h"
#include "message_table_cell_with_sublabel_and_dropdown.h"
#include "two_message_popup_data_source.h"

namespace Solver {

class CompoundInterestController : public Shared::SelectableListViewPage,
                             public Escher::ButtonDelegate,
                             public Escher::TextFieldDelegate,
                             public Escher::DropdownCallback {
public:
  CompoundInterestController(Escher::StackViewController * parent,
                          Escher::InputEventHandlerDelegate * handler,
                           Escher::ViewController * financeResultController,
                           FinanceData * data);
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int typeAtIndex(int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return k_indexOfNext + 1; }
  bool buttonAction() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }


  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override { return false; }
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // Escher::DropdownCallback
  void onDropdownSelected(int selectedRow) override;

private:
  CompoundInterestParameter paramaterAtIndex(int index) const;
  CompoundInterestData * compoundInterestData() const { assert(!m_data->isSimpleInterest); return &(m_data->m_data.m_compoundInterestData); }
  int stackTitleStyleStep() const override { return 1; }

  constexpr static int k_indexOfPayment = CompoundInterestData::k_numberOfDoubleValues - 1;
  constexpr static int k_indexOfNext = k_indexOfPayment + 1;

  constexpr static int k_inputCellType = 0;
  constexpr static int k_dropdownCellType = 1;
  constexpr static int k_confirmCellType = 2;

  // Max number of visible cells
  constexpr static int k_numberOfReusableInputs = 5;
  static_assert(k_numberOfReusableInputs <= k_indexOfPayment + 1, "Too many reusable inputs");

  TwoMessagePopupDataSource m_paymentDataSource;
  Escher::MessageTableCellWithEditableTextWithMessage m_cells[k_numberOfReusableInputs];
  MessageTableCellWithSublabelAndDropdown m_payment;
  Shared::ButtonWithSeparator m_next;

  static constexpr int k_titleBufferSize = 1 + Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  char m_titleBuffer[k_titleBufferSize];

  Escher::ViewController * m_financeResultController;
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_COMPOUND_INTEREST_CONTROLLER_H */
