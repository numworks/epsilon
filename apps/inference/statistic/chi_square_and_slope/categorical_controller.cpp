#include "categorical_controller.h"

#include <escher/invocation.h>

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

CategoricalController::CategoricalController(Responder *parent,
                                             ViewController *nextController,
                                             Invocation invocation)
    : SelectableListViewController<ListViewDataSource>(parent),
      m_nextController(nextController),
      m_next(&m_selectableListView, I18n::Message::Next, invocation,
             Palette::WallScreenDark, Metric::CommonMargin) {
  m_selectableListView.setTopMargin(0);
  m_selectableListView.setLeftMargin(0);
  m_selectableListView.setRightMargin(0);
  m_selectableListView.setBackgroundColor(Palette::WallScreenDark);
  setScrollViewDelegate(this);
}

void CategoricalController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    categoricalTableCell()->selectableTableView()->setContentOffset(
        KDPointZero);
  }
  SelectableListViewController<ListViewDataSource>::didBecomeFirstResponder();
}

bool CategoricalController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

bool CategoricalController::ButtonAction(CategoricalController *controller,
                                         void *s) {
  controller->stackOpenPage(controller->m_nextController);
  return true;
}

void CategoricalController::scrollViewDidChangeOffset(
    ScrollViewDataSource *scrollViewDataSource) {
  /* Transfer the CategoricalController offset to the CategoricalTableCell
   * offset. This is a hack to ensure that the categorical table cell doesn't
   * require too many displayable cells. If the scroll was handled by the
   * CategoricalController, the CategoricalTableCell would need as many
   * displayable cells as its real number of cells. Since the
   * CategoricalController needs at most 3 cells, we delegate the scroll
   * handling to the CategoricalTableCell. */
  KDPoint currentOffset =
      categoricalTableCell()->selectableTableView()->contentOffset();
  KDCoordinate maximalOffsetY =
      m_selectableListView.minimalSizeForOptimalDisplay().height() -
      m_selectableListView.bounds().height();
  KDCoordinate offsetToAdd = scrollViewDataSource->offset().y();
  if (offsetToAdd > maximalOffsetY) {
    /* Prevent the table from scrolling past the screen */
    offsetToAdd = maximalOffsetY;
  }
  /* New offset should be corrected to account for the truncation of the
   * categorical cell. */
  KDCoordinate displayedCategoricalCellHeight =
      nonMemoizedRowHeight(k_indexOfTableCell);
  KDCoordinate trueCategoricalCellHeight =
      categoricalTableCell()->minimalSizeForOptimalDisplay().height() -
      currentOffset.y();
  KDCoordinate newOffsetY = offsetToAdd + currentOffset.y() +
                            trueCategoricalCellHeight -
                            displayedCategoricalCellHeight;
  categoricalTableCell()->selectableTableView()->setContentOffset(
      KDPoint(currentOffset.x(), newOffsetY));
  // Unset the ScrollViewDelegate to avoid infinite looping
  setScrollViewDelegate(nullptr);
  m_selectableListView.setContentOffset(KDPointZero);
  setScrollViewDelegate(this);
}

bool CategoricalController::updateBarIndicator(bool vertical, bool *visible) {
  assert(visible);
  if (!vertical) {
    return false;
  }

  ScrollView::BarDecorator *decorator =
      static_cast<ScrollView::BarDecorator *>(m_selectableListView.decorator());
  KDCoordinate otherCellsHeight =
      m_selectableListView.minimalSizeForOptimalDisplay().height() -
      categoricalTableCell()->bounds().height();
  KDCoordinate trueOptimalHeight =
      categoricalTableCell()->minimalSizeForOptimalDisplay().height() +
      otherCellsHeight;
  *visible = decorator->verticalBar()->update(
      trueOptimalHeight,
      categoricalTableCell()->selectableTableView()->contentOffset().y(),
      m_selectableListView.bounds().height());
  return true;
}

