#include "selectable_table_view.h"
#include <algorithm>

namespace Calculation {

CalculationSelectableTableView::CalculationSelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
 SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  ::SelectableTableView(parentResponder, dataSource, selectionDataSource, delegate)
{
  setVerticalCellOverlap(0);
  setMargins(0);
  setDecoratorType(ScrollView::Decorator::Type::None);
}

void CalculationSelectableTableView::scrollToBottom() {
  KDCoordinate contentOffsetX = contentOffset().x();
  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(dataSource()->numberOfRows()) - maxContentHeightDisplayableWithoutScrolling();
  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
}

void CalculationSelectableTableView::scrollToCell(int i, int j) {
  if (m_contentView.bounds().height() < bounds().height()) {
    setTopMargin(bounds().height() - m_contentView.bounds().height());
  } else {
    setTopMargin(0);
  }
  ::SelectableTableView::scrollToCell(i, j);
  ScrollView::layoutSubviews();
  if (m_contentView.bounds().height() - contentOffset().y() < bounds().height()) {
    // Avoid empty space at the end of the table
    scrollToBottom();
  }
}

void CalculationSelectableTableView::scrollToSubviewOfTypeOfCellAtLocation(HistoryViewCellDataSource::SubviewType subviewType, int i, int j) {
  if (dataSource()->rowHeight(j) <= bounds().height()) {
    return;
  }
  /* As we scroll, the selected calculation does not use the same history view
   * cell, thus, we want to deselect the previous used history view cell. (*) */
  unhighlightSelectedCell();

  /* Main part of the scroll */
  HistoryViewCell * cell = static_cast<HistoryViewCell *>(selectedCell());
  assert(cell);
  KDCoordinate contentOffsetX = contentOffset().x();

  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(j);
  if (cell->displaysSingleLine() && dataSource()->rowHeight(j) > maxContentHeightDisplayableWithoutScrolling()) {
    /* If we cannot display the full calculation, we display the selected
     * layout as close as possible to the top of the screen without drawing
     * empty space between the history and the input field.
     *
     * Below are some values we can assign to contentOffsetY, and the kinds of
     * display they entail :
     * (the selected cell is at index j)
     *
     * 1 - cumulatedHeightFromIndex(j)
     *   Aligns the top of the cell with the top of the zone in which the
     *   history can be drawn.
     *
     * 2 - (cumulatedHeightFromIndex(j+1)
     *      - maxContentHeightDisplayableWithoutScrolling())
     *   Aligns the bottom of the cell with the top of the input field.
     *
     * 3 - cumulatedHeightFromIndex(j) + baseline1 - baseline2
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
    contentOffsetY += std::min(
        dataSource()->rowHeight(j) - maxContentHeightDisplayableWithoutScrolling(),
        std::max(0, (cell->inputView()->layout().baseline() - cell->outputView()->baseline()) * (subviewType == HistoryViewCellDataSource::SubviewType::Input ? -1 : 1)));
  } else if (subviewType != HistoryViewCellDataSource::SubviewType::Input) {
    contentOffsetY += dataSource()->rowHeight(j) - maxContentHeightDisplayableWithoutScrolling();
  }

  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  /* For the same reason as (*), we have to rehighlight the new history view
   * cell and reselect the first responder.
   * We have to recall "selectedCell" because when the table might have been
   * relayouted in "setContentOffset".*/
  cell = static_cast<HistoryViewCell *>(selectedCell());
  assert(cell);
  cell->setHighlighted(true);
  Container::activeApp()->setFirstResponder(cell);
}


}
