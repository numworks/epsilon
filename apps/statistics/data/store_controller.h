#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include "../store.h"
#include "store_parameter_controller.h"
#include <apps/shared/store_controller.h>
#include <apps/shared/hideable_even_odd_buffer_text_cell.h>

namespace Statistics {

class StoreController : public Shared::StoreController {
public:
  constexpr static char k_tableName[] = "V%c/N%c ";
  constexpr static size_t k_tableNameSize = sizeof(k_tableName);
  static void FillSeriesName(int series, char * buffer, bool withFinalSpace = false);

  StoreController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Escher::ButtonRowController * header, Poincare::Context * parentContext);

  /* Shared::StoreController */
  void sortSelectedColumn() override;
  int fillColumnName(int columnIndex, char * buffer) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override { return type == k_nonEditableCellType ? &m_nonEditableCells[index] : Shared::StoreController::reusableCell(index, type); }
  int reusableCellCount(int type) override { return type == k_nonEditableCellType ? k_maxNumberOfNonEditableCells : Shared::StoreController::reusableCellCount(type); }
  int typeAtLocation(int i, int j) override { return isCumulatedFrequencyCell(i, j) ? k_nonEditableCellType : Shared::StoreController::typeAtLocation(i, j); }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

private:
  constexpr static int k_cumulatedFrequencyRelativeColumnIndex = 2;
  constexpr static int k_columnTitleSize = 50;
  constexpr static int k_nonEditableCellType = Shared::StoreController::k_editableCellType + 1;
  // At most 1 non editable column every 3 columns
  constexpr static int k_maxNumberOfDisplayableNonEditableColumns = 1 + k_maxNumberOfDisplayableColumns / 3;
  constexpr static int k_maxNumberOfNonEditableCells = k_maxNumberOfDisplayableRows * k_maxNumberOfDisplayableNonEditableColumns;

  bool isCumulatedFrequencyColumn(int i) const { return m_store->relativeColumnIndex(i) == k_cumulatedFrequencyRelativeColumnIndex; }
  bool isCumulatedFrequencyCell(int i, int j) const { return j != 0 && isCumulatedFrequencyColumn(i); }

  bool deleteCellValue(int series, int i, int j, bool authorizeNonEmptyRowDeletion = true) override;
  Escher::InputViewController * inputViewController() override;
  Shared::ColumnParameterController * columnParameterController() override { return &m_storeParameterController; }
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void clearSelectedColumn() override;
  void setClearPopUpContent() override;
  bool isColumnClearable(int columnIndex) override { return !isCumulatedFrequencyColumn(columnIndex); }

  Store * m_store;
  StoreParameterController m_storeParameterController;
  Shared::HideableEvenOddBufferTextCell m_nonEditableCells[k_maxNumberOfNonEditableCells];
};

}

#endif
