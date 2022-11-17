#include "list_controller.h"
#include "../edit_expression_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

/* Inner List Controller */

ListController::InnerListController::InnerListController(ListController * dataSource, SelectableTableViewDelegate * delegate) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource, delegate)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void ListController::InnerListController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

/* List Controller */

ListController::ListController(EditExpressionController * editExpressionController, SelectableTableViewDelegate * delegate) :
  StackViewController(nullptr, &m_listController, StackViewController::Style::PurpleWhite),
  m_listController(this, delegate),
  m_editExpressionController(editExpressionController)
{
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() >= 0);
    char buffer[Constant::MaxSerializedExpressionSize];
    textAtIndex(buffer, Constant::MaxSerializedExpressionSize, selectedRow());
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

}
