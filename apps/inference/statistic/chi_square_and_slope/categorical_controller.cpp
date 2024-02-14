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
    : SelectableListViewController<ListViewDataSource>(parent, this),
      m_nextController(nextController),
      m_next(&m_selectableListView, I18n::Message::Next, invocation,
             Palette::WallScreenDark, Metric::CommonMargins.left()) {
  m_selectableListView.margins()->setTop(0);
  m_selectableListView.margins()->setHorizontal({0, 0});
  m_selectableListView.setBackgroundColor(Palette::WallScreenDark);
  setScrollViewDelegate(this);
}

bool CategoricalController::ButtonAction(CategoricalController *controller,
                                         void *s) {
  controller->stackOpenPage(controller->m_nextController);
  return true;
}

void CategoricalController::scrollViewDidChangeOffset(
    ScrollViewDataSource *scrollViewDataSource) {
  assert(scrollViewDataSource == this);
  /* Transfer the CategoricalController offset to the CategoricalTableCell
   * offset. This is a hack to ensure that the categorical table cell doesn't
   * require too many displayable cells. If the scroll was handled by the
   * CategoricalController, the CategoricalTableCell would need as many
   * displayable cells as its real number of cells. Since the
   * CategoricalController needs at most 3 cells, we delegate the scroll
   * handling to the CategoricalTableCell. */
  KDPoint listOffset = m_selectableListView.contentOffset();
  KDPoint tableOffset =
      categoricalTableCell()->selectableTableView()->contentOffset();

  KDCoordinate maximalListOffsetY =
      cumulatedHeightBeforeRow(indexOfTableCell()) +
      m_selectableListView.margins()->top();
  KDCoordinate maximalTableOffsetY =
      std::max(0, m_selectableListView.minimalSizeForOptimalDisplay().height() +
                      tableOffset.y() - m_selectableListView.bounds().height() -
                      maximalListOffsetY);

  KDCoordinate newListOffsetY =
      std::clamp<int>(listOffset.y() + tableOffset.y(), 0, maximalListOffsetY);
  KDCoordinate newTableOffsetY =
      std::clamp<int>(listOffset.y() + tableOffset.y() - newListOffsetY, 0,
                      maximalTableOffsetY);

  categoricalTableCell()->selectableTableView()->setContentOffset(
      KDPoint(tableOffset.x(), newTableOffsetY));
  // Unset the ScrollViewDelegate to avoid infinite looping
  setScrollViewDelegate(nullptr);
  m_selectableListView.setContentOffset(
      KDPoint(listOffset.x(), newListOffsetY));
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
      categoricalTableCell()->selectableTableView()->contentOffset().y() +
          m_selectableListView.contentOffset().y(),
      m_selectableListView.bounds().height());
  return true;
}

void CategoricalController::listViewDidChangeSelectionAndDidScroll(
    SelectableListView *l, int previousRow, KDPoint previousOffset,
    bool withinTemporarySelection) {
  assert(l == &m_selectableListView);
  if (previousRow == l->selectedRow() || withinTemporarySelection) {
    return;
  }
  if (previousRow == indexOfTableCell()) {
    categoricalTableCell()->selectRow(-1);
    categoricalTableCell()->layoutSubviews(true);
  } else if (l->selectedRow() == indexOfTableCell() &&
             previousRow > l->selectedRow()) {
    categoricalTableCell()->selectRow(
        categoricalTableCell()->tableViewDataSource()->numberOfRows() - 1);
  }
}

HighlightCell *CategoricalController::reusableCell(int index, int type) {
  if (type == indexOfTableCell()) {
    return categoricalTableCell();
  }
  assert(type == indexOfNextCell());
  return &m_next;
}

KDCoordinate CategoricalController::nonMemoizedRowHeight(int row) {
  if (row == indexOfTableCell()) {
    return std::min(
        categoricalTableCell()->minimalSizeForOptimalDisplay().height() -
            categoricalTableCell()->selectableTableView()->contentOffset().y(),
        static_cast<int>(m_selectableListView.bounds().height()));
  }
  assert(row == indexOfNextCell());
  return m_next.minimalSizeForOptimalDisplay().height();
}

void CategoricalController::initView() {
  createDynamicCells();
  SelectableListViewController::initView();
}

InputCategoricalController::InputCategoricalController(
    StackViewController *parent, ViewController *nextController,
    Statistic *statistic)
    : CategoricalController(
          parent, nextController,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_statistic(statistic),
      m_significanceCell(&m_selectableListView, this) {}

bool InputCategoricalController::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
         event == Ion::Events::Up || event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
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
    InputCategoricalController *controller, void *s) {
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
  // Table cell
  categoricalTableCell()->selectableTableView()->resetScroll();
  categoricalTableCell()->recomputeDimensionsAndReload(true, true);
}

HighlightCell *InputCategoricalController::reusableCell(int index, int type) {
  if (type == indexOfSignificanceCell()) {
    return &m_significanceCell;
  }
  return CategoricalController::reusableCell(index, type);
}

KDCoordinate InputCategoricalController::nonMemoizedRowHeight(int row) {
  if (row == indexOfSignificanceCell()) {
    return m_significanceCell.minimalSizeForOptimalDisplay().height();
  }
  return CategoricalController::nonMemoizedRowHeight(row);
}

InputNamedListsCategoricalController::InputNamedListsCategoricalController(
    StackViewController *parent, ViewController *resultsController,
    Statistic *statistic)
    : InputCategoricalController(parent, resultsController, statistic),
      m_dropdownCell(&m_selectableListView, &m_dropdownDataSource, this) {
  m_dropdownCell.setMessage(I18n::Message::Default);  // TODO
}

}  // namespace Inference
