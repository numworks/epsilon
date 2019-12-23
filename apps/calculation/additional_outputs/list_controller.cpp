#include "list_controller.h"

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

ListController::ListController(Responder * parentResponder, SelectableTableViewDelegate * delegate) :
  StackViewController(parentResponder, &m_listController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_listController(this, delegate)
{
}

bool ListController::handleEvent(Ion::Events::Event event) {
  return false;
}

void ListController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

}
