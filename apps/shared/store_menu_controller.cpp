#include "store_menu_controller.h"
#include <escher/clipboard.h>
#include <escher/invocation.h>
#include "poincare_helpers.h"
#include "text_field_delegate_app.h"

using namespace Poincare;
using namespace Shared;
using namespace Ion;
using namespace Escher;

StoreMenuController::InnerListController::InnerListController(StoreMenuController * dataSource, SelectableTableViewDelegate * delegate) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource, delegate)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void StoreMenuController::InnerListController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  m_selectableTableView.reloadData();
}

StoreMenuController::StoreMenuController() :
  ModalViewController(this, &m_stackViewController),
  m_stackViewController(nullptr, &m_listController, StackViewController::Style::PurpleWhite),
  m_listController(this),
  m_cell(this, nullptr, this, this),
  m_abortController(
    Invocation([](void * context, void * sender) {
      StoreMenuController * storeMenu = static_cast<StoreMenuController*>(context);
      storeMenu->dismissModalViewController();
      Container::activeApp()->dismissModalViewController();
      return true;
    }, this),
    Invocation([](void * context, void * sender) {
      StoreMenuController * storeMenu = static_cast<StoreMenuController*>(context);
      storeMenu->dismissModalViewController();
      return true;
    }, this),
    I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel
    ),
  m_preventReload(false)
{
  m_abortController.setContentMessage(I18n::Message::InvalidInputWarning);
}

bool StoreMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto) {
    m_cell.expressionField()->handleEventWithText("→");
    return true;
  }
  return false;
}

void StoreMenuController::didBecomeFirstResponder() {
  m_preventReload = true;
  m_cell.expressionField()->setEditing(true);
  m_cell.expressionField()->setText(m_text);
  m_cell.expressionField()->handleEventWithText("→");
  if (m_text[0] == 0) {
    m_cell.expressionField()->putCursorLeftOfField();
  }
  resetMemoization();
  m_preventReload = false;
  Container::activeApp()->setFirstResponder(&m_listController);
}

void StoreMenuController::setText(const char * text) {
  m_text = text;
}

void StoreMenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  m_cell.reloadCell();
}

void StoreMenuController::layoutFieldDidChangeSize(LayoutField * layoutField) {
  if (!m_preventReload) {
    m_preventReload = true;
    m_listController.selectableTableView()->reloadData();
  }
  m_preventReload = false;
}

bool StoreMenuController::layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) {
  constexpr size_t bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];
  layoutR.serializeForParsing(buffer, bufferSize);
  Expression exp = Expression::Parse(buffer, Container::activeApp()->localContext());
  m_preventReload = true;
  if (exp.isUninitialized()) {
    // We are already in a popup, we only need the bottom margin
    displayModalViewController(&m_abortController, 0.f, 0.f, 0, 0, Escher::Metric::PopUpBottomMargin, 0);
    return false;
  }
  PoincareHelpers::ParseAndSimplify(buffer, Container::activeApp()->localContext());
  Container::activeApp()->dismissModalViewController();
  return true;
}

bool StoreMenuController::textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  Expression exp = Expression::Parse(text, Container::activeApp()->localContext());
  m_preventReload = true;
  if (exp.isUninitialized()) {
    // We are already in a popup, we only need the bottom margin
    displayModalViewController(&m_abortController, 0.f, 0.f, 0, 0, Escher::Metric::PopUpBottomMargin, 0);
    return false;
  }
  PoincareHelpers::ParseAndSimplify(text, Container::activeApp()->localContext());
  Container::activeApp()->dismissModalViewController();
  return true;
}

bool StoreMenuController::layoutFieldShouldFinishEditing(Escher::LayoutField * layoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool StoreMenuController::textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool StoreMenuController::layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) {
  /* Since dismissing the controller will call layoutFieldDidChangeSize, we need
   * to set the flag to avoid reloadData from happening which would otherwise
   * setFirstResponder on the store menu while it is hidden. */
  m_preventReload = true;
  Container::activeApp()->dismissModalViewController();
  return true;
}

bool StoreMenuController::textFieldDidAbortEditing(Escher::AbstractTextField * textField) {
  Container::activeApp()->dismissModalViewController();
  return true;
}

