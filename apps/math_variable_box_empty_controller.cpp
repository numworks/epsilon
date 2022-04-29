#include "math_variable_box_empty_controller.h"
#include "math_variable_box_controller.h"
#include <poincare/layout_helper.h>
#include <apps/i18n.h>
#include <assert.h>

using namespace Escher;

MathVariableBoxEmptyController::MathVariableBoxEmptyView::MathVariableBoxEmptyView() :
  ModalViewEmptyView(),
  m_layoutExample(KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorBlack, Palette::WallScreen)
{
  initMessageViews();
}

void MathVariableBoxEmptyController::MathVariableBoxEmptyView::setLayout(Poincare::Layout layout) {
  m_layoutExample.setLayout(layout);
}

void MathVariableBoxEmptyController::viewDidDisappear() {
  m_view.setLayout(Poincare::Layout());
}

void MathVariableBoxEmptyController::setPage(int pageCastedToInt) {
  MathVariableBoxController::Page page = static_cast<MathVariableBoxController::Page>(pageCastedToInt);
  I18n::Message messages[MathVariableBoxEmptyView::k_numberOfMessages] = {
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::EnableCharacters
  };
  Poincare::Layout layout;
  switch (page) {
    case MathVariableBoxController::Page::Expression:
    {
      messages[0] = I18n::Message::EmptyExpressionBox0;
      messages[1] = I18n::Message::EmptyExpressionBox1;
      messages[2] = I18n::Message::EmptyExpressionBox2;
      const char * storeExpression = "3→A";
      layout = Poincare::LayoutHelper::String(storeExpression, strlen(storeExpression), MathVariableBoxEmptyView::k_font);
      break;
    }
    case MathVariableBoxController::Page::Function:
    {
      messages[0] = I18n::Message::EmptyFunctionBox0;
      messages[1] = I18n::Message::EmptyFunctionBox1;
      messages[2] = I18n::Message::EmptyFunctionBox2;
      const char * storeFunction = "3+x→f(x)";
      layout = Poincare::LayoutHelper::String(storeFunction, strlen(storeFunction), MathVariableBoxEmptyView::k_font);
      break;
    }
    case MathVariableBoxController::Page::Sequence:
    {
      messages[0] = I18n::Message::EmptySequenceBox0;
      messages[1] = I18n::Message::EmptySequenceBox1;
      messages[3] = I18n::Message::Default;
      break;
    }
    case MathVariableBoxController::Page::List:
    {
      messages[0] = I18n::Message::EmptyListBox0;
      messages[1] = I18n::Message::EmptyListBox1;
      messages[2] = I18n::Message::EmptyListBox2;
      const char * storeList = "{3,4}→L";
      layout = Poincare::LayoutHelper::String(storeList, strlen(storeList), MathVariableBoxEmptyView::k_font);
      break;
    }
    case MathVariableBoxController::Page::Matrix:
    {
      messages[0] = I18n::Message::EmptyMatrixBox0;
      messages[1] = I18n::Message::EmptyMatrixBox1;
      messages[2] = I18n::Message::EmptyMatrixBox2;
      const char * storeExpression = "[[1,2][3,4]]→M";
      layout = Poincare::LayoutHelper::String(storeExpression, strlen(storeExpression), MathVariableBoxEmptyView::k_font);
      break;
    }
    default:
      assert(false);
  }
  m_view.setMessages(messages);
  m_view.setLayout(layout);
}
