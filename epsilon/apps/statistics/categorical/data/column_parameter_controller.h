#pragma once

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/switch_view.h>
#include <poincare/print.h>

#include "table_data.h"

namespace Statistics::Categorical {

class ColumnParameterController
    : public Escher::ExplicitSelectableListViewController,
      public Escher::TextFieldDelegate {
 public:
  ColumnParameterController(Escher::Responder* parentResponder,
                            TableData* tableData,
                            Escher::StackViewController* stackViewController)
      : Escher::ExplicitSelectableListViewController(parentResponder),
        m_columnNameCell(&m_selectableListView, this),
        m_tableData(tableData),
        m_stackViewController(stackViewController) {
    m_columnNameCell.label()->setMessage(I18n::Message::ColumnName);
    m_showInGraphCell.label()->setMessage(
        I18n::Message::CategoricalActivateDeactivateStoreParamTitle);
    m_showInGraphCell.subLabel()->setMessage(
        I18n::Message::CategoricalActivateDeactivateStoreParamDescription);
    m_clearColumnCell.label()->setMessage(I18n::Message::ClearColumn);
    m_relativeFreqCell.label()->setMessage(I18n::Message::RelativeFrequency);
  }
  int numberOfRows() const override { return 4; }

  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override {
    return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
           event == Ion::Events::Down;
  }

  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override {
    const char* name = textField->draftText();
    m_tableData->setGroupName(name, m_column);
    char buffer[20];
    m_tableData->getGroupName(m_column, buffer, sizeof(buffer));
    m_columnNameCell.textField()->setText(buffer);
    m_selectableListView.reloadSelectedCell();
    if (event == Ion::Events::Down) {
      m_selectableListView.handleEvent(event);
    }
    return true;
  }

  bool handleEvent(Ion::Events::Event event) override;

  const Escher::AbstractMenuCell* cell(int row) const override {
    assert(0 <= row && row < k_numberOfCells);
    std::array<const Escher::AbstractMenuCell*, k_numberOfCells> cells = {
        &m_columnNameCell, &m_showInGraphCell, &m_clearColumnCell,
        &m_relativeFreqCell};
    return cells[row];
  }

  void setColumn(int col) {
    m_column = col;
    m_showInGraphCell.accessory()->setState(
        m_tableData->isGroupActive(m_column));
    char buffer[20];
    m_tableData->getGroupName(col, buffer, sizeof(buffer));
    m_columnNameCell.textField()->setText(buffer);
    m_selectableListView.selectRow(0);
  }

  const char* title() const override {
    char buffer[20];
    m_tableData->getGroupName(m_column, buffer, sizeof(buffer));
    Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                  I18n::translate(I18n::Message::ColumnOptions),
                                  buffer);
    return m_titleBuffer;
  }

 private:
  constexpr static int k_numberOfCells = 4;
  constexpr static int k_titleBufferSize = 23 + sizeof(TableData::Label);
  // mutable because title() needs to be const
  mutable char m_titleBuffer[k_titleBufferSize];
  int m_column = -1;

  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_columnNameCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_showInGraphCell;
  Escher::MenuCell<Escher::MessageTextView> m_clearColumnCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::SwitchView>
      m_relativeFreqCell;

  TableData* m_tableData;
  Escher::StackViewController* m_stackViewController;
};

}  // namespace Statistics::Categorical
