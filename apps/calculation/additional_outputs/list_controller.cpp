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
    m_editExpressionController->insertTextBody("TODO");
    Container::activeApp()->dismissModalViewController();
    Container::activeApp()->setFirstResponder(m_editExpressionController);
    return true;
  }
  return false;
}

void ListController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

}
