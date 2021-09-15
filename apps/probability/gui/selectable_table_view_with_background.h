#ifndef PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H
#define PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H

#include <escher/selectable_table_view.h>

namespace Probability {

class SelectableTableViewWithBackground : public Escher::SelectableTableView {
public:
  using Escher::SelectableTableView::SelectableTableView;
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H */
