#include "interval_controller.h"
#include "app.h"
#include "../i18n.h"
#include <assert.h>
#include <string.h>

namespace Solver {

IntervalController::ContentView::ContentView(SelectableTableView * selectableTableView) :
  m_instructions0(KDFont::SmallFont, I18n::Message::ApproximateSolutionIntervalInstruction0, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_instructions1(KDFont::SmallFont, I18n::Message::ApproximateSolutionIntervalInstruction1, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_selectableTableView(selectableTableView)
{
}

void IntervalController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_topMargin), Palette::WallScreen);
}

int IntervalController::ContentView::numberOfSubviews() const {
  return 3;
}

View * IntervalController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 5);
  if (index == 0) {
    return &m_instructions0;
  }
  if (index == 1) {
    return &m_instructions1;
  }
  return m_selectableTableView;
}

void IntervalController::ContentView::layoutSubviews() {
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_instructions0.setFrame(KDRect(0, k_topMargin/2-textHeight, bounds().width(), textHeight));
  m_instructions1.setFrame(KDRect(0, k_topMargin/2, bounds().width(), textHeight));
  m_selectableTableView->setFrame(KDRect(0, k_topMargin, bounds().width(),  bounds().height()-k_topMargin));
}

/* IntervalController Controller */

IntervalController::IntervalController(Responder * parentResponder, EquationStore * equationStore) :
  FloatParameterController(parentResponder),
  m_contentView(&m_selectableTableView),
  m_intervalCell{},
  m_equationStore(equationStore)
{
  m_selectableTableView.setTopMargin(0);
  m_okButton.setMessage(I18n::Message::ResolveEquation);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_intervalCell[i].setParentResponder(&m_selectableTableView);
    m_intervalCell[i].textField()->setDelegate(this);
    m_intervalCell[i].textField()->setDraftTextBuffer(m_draftTextBuffer);
  }
}

const char * IntervalController::title() {
  return I18n::translate(I18n::Message::SearchInverval);
}

int IntervalController::numberOfRows() {
  return k_maxNumberOfCells+1;
}

void IntervalController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  I18n::Message labels[k_maxNumberOfCells] = {I18n::Message::XMin, I18n::Message::XMax};
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

HighlightCell * IntervalController::reusableParameterCell(int index, int type) {
  assert(index >= 0);
  assert(index < 2);
  return &m_intervalCell[index];
}

int IntervalController::reusableParameterCellCount(int type) {
  return k_maxNumberOfCells;
}

double IntervalController::parameterAtIndex(int index) {
  return m_equationStore->intervalBound(index);
}

bool IntervalController::setParameterAtIndex(int parameterIndex, double f) {
  m_equationStore->setIntervalBound(parameterIndex, f);
  return true;
}

bool IntervalController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, text, event)) {
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

void IntervalController::buttonAction() {
  StackViewController * stack = stackController();
  App * solverApp = static_cast<App *>(app());
  m_equationStore->approximateSolve(solverApp->localContext());
  stack->push(solverApp->solutionsControllerStack(), KDColorWhite, Palette::SubTab, Palette::SubTab);
}

}
