#ifndef CALCULATION_SELECTABLE_TABLE_VIEW_H
#define CALCULATION_SELECTABLE_TABLE_VIEW_H

#include <escher.h>

namespace Calculation {

class CalculationSelectableTableView : public ::SelectableTableView {
public:
  CalculationSelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
    SelectableTableViewDelegate * delegate = nullptr);
  void scrollToCell(int i, int j) override;
};

}

#endif
