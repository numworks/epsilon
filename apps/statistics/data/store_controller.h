#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <apps/shared/store_controller.h>

#include "../store.h"
#include "store_parameter_controller.h"

namespace Statistics {

class StoreController : public Shared::StoreController {
 public:
  constexpr static char k_tableName[] = "V%c/N%c ";
  constexpr static size_t k_tableNameSize = sizeof(k_tableName);
  static void FillSeriesName(int series, char* buffer,
                             bool withFinalSpace = false);

  StoreController(Escher::Responder* parentResponder,
                  Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
                  Store* store, Escher::ButtonRowController* header,
                  Poincare::Context* parentContext);

  /* Shared::StoreController */
  void sortSelectedColumn() override;
  int fillColumnName(int column, char* buffer) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override {
    return type == k_nonEditableCellType
               ? &m_nonEditableCells[index]
               : Shared::StoreController::reusableCell(index, type);
  }
  int reusableCellCount(int type) override {
    return type == k_nonEditableCellType
               ? k_maxNumberOfNonEditableCells
               : Shared::StoreController::reusableCellCount(type);
  }
  int typeAtLocation(int column, int row) override {
    return isCumulatedFrequencyCell(column, row)
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
  bool isCumulatedFrequencyCell(int i, int j) const {
    return j != 0 && isCumulatedFrequencyColumn(i);
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
  void setClearPopUpContent() override;
  bool isColumnClearable(int column) override {
    return !isCumulatedFrequencyColumn(column);
  }

  Store* m_store;
  StoreParameterController m_storeParameterController;
  Escher::FloatEvenOddBufferTextCell<>
      m_nonEditableCells[k_maxNumberOfNonEditableCells];
};

}  // namespace Statistics

#endif
