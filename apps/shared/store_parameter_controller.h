#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/message_table_cell_with_message_with_switch.h>
#include <escher/message_table_cell.h>
#include <escher/buffer_table_cell.h>
#include <apps/i18n.h>
#include "column_parameter_controller.h"

namespace Shared {

class StoreController;

class StoreParameterController : public ColumnParameterController {
public:
  StoreParameterController(Escher::Responder * parentResponder, StoreController * storeController);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return numberOfCells(); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void initializeColumnParameters() override;
protected:
  constexpr static int k_numberOfCells = 4;
  virtual int numberOfCells() const { return k_numberOfCells; }
  StoreController * m_storeController;
  virtual int indexOfHideColumn() const { return k_indexOfFillFormula + 1; }
  int indexOfClearColumn() const { return indexOfHideColumn() + 1; }

  Escher::BufferTableCell m_clearColumn;
  Escher::MessageTableCell m_fillFormula;
private:
  EditableCellTableViewController * editableCellTableViewController() override;
  virtual I18n::Message sortMessage() { return I18n::Message::SortValues; }
  constexpr static int k_indexOfSortCell = 0;
  constexpr static int k_indexOfFillFormula = k_indexOfSortCell + 1;

  Escher::MessageTableCellWithMessage m_sortCell;
  Escher::MessageTableCellWithMessageWithSwitch m_hideCell;
};

}

#endif
