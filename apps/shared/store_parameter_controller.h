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
  int numberOfRows() const override { return k_numberOfCells; }
  int reusableCellCount(int type) override { return 1; }
  int typeAtIndex(int index) override { assert(index < k_numberOfCells); return index; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void initializeColumnParameters() override;
protected:
  // For these cells, type defines the orders
  constexpr static int k_sortCellType = 0;
  constexpr static int k_fillFormulaCellType = k_sortCellType + 1;
  constexpr static int k_hideCellType = k_fillFormulaCellType + 1;
  constexpr static int k_clearCellType = k_hideCellType + 1;
  constexpr static int k_numberOfCells = k_clearCellType + 1;

  StoreController * m_storeController;
  Escher::BufferTableCell m_clearColumn;
private:
  EditableCellTableViewController * editableCellTableViewController() override;
  virtual I18n::Message sortMessage() { return I18n::Message::SortValues; }

  Escher::MessageTableCell m_fillFormula;
  Escher::MessageTableCellWithMessage m_sortCell;
  Escher::MessageTableCellWithMessageWithSwitch m_hideCell;
};

}

#endif
