#include "controller.h"
#include <apps/expression_editor/app.h>
#include <apps/apps_container.h>
#include <ion/charset.h>

using namespace Poincare;

namespace ExpressionEditor {

Controller::Controller(Responder * parentResponder, ExpressionLayout * expressionLayout) :
  ViewController(parentResponder),
  m_view(parentResponder, expressionLayout, &m_cursor),
  m_expressionLayout(expressionLayout),
  m_resultLayout(nullptr)
{
  m_cursor.setPointedExpressionLayout(expressionLayout);
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
  if (event == Ion::Events::EXE) {
    serializeLayout();
    computeResult();
    return true;
  }
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
  m_expressionLayout->invalidAllSizesPositionsAndBaselines();
  m_view.cursorPositionChanged();
}

bool Controller::privateHandleEvent(Ion::Events::Event event) {
  if (handleMoveEvent(event)) {
    m_expressionLayout->invalidAllSizesPositionsAndBaselines();
    m_view.layoutSubviews();
    return true;
  }
  if (handleAddEvent(event)) {
    m_expressionLayout->invalidAllSizesPositionsAndBaselines();
    m_view.layoutSubviews();
    return true;
  }
  if (handleDeleteEvent(event)) {
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
    return true;
  }
  if (event == Ion::Events::ShiftRight) {
    m_cursor.setPointedExpressionLayout(m_expressionLayout);
    m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  return false;
}

bool Controller::handleAddEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Division) {
    m_cursor.addFractionLayoutAndCollapseBrothers();
    return true;
  }
  if (event == Ion::Events::XNT) {
    m_cursor.addXNTCharLayout();
    return true;
  }
  if (event == Ion::Events::Exp) {
    m_cursor.addEmptyExponentialLayout();
    return true;
  }
  if (event == Ion::Events::Log) {
    m_cursor.addEmptyLogarithmLayout();
    return true;
  }
  if (event == Ion::Events::Power) {
    m_cursor.addEmptyPowerLayout();
    return true;
  }
  if (event == Ion::Events::Sqrt) {
    m_cursor.addEmptySquareRootLayout();
    return true;
  }
  if (event == Ion::Events::Square) {
    m_cursor.addEmptySquarePowerLayout();
    return true;
  }
  if (event.hasText()) {
    const char * textToInsert = event.text();
    if (textToInsert[1] == 0) {
      if (textToInsert[0] == Ion::Charset::MultiplicationSign) {
        const char middleDotString[] = {Ion::Charset::MiddleDot, 0};
        m_cursor.insertText(middleDotString);
        return true;
      }
      if (textToInsert[0] == '[' || textToInsert[0] == ']') {
        m_cursor.addEmptyMatrixLayout();
        return true;
      }
    }
    m_cursor.insertText(textToInsert);
    return true;
  }
  return false;
}

bool Controller::handleDeleteEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    m_cursor.performBackspace();
    return true;
  }
  return false;
}

void Controller::serializeLayout() {
  m_expressionLayout->writeTextInBuffer(m_buffer, k_bufferSize);
  m_view.setText(const_cast<const char *>(m_buffer));
}

void Controller::computeResult() {
  m_expressionLayout->writeTextInBuffer(m_buffer, k_bufferSize);
  Expression * result = Expression::parse(m_buffer);
  Expression * approxResult = nullptr;
  if (result != nullptr) {
    Expression::Simplify(&result,*(((AppsContainer *)(app()->container()))->globalContext()));
    approxResult = result->approximate<double>(*(((AppsContainer *)(app()->container()))->globalContext()));
    delete m_resultLayout;
  } else {
    approxResult = new Undefined();
  }
  m_resultLayout = approxResult->createLayout();
  m_view.setResult(m_resultLayout);
  if (result != nullptr) {
    delete result;
  }
  if (approxResult != nullptr) {
    delete approxResult;
  }
}

}