void CategoricalController::tableViewDidChangeSelection(
    SelectableTableView *t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  int row = t->selectedRow();
  int col = t->selectedColumn();
  if (!withinTemporarySelection && previousSelectedRow != t->selectedRow()) {
    KDCoordinate verticalOffset =
        categoricalTableCell()->selectableTableView()->contentOffset().y();
    KDCoordinate tableCellRequiredHeight = categoricalTableCell()
                                               ->selectableTableView()
                                               ->minimalSizeForOptimalDisplay()
                                               .height();
    KDCoordinate displayedHeight = m_selectableListView.bounds().height();
    KDCoordinate givenHeight;
    if (verticalOffset + displayedHeight < tableCellRequiredHeight) {
      // We need to clip the size of the CategoricalTableCell to force it to
      // scroll
      givenHeight = displayedHeight;
    } else {
      // We need to enlarge the size of the CategoricalTableCell to authorize it
      // to scroll downer than its own height
      givenHeight = tableCellRequiredHeight - verticalOffset;
    }
    categoricalTableCell()->selectableTableView()->setSize(
        KDSize(m_selectableListView.bounds().width(), givenHeight));
    categoricalTableCell()->selectableTableView()->scrollToCell(col, row);
    if (categoricalTableCell()->selectableTableView()->contentOffset().y() !=
        verticalOffset) {
      // Relayout the whole Categorical table if the scroll change
      m_selectableListView.reloadData(false);
    }
  }
}

HighlightCell *CategoricalController::reusableCell(int index, int type) {
  if (type == k_indexOfTableCell) {
    return categoricalTableCell();
  } else {
    assert(type == indexOfNextCell());
    return &m_next;
  }
}

KDCoordinate CategoricalController::nonMemoizedRowHeight(int index) {
  if (index == k_indexOfTableCell) {
    return std::min(
        categoricalTableCell()->minimalSizeForOptimalDisplay().height() -
            categoricalTableCell()->selectableTableView()->contentOffset().y(),
        static_cast<int>(m_selectableListView.bounds().height()));
  }
  return ListViewDataSource::nonMemoizedRowHeight(index);
}

InputCategoricalController::InputCategoricalController(
    StackViewController *parent, ViewController *nextController,
    Statistic *statistic, InputEventHandlerDelegate *inputEventHandlerDelegate)
    : CategoricalController(
          parent, nextController,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_statistic(statistic),
      m_significanceCell(&m_selectableListView, inputEventHandlerDelegate,
                         this) {}

bool InputCategoricalController::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE ||
         event == Ion::Events::Up || event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  // Parse and check significance level
  double p = textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (textFieldDelegateApp()->hasUndefinedValue(p, false, false)) {
    return false;
  }
  return handleEditedValue(
      indexOfEditedParameterAtIndex(m_selectableListView.selectedRow()), p,
      textField, event);
}

bool InputCategoricalController::ButtonAction(
    InputCategoricalController *controller, void *s) {
  if (!controller->m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return false;
  }
  controller->m_statistic->compute();
  return CategoricalController::ButtonAction(controller, s);
}

void InputCategoricalController::viewWillAppear() {
  categoricalTableCell()->selectableTableView()->setContentOffset(KDPointZero);
  categoricalTableCell()->recomputeDimensions();
  PrintValueInTextHolder(m_statistic->threshold(),
                         m_significanceCell.textField(), true, true);
  CategoricalController::viewWillAppear();
}

void InputCategoricalController::tableViewDataSourceDidChangeSize() {
  /* Relayout when inner table changes size. We need to reload the table because
   * its width might change but it won't relayout as its frame isn't changed by
   * the InputCategoricalController */
  categoricalTableCell()->selectableTableView()->reloadData(false);
  m_selectableListView.reloadData(false);
}

HighlightCell *InputCategoricalController::reusableCell(int index, int type) {
  if (type == indexOfSignificanceCell()) {
    return &m_significanceCell;
  } else {
    return CategoricalController::reusableCell(index, type);
  }
}

void InputCategoricalController::willDisplayCellForIndex(
    Escher::HighlightCell *cell, int index) {
  if (index == indexOfSignificanceCell()) {
    assert(cell == &m_significanceCell);
    m_significanceCell.setMessages(m_statistic->thresholdName(),
                                   m_statistic->thresholdDescription());
  }
}

bool InputCategoricalController::handleEditedValue(int i, double p,
                                                   AbstractTextField *textField,
                                                   Ion::Events::Event event) {
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

}  // namespace Inference
