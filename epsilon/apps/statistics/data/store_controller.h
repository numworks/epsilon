#pragma once

#include <apps/shared/store_controller.h>

#include "../statistics_type_controller.h"
#include "../store.h"
#include "store_parameter_controller.h"

namespace Statistics {

class StoreController : public Shared::StoreController {
 public:
  StoreController(Escher::Responder* parentResponder, Store* store,
                  Escher::ButtonRowController* header,
                  Escher::StackViewController* stackViewController,
                  Escher::ViewController* dataTypeController);

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

  /* Shared::StoreController */
  void sortSelectedColumn() override;
  size_t fillColumnName(int column, char* buffer) override;
  Escher::TabViewController* tabController() const override {
    return static_cast<Escher::TabViewController*>(parentResponder()
                                                       ->parentResponder()
                                                       ->parentResponder()
                                                       ->parentResponder());
  }
  Escher::StackViewController* stackController() const override {
    return m_stackViewController;
  }

  // TableViewDataSource
  int numberOfColumns() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override {
    return type == k_nonEditableCellType
               ? &m_nonEditableCells[index]
               : Shared::StoreController::reusableCell(index, type);
  }
  int reusableCellCount(int type) const override {
    return type == k_nonEditableCellType
               ? k_maxNumberOfNonEditableCells
               : Shared::StoreController::reusableCellCount(type);
  }
  int typeAtLocation(int column, int row) const override {
    return row != 0 && isCumulatedFrequencyColumn(column)
               ? k_nonEditableCellType
               : Shared::StoreController::typeAtLocation(column, row);
  }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

 private:
  constexpr static int k_cumulatedFrequencyRelativeColumn = 2;
  constexpr static int k_columnTitleSize = 50;
  constexpr static int k_nonEditableCellType =
      Shared::StoreController::k_editableCellType + 1;
  // At most 1 non editable column every 3 columns
  constexpr static int k_maxNumberOfDisplayableNonEditableColumns =
      1 + k_maxNumberOfDisplayableColumns / 3;
  constexpr static int k_maxNumberOfNonEditableCells =
      k_maxNumberOfDisplayableRows * k_maxNumberOfDisplayableNonEditableColumns;

  bool isCumulatedFrequencyColumn(int i) const {
    return m_store->relativeColumn(i) == k_cumulatedFrequencyRelativeColumn;
  }

  bool deleteCellValue(int series, int i, int j,
                       bool authorizeNonEmptyRowDeletion = true) override;
  Escher::InputViewController* inputViewController() override;
  Shared::ColumnParameterController* columnParameterController() override {
    return &m_storeParameterController;
  }
  Shared::ColumnParameters* columnParameters() override {
    return &m_storeParameterController;
  }
  bool setDataAtLocation(double floatBody, int column, int row) override;
  double dataAtLocation(int column, int row) override;
  void setTitleCellText(Escher::HighlightCell* titleCell, int column) override;
  void clearSelectedColumn() override;
  bool isColumnClearable(int column) override {
    return !isCumulatedFrequencyColumn(column);
  }

  void pushTypeController() { stackController()->push(m_dataTypeController); }

  Store* m_store;
  Escher::StackViewController* m_stackViewController;
  StoreParameterController m_storeParameterController;
  Escher::FloatEvenOddBufferTextCell<>
      m_nonEditableCells[k_maxNumberOfNonEditableCells];
  Escher::ViewController* m_dataTypeController;
  Escher::SimpleButtonCell m_dataTypeButton;
};

}  // namespace Statistics
