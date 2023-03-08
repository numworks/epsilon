#include "list_controller.h"

#include "../edit_expression_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

/* Inner List Controller */

ListController::InnerListController::InnerListController(
    ListController* dataSource, SelectableListViewDelegate* delegate)
    : ViewController(dataSource),
      m_selectableListView(this, dataSource, dataSource, delegate) {
  m_selectableListView.setMargins(0);
  m_selectableListView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void ListController::InnerListController::didBecomeFirstResponder() {
  m_selectableListView.reloadData();
}

/* List Controller */

ListController::ListController(
    EditExpressionController* editExpressionController,
    SelectableListViewDelegate* delegate)
    : StackViewController(nullptr, &m_listController,
                          StackViewController::Style::PurpleWhite),
      m_listController(this, delegate),
      m_editExpressionController(editExpressionController) {}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() >= 0);
    char buffer[Constant::MaxSerializedExpressionSize];
    HighlightCell* cell =
        m_listController.selectableListView()->cell(selectedRow());
    textAtIndex(buffer, Constant::MaxSerializedExpressionSize, cell,
                selectedRow());
    /* The order is important here: we dismiss the pop-up first because it
     * clears the Poincare pool from the layouts used to display the pop-up.
     * Thereby it frees memory to do Poincare computations required by
     * insertTextBody. */
    Container::activeApp()->modalViewController()->dismissModal();
    m_editExpressionController->insertTextBody(buffer);
    return true;
  }
  return false;
}

void ListController::didBecomeFirstResponder() {
  resetMemoization();
  Container::activeApp()->setFirstResponder(&m_listController);
  // Additional outputs should have at least one row to display
  assert(numberOfRows() > 0);
}

}  // namespace Calculation
