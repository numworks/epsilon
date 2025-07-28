#ifndef CALCULATION_SELECTABLE_LIST_VIEW_H
#define CALCULATION_SELECTABLE_LIST_VIEW_H

#include <escher/container.h>
#include <escher/selectable_list_view.h>

#include "history_view_cell.h"

namespace Calculation {

class CalculationSelectableListView : public Escher::SelectableListView {
 public:
  CalculationSelectableListView(
      Escher::Responder* parentResponder,
      Escher::ListViewDataSource* dataSource,
      Escher::SelectableListViewDataSource* selectionDataSource,
      Escher::SelectableListViewDelegate* delegate = nullptr);
  void didChangeSelectionAndDidScroll();
  void scrollToSubviewOfTypeOfCellAtRow(
      HistoryViewCellDataSource::SubviewType subviewType, int row);

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;
};

}  // namespace Calculation

#endif
