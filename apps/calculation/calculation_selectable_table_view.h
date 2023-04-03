#ifndef CALCULATION_SELECTABLE_TABLE_VIEW_H
#define CALCULATION_SELECTABLE_TABLE_VIEW_H

#include <escher/container.h>
#include <escher/selectable_table_view.h>

#include "history_view_cell.h"

namespace Calculation {

class CalculationSelectableTableView : public Escher::SelectableTableView {
 public:
  CalculationSelectableTableView(
      Escher::Responder* parentResponder,
      Escher::TableViewDataSource* dataSource,
      Escher::SelectableTableViewDataSource* selectionDataSource,
      Escher::SelectableTableViewDelegate* delegate = nullptr);
  void scrollToBottom();
  void scrollToCell(int i, int j) override;
  void scrollToSubviewOfTypeOfCellAtLocation(
      HistoryViewCellDataSource::SubviewType subviewType, int i, int j);
};

}  // namespace Calculation

#endif
