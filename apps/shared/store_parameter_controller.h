#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/message_table_cell.h>
#include <apps/i18n.h>
#include "double_pair_store.h"

namespace Shared {

class StoreController;

class StoreParameterController : public Escher::SelectableListViewController {
public:
  StoreParameterController(Escher::Responder * parentResponder, DoublePairStore * store, StoreController * storeController);
  void selectXColumn(bool xColumnSelected) { m_xColumnSelected = xColumnSelected; }
  void selectSeries(int series) {
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);
    m_series = series;
  }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return type == k_defaultCellType ? k_totalNumberOfCell - 1 : 1; }
  int typeAtIndex(int index) override { return index == k_indexOfSortValues ? k_sortCellType : k_defaultCellType; }
  KDCoordinate nonMemoizedRowHeight(int index) override;
protected:
  constexpr static int k_totalNumberOfCell = 3;
  DoublePairStore * m_store;
  int m_series;
private:
  void popFromStackView();
  virtual I18n::Message sortMessage() { return m_xColumnSelected ? I18n::Message::SortValues : I18n::Message::SortSizes; }
  void sortColumn();

  constexpr static int k_indexOfSortValues = 0;
  constexpr static int k_indexOfFillFormula = k_indexOfSortValues + 1;
  constexpr static int k_indexOfRemoveColumn = k_indexOfFillFormula + 1;
  constexpr static int k_defaultCellType = 0;
  constexpr static int k_sortCellType = 1;

  Escher::MessageTableCell m_cells[k_totalNumberOfCell-1];
  Escher::MessageTableCellWithMessage m_sortCell;
  StoreController * m_storeController;
  bool m_xColumnSelected;
};

}

#endif /* SHARED_STORE_PARAM_CONTROLLER_H */
