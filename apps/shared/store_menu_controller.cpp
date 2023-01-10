#include "store_menu_controller.h"
#include <escher/clipboard.h>
#include <escher/invocation.h>
#include <poincare/store.h>
#include "expression_display_permissions.h"
#include "poincare_helpers.h"
#include "text_field_delegate_app.h"
#include <apps/shared/expression_field_delegate_app.h>

using namespace Poincare;
using namespace Escher;

StoreMenuController::InnerListController::InnerListController(StoreMenuController * dataSource, SelectableTableViewDelegate * delegate) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource, delegate)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void StoreMenuController::open() {
  Container::activeApp()->displayModalViewController(this, KDContext::k_alignCenter, KDContext::k_alignCenter, 0, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin, true);
}

void StoreMenuController::close() {
  Container::activeApp()->modalViewController()->dismissModal();
}

void StoreMenuController::InnerListController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  m_selectableTableView.reloadData();
}

StoreMenuController::StoreMenuController() :
  ModalViewController(this, &m_stackViewController),
  m_stackViewController(nullptr, &m_listController, StackViewController::Style::PurpleWhite, false),
  m_listController(this),
  m_cell(this, nullptr, this, this),
  m_abortController(
    Invocation::Builder<StoreMenuController>([](StoreMenuController * storeMenu, void * sender) {
      // Close the warning and then the store menu which are both modals
      storeMenu->dismissModal();
      storeMenu->close();
      return true;
    }, this),
    Invocation::Builder<StoreMenuController>([](StoreMenuController * storeMenu, void * sender) {
      storeMenu->dismissModal();
      return true;
    }, this),
    I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel
    ),
  m_preventReload(false)
{
  m_abortController.setContentMessage(I18n::Message::InvalidInputWarning);
  /* We need to set the width early since minimalSizeForOptimalDisplay will be
   * called before willDisplayCell. */
  m_cell.setFrame(KDRect(0, 0, Ion::Display::Width - Metric::PopUpLeftMargin - Metric::PopUpRightMargin, 0), false);
  m_cell.expressionField()->setTextEditionBuffer(m_savedDraftTextBuffer, AbstractTextField::MaxBufferSize());
}

void StoreMenuController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
  m_cell.expressionField()->reload();
}

void StoreMenuController::setText(const char * text) {
  m_preventReload = true;
  m_cell.expressionField()->setEditing(true);
  m_cell.expressionField()->handleEventWithText(text, false, true);
  m_cell.expressionField()->handleEventWithText("→");
  if (text[0] == 0) {
    m_cell.expressionField()->putCursorLeftOfField();
  }
  m_stackViewController.setupActiveView();
  m_preventReload = false;
}

void StoreMenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  m_cell.reloadCell();
}

void StoreMenuController::layoutFieldDidChangeSize(LayoutField * layoutField) {
  if (m_preventReload) {
    return;
  }
  /* Reloading the store menu will update its frame to match the size of the
   * layout but it will also call layoutFieldDidChangeSize. We set this
   * boolean to break the cycle. */
  m_preventReload = true;
  Container::activeApp()->modalViewController()->reloadModal();
  m_preventReload = false;
}

void StoreMenuController::openAbortWarning() {
  /* We want to open the warning but the current store menu is likely too small
   * to display it entirely. We open the pop-up and then reload which will cause
   * the store menu to be relayouted with the minimalsizeForOptimalDisplay of
   * the warning. We could save a reload by moving the centering logic after the
   * embedded pop-up. */
  displayModalViewController(&m_abortController, KDContext::k_alignCenter, KDContext::k_alignCenter);
  Container::activeApp()->modalViewController()->reloadModal();
}

bool StoreMenuController::parseAndStore(const char * text) {
  Shared::ExpressionFieldDelegateApp * app = static_cast<Shared::ExpressionFieldDelegateApp*>(Container::activeApp());
  Poincare::Context * context = app->localContext();
  Expression input = Expression::Parse(text, context);
  if (input.isUninitialized()) {
    openAbortWarning();
    return false;
  }
  Expression reducedExp = input;
  Shared::PoincareHelpers::CloneAndSimplify(&reducedExp, context, Poincare::ExpressionNode::ReductionTarget::User);
  if (reducedExp.type() != ExpressionNode::Type::Store) {
    openAbortWarning();
    return false;
  }
  bool isVariable = reducedExp.childAtIndex(1).type() == Poincare::ExpressionNode::Type::Symbol;
  if (isVariable && Shared::ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(input, reducedExp, context)) {
    reducedExp.replaceChildAtIndexInPlace(0, Shared::PoincareHelpers::ApproximateKeepingUnits<double>(reducedExp.childAtIndex(0), context));
  }
  Store store = static_cast<Store&>(reducedExp);
  close();
  app->prepareForIntrusiveStorageChange();
  bool storeImpossible = !store.storeValueForSymbol(context);
  app->concludeIntrusiveStorageChange();
  if (storeImpossible) {
    // TODO: we could detect this before the close and open the warning over the
    // store menu
    app->displayWarning(I18n::Message::VariableCantBeEdited);
  }
  return true;
}

bool StoreMenuController::layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) {
  constexpr size_t bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  layoutR.serializeForParsing(buffer, bufferSize);
  return parseAndStore(buffer);
}

bool StoreMenuController::textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  return parseAndStore(text);
}

bool StoreMenuController::layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) {
  /* Since dismissing the controller will call layoutFieldDidChangeSize, we need
   * to set the flag to avoid reloadData from happening which would otherwise
   * setFirstResponder on the store menu while it is hidden. */
  m_preventReload = true;
  close();
  return true;
}

bool StoreMenuController::textFieldDidAbortEditing(Escher::AbstractTextField * textField) {
  close();
  return true;
}

bool StoreMenuController::layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) {
  if (event == Ion::Events::Sto) {
    layoutField->handleEventWithText("→");
    return true;
  }
  // We short circuit the LayoutFieldDelegate to avoid calls to displayWarning
  return textFieldDelegateApp()->fieldDidReceiveEvent(layoutField, layoutField, event);
}

bool StoreMenuController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Sto) {
    textField->handleEventWithText("→");
    return true;
  }
  return textFieldDelegateApp()->fieldDidReceiveEvent(textField, textField, event);
}
