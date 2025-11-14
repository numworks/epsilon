#pragma once

#include <escher/selectable_table_view.h>

namespace Statistics {

class CalculationSelectableTableView : public Escher::SelectableTableView {
 public:
  using Escher::SelectableTableView::SelectableTableView;
  bool handleEvent(Ion::Events::Event event) override;
};

}  // namespace Statistics
