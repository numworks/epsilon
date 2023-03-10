#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_table_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_table_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/switch_view.h>

#include "column_parameter_controller.h"

namespace Shared {

class StoreController;

class StoreParameterController : public ColumnParameterController {
 public:
  StoreParameterController(Escher::Responder* parentResponder,
                           StoreColumnHelper* storeColumnHelper);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfCells; }
  int reusableCellCount(int type) override { return 1; }
  int typeAtIndex(int index) const override {
    assert(index < k_numberOfCells);
    return index;
  }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  void initializeColumnParameters() override;

 protected:
  // For these cells, type defines the orders
  constexpr static int k_sortCellType = 0;
  constexpr static int k_fillFormulaCellType = k_sortCellType + 1;
  constexpr static int k_hideCellType = k_fillFormulaCellType + 1;
  constexpr static int k_clearCellType = k_hideCellType + 1;
  constexpr static int k_numberOfCells = k_clearCellType + 1;

  StoreColumnHelper* m_storeColumnHelper;
  Escher::BufferTableCell m_clearColumn;

 private:
  ColumnNameHelper* columnNameHelper() override;
  virtual I18n::Message sortMessage() { return I18n::Message::SortValues; }

  Escher::MessageTableCell m_fillFormula;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView> m_sortCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_hideCell;
};

}  // namespace Shared

#endif
