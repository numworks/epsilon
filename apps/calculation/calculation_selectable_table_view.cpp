#include "calculation_selectable_table_view.h"

#include <algorithm>

using namespace Escher;

namespace Calculation {

CalculationSelectableTableView::CalculationSelectableTableView(
    Responder *parentResponder, TableViewDataSource *dataSource,
    SelectableTableViewDataSource *selectionDataSource,
    SelectableTableViewDelegate *delegate)
    : ::SelectableTableView(parentResponder, dataSource, selectionDataSource,
                            delegate) {
  setVerticalCellOverlap(0);
  setMargins(0);
  hideScrollBars();
}

void CalculationSelectableTableView::scrollToBottom() {
  KDCoordinate contentOffsetX = contentOffset().x();
  KDCoordinate contentOffsetY =
      dataSource()->cumulatedHeightBeforeIndex(dataSource()->numberOfRows()) -
      maxContentHeightDisplayableWithoutScrolling();
  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
}

void CalculationSelectableTableView::scrollToCell(int i, int j) {
  ::SelectableTableView::scrollToCell(i, j);
  ScrollView::layoutSubviews();
  if (m_contentView.bounds().height() - contentOffset().y() <
      bounds().height()) {
    // Avoid empty space at the end of the table
    scrollToBottom();
  }
}

void CalculationSelectableTableView::scrollToSubviewOfTypeOfCellAtLocation(
    HistoryViewCellDataSource::SubviewType subviewType, int i, int j) {
  if (dataSource()->rowHeight(j) <= bounds().height()) {
    return;
  }

  /* Main part of the scroll */
  HistoryViewCell *cell = static_cast<HistoryViewCell *>(selectedCell());
  assert(cell);
  KDCoordinate contentOffsetX = contentOffset().x();

  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightBeforeIndex(j);
  if (cell->displaysSingleLine() &&
      dataSource()->rowHeight(j) >
          maxContentHeightDisplayableWithoutScrolling()) {
    /* If we cannot display the full calculation, we display the selected
     * layout as close as possible to the top of the screen without drawing
     * empty space between the history and the input field.
     *
     * Below are some values we can assign to contentOffsetY, and the kinds of
     * display they entail :
     * (the selected cell is at index j)
     *
     * 1 - cumulatedHeightBeforeIndex(j)
     *   Aligns the top of the cell with the top of the zone in which the
     *   history can be drawn.
     *
     * 2 - (cumulatedHeightBeforeIndex(j+1)
     *      - maxContentHeightDisplayableWithoutScrolling())
     *   Aligns the bottom of the cell with the top of the input field.
     *
     * 3 - cumulatedHeightBeforeIndex(j) + baseline1 - baseline2
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
     * AbstractScrollableMultipleExpressionsView::ContentCell::baseline. */
    KDCoordinate inputBaseline =
        inputLayout.isUninitialized()
            ? 0
            : inputLayout.baseline(cell->inputView()->font());
    contentOffsetY += std::min(
        dataSource()->rowHeight(j) -
            maxContentHeightDisplayableWithoutScrolling(),
        std::max(
            0, (inputBaseline - cell->outputView()->baseline()) *
                   (subviewType == HistoryViewCellDataSource::SubviewType::Input
                        ? -1
                        : 1)));
  } else if (subviewType != HistoryViewCellDataSource::SubviewType::Input) {
    contentOffsetY += dataSource()->rowHeight(j) -
                      maxContentHeightDisplayableWithoutScrolling();
  }

  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  /* For the same reason as (*), we have to rehighlight the new history view
   * cell and reselect the first responder.
   * We have to recall "selectedCell" because when the table might have been
   * relayouted in "setContentOffset".*/
  cell = static_cast<HistoryViewCell *>(selectedCell());
  assert(cell);
  Container::activeApp()->setFirstResponder(cell);
}

}  // namespace Calculation
