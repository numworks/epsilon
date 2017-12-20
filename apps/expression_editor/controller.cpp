#include "controller.h"
#include <apps/expression_editor/app.h>

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
  return Responder::handleEvent(event);
}

Toolbox * Controller::toolbox() {
  ExpressionEditor::App * expressionEditorApp = static_cast<ExpressionEditor::App *>(app());
  return expressionEditorApp->mathToolbox();
}

void Controller::insertLayoutAtCursor(ExpressionLayout * layout, ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  m_cursor.addLayout(layout);
  m_cursor.setPointedExpressionLayout(pointedLayout);
  m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
  m_cursor.setPositionInside(0);
  m_view.cursorPositionChanged();
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
  if (event == Ion::Events::ShiftLeft) {
    // The cursor should never point to the main HorizontalLayout.
    m_cursor.setPointedExpressionLayout(m_expressionLayout);
    m_cursor.setPosition(ExpressionLayoutCursor::Position::Left);
    m_cursor.setPositionInside(0);
    return true;
  }
  if (event == Ion::Events::ShiftRight) {
    m_cursor.setPointedExpressionLayout(m_expressionLayout);
    m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
    m_cursor.setPositionInside(0);
    return true;
  }
  return false;
}

ExpressionLayout * Controller::handleAddEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Division) {
    return m_cursor.addEmptyFractionLayout();
  }
  if (event == Ion::Events::XNT) {
    return m_cursor.addXNTCharLayout();
  }
  if (event == Ion::Events::Exp) {
    return m_cursor.addEmptyExponentialLayout();
  }
  if (event == Ion::Events::Log) {
    return m_cursor.addEmptyLogarithmLayout();
  }
  if (event == Ion::Events::Power) {
    return m_cursor.addEmptyPowerLayout();
  }
  if (event == Ion::Events::Sqrt) {
    return m_cursor.addEmptyRootLayout();
  }
  if (event == Ion::Events::Square) {
    return m_cursor.addEmptySquarePowerLayout();
  }
  if (event.hasText()) {
    return m_cursor.insertText(event.text());
  }
  return nullptr;
}

}
