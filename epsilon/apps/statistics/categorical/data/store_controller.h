#pragma once

#include <apps/i18n.h>
#include <apps/inference/text_helpers.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/highlight_cell.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <shared/math_field_delegate.h>
#include <shared/tab_table_controller.h>

#include "table_data.h"
#include "table_data_source.h"

namespace Statistics::Categorical {

class StoreController : public Shared::TabTableController,
                        public Escher::SelectableTableViewDelegate,
                        public TableViewDataSource,
                        public Escher::ButtonRowDelegate,
                        public Shared::MathTextFieldDelegate {
 public:
  StoreController(Escher::Responder* parentResponder,
                  Escher::ButtonRowController* header,
                  Escher::StackViewController* stackViewController,
                  Escher::TabViewController* tabViewController,
                  Escher::ViewController* dataTypeController,
                  TableData* tableData);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  // Categorical::TableViewDataSource
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;

  // Escher::ButtonRowDelegate
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    assert(position == Escher::ButtonRowController::Position::Top);
    return 1;
  }
  Escher::ButtonCell* buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    assert(index == 0 &&
           position == Escher::ButtonRowController::Position::Top);
    return const_cast<Escher::SimpleButtonCell*>(&m_dataTypeButton);
  }

  // TabTableController
  Escher::Responder* tabController() const override { return m_tabController; }
  Escher::SelectableTableView* selectableTableView() override {
    return &m_selectableTableView;
  }

  void pushTypeController() {
    m_stackViewController->push(m_dataTypeController);
  }

  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  bool recomputeDimensions();
  void recomputeDimensionsAndReload();

  static constexpr int k_maxNumberOfColumns =
      TableData::k_maxNumberOfGroups + 1;
  static constexpr int k_maxNumberOfRows = TableData::k_maxNumberOfCategory + 1;

  Escher::SimpleButtonCell m_dataTypeButton;
  Escher::SelectableTableView m_selectableTableView;
  TableData* m_tableData;

  Escher::StackViewController* m_stackViewController;
  Escher::TabViewController* m_tabController;
  Escher::ViewController* m_dataTypeController;
};

}  // namespace Statistics::Categorical
