#ifndef PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H
#define PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H

#include <escher/selectable_table_view.h>

namespace Probability {

class SelectableTableViewWithBackground : public Escher::SelectableTableView, public Escher::ScrollViewDelegate {
public:
  using Escher::SelectableTableView::SelectableTableView;
  void scrollViewDidChangeOffset(Escher::ScrollViewDataSource * scrollViewDataSource) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H */
