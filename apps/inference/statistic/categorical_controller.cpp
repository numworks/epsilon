#include "categorical_controller.h"

#include <escher/invocation.h>

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

CategoricalController::CategoricalController(Responder* parent,
                                             ViewController* nextController,
                                             Invocation invocation)
    : SelectableListViewController<ListViewDataSource>(parent, this),
      m_nextController(nextController),
      m_next(&m_selectableListView, I18n::Message::Next, invocation) {
  m_selectableListView.margins()->setTop(0);
  m_selectableListView.margins()->setHorizontal({0, 0});
  m_selectableListView.setBackgroundColor(Palette::WallScreenDark);
  setScrollViewDataSourceDelegate(this);
}

bool CategoricalController::ButtonAction(CategoricalController* controller,
                                         void* s) {
  controller->stackOpenPage(controller->m_nextController);
  return true;
}

void CategoricalController::didScroll() {
  /* Transfer the CategoricalController offset to the CategoricalTableCell
   * offset. This is a hack to ensure that the categorical table cell doesn't
   * require too many displayable cells. If the scroll was handled by the
   * CategoricalController, the CategoricalTableCell would need as many
   * displayable cells as its real number of cells. Since the
   * CategoricalController needs at most 3 cells, we delegate the scroll
   * handling to the CategoricalTableCell. */
  KDCoordinate listOffset = listVerticalOffset();
  KDCoordinate tableOffset = tableCellVerticalOffset();

  int tableCellRow = indexOfTableCell();
  KDCoordinate topMargin = m_selectableListView.margins()->top();
  int firstVisibleRow =
      rowAfterCumulatedHeight(std::max(listOffset - topMargin, 0));
  KDCoordinate heightBeforeTableCell =
      cumulatedHeightBeforeRow(tableCellRow) + topMargin;

  assert((firstVisibleRow < tableCellRow) ==
         (listOffset < heightBeforeTableCell));

  KDCoordinate maxTableOffset = std::max(
      0, tableCellFullHeight() - (m_selectableListView.bounds().height() -
                                  m_selectableListView.margins()->height()));
  assert(tableOffset <= maxTableOffset);

  if (firstVisibleRow == tableCellRow && listOffset > heightBeforeTableCell &&
      tableOffset < maxTableOffset) {
    // Transfer list's offset to table
    KDCoordinate translation = std::min(maxTableOffset - tableOffset,
                                        listOffset - heightBeforeTableCell);
    assert(translation > 0);
    // Change table cell offset first (to relayout well the list)
    categoricalTableCell()->selectableTableView()->translateContentOffsetBy(
        KDPoint(0, translation));
    assert(0 <= tableCellVerticalOffset() &&
           tableCellVerticalOffset() <= maxTableOffset);
    m_selectableListView.translateContentOffsetBy(KDPoint(0, -translation));
    assert((listVerticalOffset() == heightBeforeTableCell &&
            tableCellVerticalOffset() <= maxTableOffset) ||
           (listVerticalOffset() >= heightBeforeTableCell &&
            tableCellVerticalOffset() == maxTableOffset));
  } else if (firstVisibleRow < tableCellRow && tableOffset > 0) {
    assert(listOffset < heightBeforeTableCell);
    // Transfer table's offset to list
    KDCoordinate translation = std::min(static_cast<int>(tableOffset),
                                        heightBeforeTableCell - listOffset);
    assert(translation > 0);
    // Change table cell offset first (to relayout well the list)
    categoricalTableCell()->selectableTableView()->translateContentOffsetBy(
        KDPoint(0, -translation));
    assert(0 <= tableCellVerticalOffset() &&
           tableCellVerticalOffset() <= maxTableOffset);
    m_selectableListView.translateContentOffsetBy(KDPoint(0, translation));
    assert(0 <= listVerticalOffset() &&
           listVerticalOffset() <= heightBeforeTableCell);
    assert((listVerticalOffset() == heightBeforeTableCell &&
            tableCellVerticalOffset() >= 0) ||
           (listVerticalOffset() <= heightBeforeTableCell &&
            tableCellVerticalOffset() == 0));
  }
}

bool CategoricalController::updateBarIndicator(bool vertical, bool* visible) {
  assert(visible);
  if (!vertical) {
    return false;
  }

  ScrollView::BarDecorator* decorator =
      static_cast<ScrollView::BarDecorator*>(m_selectableListView.decorator());
  KDCoordinate otherCellsHeight =
      m_selectableListView.minimalSizeForOptimalDisplay().height() -
      categoricalTableCell()->bounds().height();
  KDCoordinate trueOptimalHeight = tableCellFullHeight() + otherCellsHeight;
  *visible = decorator->verticalBar()->update(
      trueOptimalHeight, tableCellVerticalOffset() + listVerticalOffset(),
      m_selectableListView.bounds().height());
  return true;
}

