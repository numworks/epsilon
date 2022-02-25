#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/message_table_cell.h>
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
  KDCoordinate nonMemoizedRowHeight(int index) override;

  int reusableCellCount(int type) override { return type == k_defaultCellType ? 2 : 1; }
  int typeAtIndex(int index) override { return index == k_indexOfSortCell ? k_sortCellType : k_defaultCellType; }

  virtual void initializeColumnParameters() override;
protected:
  constexpr static int k_numberOfCells = 3;
  virtual int numberOfCells() const { return k_numberOfCells; }
  StoreController * m_storeController;

private:
  EditableCellTableViewController * editableCellTableViewController() override;
  virtual I18n::Message sortMessage() { return I18n::Message::SortValues; }
  constexpr static int k_indexOfSortCell = 0;
  constexpr static int k_indexOfFillFormula = k_indexOfSortCell + 1;
  constexpr static int k_indexOfClearColumn = k_indexOfFillFormula + 1;
  constexpr static int k_defaultCellType = 0;
  constexpr static int k_sortCellType = 1;

  Escher::MessageTableCellWithMessage m_sortCell;
  Escher::MessageTableCell m_fillFormula;
  Escher::MessageTableCell m_clearColumn;

};

}

#endif
