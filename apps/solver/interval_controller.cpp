#include "interval_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <string.h>

#include "app.h"

using namespace Escher;

namespace Solver {

IntervalController::ContentView::ContentView(
    SelectableTableView *selectableTableView)
    : m_instructions0(I18n::Message::ApproximateSolutionIntervalInstruction0,
                      k_glyphFormat),
      m_instructions1(I18n::Message::ApproximateSolutionIntervalInstruction1,
                      k_glyphFormat),
      m_selectableTableView(selectableTableView) {}

void IntervalController::ContentView::drawRect(KDContext *ctx,
                                               KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_topMargin),
                Palette::WallScreen);
}

int IntervalController::ContentView::numberOfSubviews() const { return 3; }

View *IntervalController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 5);
  if (index == 0) {
    return &m_instructions0;
  }
  if (index == 1) {
    return &m_instructions1;
  }
  return m_selectableTableView;
}

void IntervalController::ContentView::layoutSubviews(bool force) {
  KDCoordinate textHeight = KDFont::GlyphHeight(KDFont::Size::Small);
  setChildFrame(
      &m_instructions0,
      KDRect(0, k_topMargin / 2 - textHeight, bounds().width(), textHeight),
      force);
  setChildFrame(&m_instructions1,
                KDRect(0, k_topMargin / 2, bounds().width(), textHeight),
                force);
  setChildFrame(
      m_selectableTableView,
      KDRect(0, k_topMargin, bounds().width(), bounds().height() - k_topMargin),
      force);
}

/* IntervalController Controller */

IntervalController::IntervalController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate)
    : FloatParameterController<double>(parentResponder),
      m_contentView(&m_selectableListView),
      m_shouldReplaceFunctionsButNotSymbols(false) {
  m_selectableListView.setTopMargin(0);
  m_okButton.setMessage(I18n::Message::ResolveEquation);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_intervalCell[i].setParentResponder(&m_selectableListView);
    m_intervalCell[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

const char *IntervalController::title() {
  return I18n::translate(I18n::Message::SearchInverval);
}

int IntervalController::numberOfRows() const { return k_maxNumberOfCells + 1; }

void IntervalController::willDisplayCellForIndex(HighlightCell *cell,
                                                 int index) {
  if (index == numberOfRows() - 1) {
    return;
  }
  I18n::Message labels[k_maxNumberOfCells] = {I18n::Message::XMin,
                                              I18n::Message::XMax};
  MenuCellWithEditableText<MessageTextView> *myCell =
      static_cast<MenuCellWithEditableText<MessageTextView> *>(cell);
  myCell->label()->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

HighlightCell *IntervalController::reusableParameterCell(int index, int type) {
  assert(index >= 0);
  assert(index < 2);
  return &m_intervalCell[index];
}

TextField *IntervalController::textFieldOfCellAtIndex(HighlightCell *cell,
                                                      int index) {
  assert(typeAtIndex(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView> *>(cell)
      ->textField();
}

int IntervalController::reusableParameterCellCount(int type) {
  return k_maxNumberOfCells;
}

double IntervalController::parameterAtIndex(int index) {
  SystemOfEquations *system = App::app()->system();
  return index == 0 ? system->approximateResolutionMinimum()
                    : system->approximateResolutionMaximum();
}

bool IntervalController::setParameterAtIndex(int parameterIndex, double f) {
  SystemOfEquations *system = App::app()->system();
  parameterIndex == 0 ? system->setApproximateResolutionMinimum(f)
                      : system->setApproximateResolutionMaximum(f);
  return true;
}

bool IntervalController::textFieldDidFinishEditing(AbstractTextField *textField,
                                                   const char *text,
                                                   Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, text,
                                                          event)) {
    resetMemoization();
    m_selectableListView.reloadData();
    return true;
  }
  return false;
}

void IntervalController::buttonAction() {
  StackViewController *stack = stackController();
  App::app()->system()->approximateSolve(
      textFieldDelegateApp()->localContext());
  stack->push(App::app()->solutionsControllerStack());
}

}  // namespace Solver
