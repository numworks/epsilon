#ifndef FINANCE_PARAMETERS_CONTROLLER_H
#define FINANCE_PARAMETERS_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/dropdown_widget.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>
#include <escher/text_field_delegate.h>

#include "result_controller.h"
#include "two_messages_popup_data_source.h"

namespace Finance {

class ParametersController : public Shared::FloatParameterController<double>,
                             public Escher::DropdownCallback {
 public:
  ParametersController(Escher::StackViewController* parent,
                       ResultController* resultController);
  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  // Confirm cell plus all parameters but the unknown one
  int numberOfRows() const override;
  Escher::ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }

  // Escher::DropdownCallback
  void onDropdownSelected(int selectedRow) override;

 private:
  uint8_t interestParameterAtIndex(int index) const;

  // Shared::FloatParameterController<double>
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int i, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
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
  Escher::MenuCellWithEditableText<Escher::MessageTextView,
                                   Escher::MessageTextView>
      m_cells[k_numberOfReusableInputs];
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::DropdownWidget>
      m_dropdownCell;
  Escher::Dropdown m_dropdown;

  constexpr static int k_titleBufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];

  ResultController* m_resultController;
  Escher::MessageTextView m_messageView;
};

}  // namespace Finance

#endif
