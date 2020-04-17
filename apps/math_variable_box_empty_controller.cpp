#include "math_variable_box_empty_controller.h"
#include <poincare/layout_helper.h>
#include <apps/i18n.h>
#include <assert.h>

MathVariableBoxEmptyController::MathVariableBoxEmptyView::MathVariableBoxEmptyView() :
  VariableBoxEmptyView(),
  m_layoutExample(0.5f, 0.5f, KDColorBlack, Palette::WallScreen)
{
  initMessageViews();
}

void MathVariableBoxEmptyController::MathVariableBoxEmptyView::setLayout(Poincare::Layout layout) {
  m_layoutExample.setLayout(layout);
}

void MathVariableBoxEmptyController::viewDidDisappear() {
  m_view.setLayout(Poincare::Layout());
}

void MathVariableBoxEmptyController::setType(Type type) {
  I18n::Message messages[MathVariableBoxEmptyView::k_numberOfMessages] = {
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::EnableCharacters
  };
  Poincare::Layout layout;
  switch (type) {
    case Type::Expressions:
    {
      messages[0] = I18n::Message::EmptyExpressionBox0;
      messages[1] = I18n::Message::EmptyExpressionBox1;
      messages[2] = I18n::Message::EmptyExpressionBox2;
      const char * storeExpression = "3→A";
      layout = Poincare::LayoutHelper::String(storeExpression, strlen(storeExpression), MathVariableBoxEmptyView::k_font);
      break;
    }
    case Type::Functions:
    {
      messages[0] = I18n::Message::EmptyFunctionBox0;
      messages[1] = I18n::Message::EmptyFunctionBox1;
      messages[2] = I18n::Message::EmptyFunctionBox2;
      const char * storeFunction = "3+x→f(x)";
      layout = Poincare::LayoutHelper::String(storeFunction, strlen(storeFunction), MathVariableBoxEmptyView::k_font);
      break;
    }
    default:
      assert(false);
  }
  m_view.setMessages(messages);
  m_view.setLayout(layout);
}