KDRect CategoricalController::visibleRectInBounds(ScrollView* scrollView) {
  assert(scrollView == categoricalTableCell()->selectableTableView());

  KDCoordinate listVisibleStart = listVerticalOffset();
  KDCoordinate listVisibleHeight = m_selectableListView.bounds().height();
  KDCoordinate listVisibleEnd = listVisibleStart + listVisibleHeight;

  KDSize tableBounds =
      categoricalTableCell()->selectableTableView()->bounds().size();
  assert(0 <= tableBounds.height() &&
         tableBounds.height() <= tableCellFullHeight());

  KDCoordinate heightBeforeTableCell =
      cumulatedHeightBeforeRow(indexOfTableCell()) +
      m_selectableListView.margins()->top();
  KDCoordinate heightAfterTableCell =
      heightBeforeTableCell + tableBounds.height();
  assert(heightBeforeTableCell < listVisibleEnd);
  assert(heightAfterTableCell > listVisibleStart);

  KDCoordinate tableVisibleStart = 0;
  KDCoordinate tableVisibleHeight = tableBounds.height();
  if (heightBeforeTableCell < listVisibleStart) {
    KDCoordinate diff = listVisibleStart - heightBeforeTableCell;
    tableVisibleStart += diff;
    tableVisibleHeight -= diff;
  }
  if (heightAfterTableCell > listVisibleEnd) {
    KDCoordinate diff = heightAfterTableCell - listVisibleEnd;
    tableVisibleHeight -= diff;
  }

  assert(0 <= tableVisibleStart && 0 <= tableVisibleHeight &&
         tableVisibleStart + tableVisibleHeight <= tableCellFullHeight());

  return KDRect(KDPoint(0, tableVisibleStart),
                KDSize(tableBounds.width(), tableVisibleHeight));
}

void CategoricalController::listViewDidChangeSelectionAndDidScroll(
    SelectableListView* l, int previousRow, KDPoint previousOffset,
    bool withinTemporarySelection) {
  assert(l == &m_selectableListView);
#if 0
  if (previousRow != l->selectedRow() &&
      l->selectedRow() == indexOfTableCell()) {
    // Restore offset and let table cell scroll if necessary
    m_selectableListView.setContentOffset(previousOffset);
  }
#endif
  if (previousRow == l->selectedRow() || withinTemporarySelection) {
    return;
  }
  if (previousRow == indexOfTableCell()) {
    categoricalTableCell()->selectRow(-1);
    categoricalTableCell()->layoutSubviews(true);
  } else if (l->selectedRow() == indexOfTableCell() && previousRow >= 0) {
    int rowToSelect;
    if (previousRow < l->selectedRow()) {
      rowToSelect = 0;
    } else {
      assert(previousRow > l->selectedRow());
      rowToSelect =
          categoricalTableCell()->tableViewDataSource()->numberOfRows() - 1;
    }
    categoricalTableCell()->selectRow(rowToSelect);
  }
  if (m_selectableListView.contentOffset() != previousOffset) {
    didScroll();
  }
}

HighlightCell* CategoricalController::reusableCell(int index, int type) {
  assert(index == 0);
  if (type == indexOfTableCell()) {
    return categoricalTableCell();
  }
  return explicitCellAtRow(type);
}

HighlightCell* CategoricalController::explicitCellAtRow(int row) {
  assert(row == indexOfNextCell());
  return &m_next;
}

KDCoordinate CategoricalController::nonMemoizedRowHeight(int row) {
  if (row == indexOfTableCell()) {
    return std::min(tableCellFullHeight() - tableCellVerticalOffset(),
                    static_cast<int>(m_selectableListView.bounds().height()));
  }
  // The rest of the cells are explicit
  return explicitCellAtRow(row)->minimalSizeForOptimalDisplay().height();
}

void CategoricalController::initView() {
  createDynamicCells();
  SelectableListViewController::initView();
}

void CategoricalController::initWidth(TableView* tableView) {
  ListViewDataSource::initWidth(tableView);
  int n = numberOfRows();
  for (int row = 0; row < n; row++) {
    if (row != indexOfTableCell()) {
      explicitCellAtRow(row)->initSize(m_width);
    }
  }
}

InputCategoricalController::InputCategoricalController(
    StackViewController* parent, ViewController* nextController,
    Statistic* statistic)
    : CategoricalController(
          parent, nextController,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_statistic(statistic),
      m_significanceCell(&m_selectableListView, this) {}

bool InputCategoricalController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
         event == Ion::Events::Up || event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  // Parse and check significance level
  double p = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(p)) {
    return false;
  }
  int i = indexOfEditedParameterAtIndex(m_selectableListView.selectedRow());
  if (!m_statistic->authorizedParameterAtIndex(p, i)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtIndex(p, i);
  /* Alpha and DegreeOfFreedom cannot be negative. However, DegreeOfFreedom
   * can be computed to a negative when there are no rows.
   * In that case, the degreeOfFreedom cell should display nothing. */
  PrintValueInTextHolder(p, textField, true, true);
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    event = Ion::Events::Down;
  }
  m_selectableListView.handleEvent(event);
  return true;
}

bool InputCategoricalController::ButtonAction(
    InputCategoricalController* controller, void* s) {
  if (!controller->m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return false;
  }
  controller->m_statistic->compute();
  return CategoricalController::ButtonAction(controller, s);
}

bool InputCategoricalController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

void InputCategoricalController::viewWillAppear() {
  // Significance cell
  PrintValueInTextHolder(m_statistic->threshold(),
                         m_significanceCell.textField(), true, true);
  m_significanceCell.setMessages(m_statistic->thresholdName(),
                                 m_statistic->thresholdDescription());

  categoricalTableCell()->recomputeDimensionsAndReload(true, true);
}

HighlightCell* InputCategoricalController::explicitCellAtRow(int row) {
  if (row == indexOfSignificanceCell()) {
    return &m_significanceCell;
  }
  return CategoricalController::explicitCellAtRow(row);
}

}  // namespace Inference
