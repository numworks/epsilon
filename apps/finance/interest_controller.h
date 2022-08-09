#ifndef FINANCE_SIMPLE_INTEREST_CONTROLLER_H
#define FINANCE_SIMPLE_INTEREST_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <apps/shared/float_parameter_controller.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/message_table_cell_with_sublabel_and_dropdown.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view_controller.h>
#include "result_controller.h"
#include "two_messages_popup_data_source.h"

namespace Finance {

class InterestController : public Shared::FloatParameterController<double>,
                           public Escher::DropdownCallback {
public:
  InterestController(Escher::StackViewController * parent, Escher::InputEventHandlerDelegate * handler, ResultController * resultController);
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int typeAtIndex(int index) const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  // Confirm cell plus all parameters but the unknown one
  int numberOfRows() const override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }

  // Escher::DropdownCallback
  void onDropdownSelected(int selectedRow) override;

private:
  uint8_t interestParamaterAtIndex(int index) const;

  // Shared::FloatParameterController<double>
  int reusableParameterCellCount(int type) override;
  Escher::HighlightCell * reusableParameterCell(int i, int type) override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  void buttonAction() override { stackOpenPage(m_resultController); }

  // Dropdown cell is right after all double parameters but the unknown one
  int indexOfDropdown() const;

  constexpr static int k_inputCellType = 0;
  constexpr static int k_dropdownCellType = 2;

  // Max number of visible cells
  constexpr static int k_numberOfReusableInputs = 5;

  TwoMessagesPopupDataSource m_dropdownDataSource;
  Escher::MessageTableCellWithEditableTextWithMessage m_cells[k_numberOfReusableInputs];
  Escher::MessageTableCellWithSublabelAndDropdown m_dropdownCell;

  constexpr static int k_titleBufferSize = 1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char m_titleBuffer[k_titleBufferSize];

  ResultController * m_resultController;
};

}

#endif
