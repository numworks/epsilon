#pragma once

#include <apps/i18n.h>
#include <apps/inference/text_helpers.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/highlight_cell.h>
#include <escher/prefaced_twice_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <shared/math_field_delegate.h>
#include <shared/tab_table_controller.h>

#include "column_parameter_controller.h"
#include "rf_column_parameter_controller.h"
#include "row_parameter_controller.h"
#include "store.h"
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
                  Escher::ViewController* dataTypeController, Store* store);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  // Categorical::TableViewDataSource
  const Store* store() const override { return m_store; }

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
  void viewWillAppear() override;
  Escher::Responder* tabController() const override { return m_tabController; }
  Escher::SelectableTableView* selectableTableView() override {
    return &m_selectableTableView;
  }
  Escher::View* view() override { return &m_prefacedTableView; }

  void pushTypeController() {
    m_stackViewController->push(m_dataTypeController);
  }

  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  bool recomputeDimensions();
  void recomputeDimensionsAndReload(bool force);

  static constexpr int k_maxNumberOfColumns = Store::k_maxNumberOfGroups + 1;
  static constexpr int k_maxNumberOfRows = Store::k_maxNumberOfCategory + 1;

  Escher::SimpleButtonCell m_dataTypeButton;
  Escher::SelectableTableView m_selectableTableView;
  Escher::PrefacedTwiceTableView m_prefacedTableView;
  Store* m_store;

  ColumnParameterController m_columnParameterController;
  RFColumnParameterController m_rfColumnParameterController;
  RowParameterController m_rowParameterController;
  Escher::StackViewController* m_stackViewController;
  Escher::TabViewController* m_tabController;
  Escher::ViewController* m_dataTypeController;
};

}  // namespace Statistics::Categorical
