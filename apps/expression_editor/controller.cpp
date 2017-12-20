#include "controller.h"
#include <poincare/src/layout/char_layout.h> //TODO move from there.
#include <poincare/src/layout/fraction_layout.h> //TODO move from there.
#include <poincare/src/layout/empty_visible_layout.h> //TODO move from there.

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
  bool returnValue = false;
  if ((event == Ion::Events::Left && m_cursor.moveLeft())
    || (event == Ion::Events::Right && m_cursor.moveRight())
    || (event == Ion::Events::Up && m_cursor.moveUp())
    || (event == Ion::Events::Down && m_cursor.moveDown()))
  {
    returnValue = true;
  } else if (event == Ion::Events::Division) {
    EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
    EmptyVisibleLayout * child2 = new EmptyVisibleLayout();

    FractionLayout * newChild = new FractionLayout(child1, child2, false);
    m_cursor.pointedExpressionLayout()->addBrother(&m_cursor, newChild);
    m_cursor.setPointedExpressionLayout(newChild);
    m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
    m_cursor.setPositionInside(0);
    returnValue = true;
    m_expressionLayout->invalidAllSizesPositionsAndBaselines();
    m_view.layoutSubviews();
  } else if (event.hasText()) {
    insertTextAtCursor(event.text());
    returnValue = true;
    m_expressionLayout->invalidAllSizesPositionsAndBaselines();
    m_view.layoutSubviews();
  }
  m_view.cursorPositionChanged();
  return returnValue;
}

void Controller::insertTextAtCursor(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  CharLayout * newChild = nullptr;
  for (int i = 0; i < textLength; i++) {
    newChild = new CharLayout(text[i]);
    m_cursor.pointedExpressionLayout()->addBrother(&m_cursor, newChild);
  }
  assert(newChild != nullptr);
  m_cursor.setPointedExpressionLayout(newChild);
  m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
  m_cursor.setPositionInside(0);
}

}
