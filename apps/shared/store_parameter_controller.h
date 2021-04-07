#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_message.h>
#include "double_pair_store.h"
#include <apps/i18n.h>

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
protected:
  DoublePairStore * m_store;
  int m_series;
private:
  virtual I18n::Message sortMessage() { return m_xColumnSelected ? I18n::Message::SortValues : I18n::Message::SortSizes; }
  constexpr static int k_totalNumberOfCell = 3;
  Escher::MessageTableCellWithMessage m_cells[k_totalNumberOfCell];
  StoreController * m_storeController;
  bool m_xColumnSelected;
};

}

#endif
