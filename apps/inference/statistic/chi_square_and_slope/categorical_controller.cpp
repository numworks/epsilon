#include "categorical_controller.h"

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/text_helpers.h"
#include <escher/invocation.h>

using namespace Escher;

namespace Inference {

CategoricalController::CategoricalController(
    Responder * parent,
    ViewController * nextController,
    Invocation invocation) :
      SelectableListViewController<ListViewDataSource>(parent),
      m_nextController(nextController),
      m_next(&m_selectableTableView, I18n::Message::Next, invocation, Palette::WallScreenDark, Metric::CommonMargin)
{
  m_selectableTableView.setTopMargin(0);
  m_selectableTableView.setLeftMargin(0);
  m_selectableTableView.setRightMargin(0);
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
  setScrollViewDelegate(this);
}

void CategoricalController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectRow(0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool CategoricalController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

bool CategoricalController::ButtonAction(void * c, void * s) {
  CategoricalController * controller = static_cast<CategoricalController *>(c);
  controller->stackOpenPage(controller->m_nextController);
  return true;
}

void CategoricalController::scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) {
  /* Transfer the CategoricalController offset to the CategoricalTableCell
   * offset. This is a hack to ensure that the categorical table cell doesn't
   * require too many displayable cells. If the scroll was handled by the
   * CategoricalController, the CategoricalTableCell would need as many
   * displayable cells as its real number of cells. Since the CategoricalController
   * need at most 3 cells, we delegate the scroll handling to the
   * CategoricalTableCell. */
  categoricalTableCell()->selectableTableView()->setContentOffset(KDPoint(0, categoricalTableCell()->selectableTableView()->contentOffset().y() + scrollViewDataSource->offset().y()));
  // Unset the ScrollViewDelegate to avoid infinite looping
  setScrollViewDelegate(nullptr);
  m_selectableTableView.setContentOffset(KDPointZero);
  setScrollViewDelegate(this);
}


void CategoricalController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  int row = t->selectedRow();
  int col = t->selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != t->selectedRow()) {
    // We need to clip the size of the CategoricalTableCell to force it to scroll
    categoricalTableCell()->selectableTableView()->setSize(m_selectableTableView.bounds().size());
    categoricalTableCell()->selectableTableView()->scrollToCell(col, row);
    m_selectableTableView.reloadData(false, false);
  }
}

HighlightCell * CategoricalController::reusableCell(int index, int type) {
  if (type == k_indexOfTableCell) {
    return categoricalTableCell();
  } else {
    assert(type == indexOfNextCell());
    return &m_next;
  }
}

KDCoordinate CategoricalController::rowHeight(int index) {
  if (index == k_indexOfTableCell) {
    return std::min(categoricalTableCell()->minimalSizeForOptimalDisplay().height() - categoricalTableCell()->selectableTableView()->contentOffset().y(), static_cast<int>(m_selectableTableView.bounds().height()));
  }
  return ListViewDataSource::rowHeight(index);
}

InputCategoricalController::InputCategoricalController(
    StackViewController * parent,
    ViewController * nextController,
    Statistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
      CategoricalController(parent, nextController, Invocation(&InputCategoricalController::ButtonAction, this)),
      m_statistic(statistic),
      m_innerSignificanceCell(&m_selectableTableView, inputEventHandlerDelegate, this),
      m_significanceCell(&m_innerSignificanceCell)
{
  m_innerSignificanceCell.setMessage(I18n::Message::GreekAlpha);
  m_innerSignificanceCell.setSubLabelMessage(I18n::Message::SignificanceLevel);
}

bool InputCategoricalController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Up || event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  // Parse and check significance level
  double p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
  return handleEditedValue(indexOfEditedParameterAtIndex(m_selectableTableView.selectedRow()), p, textField, event);
}

void InputCategoricalController::didEnterResponderChain(Responder * previousResponder) {
  categoricalTableCell()->recomputeDimensions();
  PrintValueInTextHolder(m_statistic->threshold(), m_innerSignificanceCell.textField(), true, true);
}

bool InputCategoricalController::ButtonAction(void * c, void * s) {
  InputCategoricalController * controller = static_cast<InputCategoricalController *>(c);
  if (!controller->m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return false;
  }
  controller->m_statistic->compute();
  return CategoricalController::ButtonAction(c, s);
}

void InputCategoricalController::tableViewDataSourceDidChangeSize() {
  /* Relayout when inner table changes size. We need to reload the table because
   * its width might change but it won't relayout as its frame isn't changed by
   * the InputCategoricalController */
  categoricalTableCell()->selectableTableView()->reloadData(false);
  m_selectableTableView.reloadData(false, false);
}

HighlightCell * InputCategoricalController::reusableCell(int index, int type) {
  if (type == indexOfSignificanceCell()) {
    return &m_significanceCell;
  } else {
    return CategoricalController::reusableCell(index, type);
  }
}

bool InputCategoricalController::handleEditedValue(int i, double p, TextField * textField, Ion::Events::Event event) {
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
  m_selectableTableView.handleEvent(event);
  return true;
}

}  // namespace Inference
