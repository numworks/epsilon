#include "calculation_selectable_list_view.h"

#include <algorithm>

using namespace Escher;

namespace Calculation {

CalculationSelectableListView::CalculationSelectableListView(
    Responder* parentResponder, ListViewDataSource* dataSource,
    SelectableListViewDataSource* selectionDataSource,
    SelectableListViewDelegate* delegate)
    : ::SelectableListView(parentResponder, dataSource, selectionDataSource,
                           delegate) {
  setVerticalCellOverlap(0);
  resetMargins();
  hideScrollBars();
}

void CalculationSelectableListView::didChangeSelectionAndDidScroll() {
  TableView::layoutSubviews();
  if (m_contentView.bounds().height() - contentOffset().y() <
      bounds().height()) {
    // Avoid empty space at the end of the table
    scrollToBottom();
  }
}

void CalculationSelectableListView::scrollToSubviewOfTypeOfCellAtRow(
    HistoryViewCellDataSource::SubviewType subviewType, int row) {
  if (dataSource()->rowHeight(row) <= bounds().height()) {
    return;
  }

  /* Main part of the scroll */
  HistoryViewCell* cell = static_cast<HistoryViewCell*>(selectedCell());
  assert(cell);
  KDCoordinate contentOffsetX = contentOffset().x();

  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightBeforeRow(row);
  if (cell->displaysSingleLine() &&
      dataSource()->rowHeight(row) >
          maxContentHeightDisplayableWithoutScrolling()) {
    /* If we cannot display the full calculation, we display the selected
     * layout as close as possible to the top of the screen without drawing
     * empty space between the history and the input field.
     *
     * Below are some values we can assign to contentOffsetY, and the kinds of
     * display they entail :
     * (the selected cell is at index j)
     *
     * 1 - cumulatedHeightBeforeRow(j)
     *   Aligns the top of the cell with the top of the zone in which the
     *   history can be drawn.
     *
     * 2 - (cumulatedHeightBeforeRow(j+1)
     *      - maxContentHeightDisplayableWithoutScrolling())
     *   Aligns the bottom of the cell with the top of the input field.
     *
     * 3 - cumulatedHeightBeforeRow(j) + baseline1 - baseline2
     *   Aligns the top of the selected layout with the top of the screen (only
     *   used when the selected layout is the smallest).
     *
     * The following drawing shows where the calculation would be aligned with
     * each value of contentOffsetY, for the calculation (1/3)/(4/2) = 1/6.
     *
     *        (1)                 (2)                 (3)
     *  +--------------+    +--------------+    +--------------+
     *  |  1           |    | ---        - |    |  3         1 |
     *  |  -           |    |  4         6 |    | ---        - |
     *  |  3         1 |    |  -           |    |  4         6 |
     *  | ---        - |    |  2           |    |  -           |
     *  +--------------+    +--------------+    +--------------+
     *  | (1/3)/(4/2)  |    | (1/3)/(4/2)  |    | (1/3)/(4/2)  |
     *  +--------------+    +--------------+    +--------------+
     *
     * */
    Poincare::Layout inputLayout = cell->inputView()->layout();
    /* Default value of 0 for the baseline is the same as the one given to the
     * output baseline in
     * AbstractScrollableMultipleLayoutsView::ContentCell::baseline. */
    KDCoordinate inputBaseline =
        inputLayout.isUninitialized()
            ? 0
            : inputLayout->baseline(cell->inputView()->font());
    contentOffsetY += std::min(
        dataSource()->rowHeight(row) -
            maxContentHeightDisplayableWithoutScrolling(),
        std::max(
            0, (inputBaseline - cell->outputView()->baseline()) *
                   (subviewType == HistoryViewCellDataSource::SubviewType::Input
                        ? -1
                        : 1)));
  } else if (subviewType != HistoryViewCellDataSource::SubviewType::Input) {
    contentOffsetY += dataSource()->rowHeight(row) -
                      maxContentHeightDisplayableWithoutScrolling();
  }

  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  /* For the same reason as (*), we have to rehighlight the new history view
   * cell and reselect the first responder.
   * We have to recall "selectedCell" because when the table might have been
   * relayouted in "setContentOffset".*/
  cell = static_cast<HistoryViewCell*>(selectedCell());
  assert(cell);
  App::app()->setFirstResponder(cell, true);
}

void CalculationSelectableListView::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::DidEnter) {
    resetSizeAndOffsetMemoization();
    SelectableTableView::handleResponderChainEvent(event);
  }
}

}  // namespace Calculation
