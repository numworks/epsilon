#include "float_parameter_controller.h"
#include "app.h"
#include "../constant.h"
#include "../apps_container.h"
#include <assert.h>

namespace Graph {

FloatParameterController::FloatParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
{
}

View * FloatParameterController::view() {
  return &m_selectableTableView;
}

void FloatParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int FloatParameterController::activeCell() {
  return m_selectableTableView.selectedRow();
}

void FloatParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  TextMenuListCell * myCell = (TextMenuListCell *) cell;
  char buffer[Constant::FloatBufferSizeInScientificMode];
  Float(parameterAtIndex(index)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  myCell->setAccessoryText(buffer);
}

bool FloatParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    editParameter();
    return true;
  }
  if (event.hasText()) {
    editParameter(event.text());
    return true;
  }
  return false;
}

void FloatParameterController::editParameter(const char * initialText) {
  /* This code assumes that the active cell remains the one which is edited
   * until the invocation is performed. This could lead to concurrency issue in
   * other cases. */
  char initialTextContent[255];
  int cursorDelta = 0;
  if (initialText) {
    strlcpy(initialTextContent, initialText, sizeof(initialTextContent));
    cursorDelta = strlen(initialText) > 1 ? -1 : 0;
  } else {
    TextMenuListCell * textMenuListCell = (TextMenuListCell *)reusableCell(activeCell());
    strlcpy(initialTextContent, textMenuListCell->accessoryText(), sizeof(initialTextContent));
  }
  int cursorLocation = strlen(initialTextContent) + cursorDelta;
  EditableTextMenuListCell * cell = (EditableTextMenuListCell *)m_selectableTableView.cellAtLocation(0, activeCell());
  cell->setParentResponder(&m_selectableTableView);
  App * myApp = (App *)app();
  cell->editValue(myApp, initialTextContent, cursorLocation, this,
    [](void * context, void * sender){
    FloatParameterController * floatParameterController = (FloatParameterController *)context;
    int activeCell = floatParameterController->activeCell();
    EditableTextMenuListCell * cell = (EditableTextMenuListCell *)sender;
    const char * textBody = cell->editedText();
    AppsContainer * appsContainer = (AppsContainer *)floatParameterController->app()->container();
    Context * globalContext = appsContainer->context();
    float floatBody = Expression::parse(textBody)->approximate(*globalContext);
    floatParameterController->setParameterAtIndex(activeCell, floatBody);
    floatParameterController->willDisplayCellForIndex(cell, activeCell);
    });
}

KDCoordinate FloatParameterController::cellHeight() {
  return 35;
}

}
