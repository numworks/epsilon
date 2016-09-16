#include "function_cell.h"
#include "list_controller.h"
#include <poincare.h>
#include <assert.h>

FunctionCell::FunctionCell() :
  View(),
  Responder(nullptr),
  m_functionExpressionView(FunctionExpressionView(this)),
  m_functionNameView(FunctionNameView(this, Invocation([](void * context, void * sender) {
    FunctionCell * functionCell = (FunctionCell *)context;
    ListController * listFunction = (ListController *)(functionCell->parentResponder());
    listFunction->configureFunction(functionCell->m_function);
    }, this))),
  m_function(nullptr),
  m_even(false)
{
}

int FunctionCell::numberOfSubviews() const {
  return 2;
}

View * FunctionCell::subviewAtIndex(int index) {
  assert(index == 1 || index == 0);
  if (index == 0) {
    return &m_functionNameView;
  } else {
    return &m_functionExpressionView;
  }

}
void FunctionCell::layoutSubviews() {
  m_functionNameView.setFrame(KDRect(0, 0, k_functionNameWidth, k_functionCellHeight));
  m_functionExpressionView.setFrame(KDRect(k_functionNameWidth, 0, k_functionExpressionWidth, k_functionCellHeight));
}


void FunctionCell::setEven(bool even) {
  m_even = even;
}

void FunctionCell::setFunction(Graph::Function * f) {
  m_function = f;
}

bool FunctionCell::isEven() {
  return m_even;
}

Graph::Function * FunctionCell::function() {
  return m_function;
}


void FunctionCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_functionExpressionView);
 //markRectAsDirty(bounds());
}

void FunctionCell::didResignFirstResponder() {
  //markRectAsDirty(bounds());
}

bool FunctionCell::handleEvent(Ion::Events::Event event){
  switch (event) {
    case Ion::Events::Event::LEFT_ARROW:
      if (!m_functionNameView.isFocused()) {
        app()->setFirstResponder(&m_functionNameView);
      }
      return true;
    case Ion::Events::Event::RIGHT_ARROW:
      if (!m_functionExpressionView.isFocused()) {
        app()->setFirstResponder(&m_functionExpressionView);
      }
      return true;
    default:
      return false;
  }
}
