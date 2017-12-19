#include "controller.h"

namespace ExpressionEditor {

Controller::Controller(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout) :
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
  }
  else if (event.hasText() && m_expressionLayout->insertLayoutForTextAtCursor(event.text(), &m_cursor)) {
    returnValue = true;
    m_expressionLayout->invalidAllSizesAndPositions();
    m_view.layoutSubviews();
  }
  m_view.cursorPositionChanged();
  return returnValue;
}

}
