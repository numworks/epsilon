#include "list_controller.h"
#include "../edit_expression_controller.h"

using namespace Poincare;

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

ListController::ListController(Responder * parentResponder, EditExpressionController * editExpressionController, SelectableTableViewDelegate * delegate) :
  StackViewController(parentResponder, &m_listController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_listController(this, delegate),
  m_editExpressionController(editExpressionController)
{
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    char buffer[Constant::MaxSerializedExpressionSize];
    textAtIndex(buffer, Constant::MaxSerializedExpressionSize, selectedRow());
    /* The order is important here: we dismiss the pop-up first because it
     * clears the Poincare pool from the layouts used to display the pop-up.
     * Thereby it frees memory to do Poincare computations required by
     * insertTextBody. */
    Container::activeApp()->dismissModalViewController();
    m_editExpressionController->insertTextBody(buffer);
    Container::activeApp()->setFirstResponder(m_editExpressionController);
    return true;
  }
  return false;
}

void ListController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

}
