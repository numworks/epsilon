#include "controller.h"

using namespace Poincare;

namespace ExpressionEditor {

Controller::Controller(Responder * parentResponder, ExpressionLayout * expressionLayout) :
  ViewController(parentResponder),
  m_view(parentResponder, expressionLayout, &m_cursor),
  m_expressionLayout(expressionLayout)
{
  m_cursor.setPointedExpressionLayout(expressionLayout->editableChild(0));
}

void Controller::didBecomeFirstResponder() {
  m_view.layoutSubviews();
  /* TODO We need the layout to be done in order to scroll to the cursor. We
   * thus made ExpressionViewWithCursor's layoutSubviews() public, which is the
   * solution used in ModalViewController for instance. A cleaner solution would
   * be to split viewWillAppear into loadViewIfNeeded() and viewWillAppear(), in
   * order to change App::didBecomeActive to:
   *    m_modalViewController.loadViewIfNeeded();
   *    window->setContentView(view);
   *    m_modalViewController.viewWillAppear();
   * The scrolling could then be done in viewWillAppear(), without calling
   * layoutSubviews() manually. */
  m_view.scrollableExpressionViewWithCursor()->scrollToCursor();
}

bool Controller::handleEvent(Ion::Events::Event event) {
  if (privateHandleEvent(event)) {
    m_view.cursorPositionChanged();
    return true;
  }
  return false;
}

bool Controller::privateHandleEvent(Ion::Events::Event event) {
  if (handleMoveEvent(event)) {
    return true;
  }
  ExpressionLayout * newPointedLayout = handleAddEvent(event);
  if (newPointedLayout != nullptr) {
    m_cursor.setPointedExpressionLayout(newPointedLayout);
    m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
    m_cursor.setPositionInside(0);
    m_expressionLayout->invalidAllSizesPositionsAndBaselines();
    m_view.layoutSubviews();
    return true;
  }
  return false;
}

bool Controller::handleMoveEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    return m_cursor.moveLeft();
  }
  if (event == Ion::Events::Right) {
    return m_cursor.moveRight();
  }
  if (event == Ion::Events::Up) {
    return m_cursor.moveUp();
  }
  if (event == Ion::Events::Down) {
    return m_cursor.moveDown();
  }
  return false;
}

ExpressionLayout * Controller::handleAddEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Division) {
    return m_cursor.addEmptyFractionLayout();
  }
  if (event.hasText()) {
    return m_cursor.insertText(event.text());
  }
  return nullptr;
}

}
