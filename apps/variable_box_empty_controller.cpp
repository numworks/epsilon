#include "variable_box_empty_controller.h"
#include <poincare/layout_helper.h>
#include "i18n.h"
#include <assert.h>

using namespace Poincare;
using namespace Ion;

VariableBoxEmptyController::VariableBoxEmptyView::VariableBoxEmptyView() :
  m_layoutExample(0.5f, 0.5f, KDColorBlack, Palette::WallScreen)
{
  I18n::Message messages[k_numberOfMessages] = {I18n::Message::EmptyVariableBox0, I18n::Message::Default, I18n::Message::EmptyVariableBox1, I18n::Message::EmptyVariableBox2, I18n::Message::EmptyVariableBox3};
  for (int i = 0; i < k_numberOfMessages; i++) {
    m_messages[i].setFontSize(KDText::FontSize::Small);
    m_messages[i].setAlignment(0.5f, 0.5f);
    m_messages[i].setBackgroundColor(Palette::WallScreen);
    m_messages[i].setMessage(messages[i]);
  }
  m_messages[0].setAlignment(0.5f,1.0f);
  m_messages[k_numberOfMessages-1].setAlignment(0.5f,0.0f);
}

void VariableBoxEmptyController::VariableBoxEmptyView::setMessage(I18n::Message message) {
  m_messages[1].setMessage(message);
}

void VariableBoxEmptyController::VariableBoxEmptyView::setLayout(Poincare::Layout layout) {
  m_layoutExample.setLayout(layout);
}

int VariableBoxEmptyController::VariableBoxEmptyView::numberOfSubviews() const {
  return k_numberOfMessages+1;
}

View * VariableBoxEmptyController::VariableBoxEmptyView::subviewAtIndex(int index) {
  if (index == k_layoutRowIndex) {
    return &m_layoutExample;
  }
  if (index < k_layoutRowIndex) {
    return &m_messages[index];
  }
  return &m_messages[index-1];
}

void VariableBoxEmptyController::VariableBoxEmptyView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  KDCoordinate layoutHeight = m_layoutExample.minimalSizeForOptimalDisplay().height();
  KDCoordinate margin = (height - k_numberOfMessages*textHeight-layoutHeight)/2;
  m_layoutExample.setFrame(KDRect(0, margin+3*textHeight, width, layoutHeight));
  KDCoordinate currentHeight = 0;
  for (uint8_t i = 0; i < k_numberOfMessages; i++) {
    if (i == k_layoutRowIndex) {
      currentHeight += layoutHeight;
    }
    KDCoordinate h = i == 0 || i == k_numberOfMessages - 1 ? textHeight+margin : textHeight;
    m_messages[i].setFrame(KDRect(0, currentHeight, width, h));
    currentHeight += h;
  }
}

VariableBoxEmptyController::VariableBoxEmptyController() :
  ViewController(nullptr),
  m_view()
{
}

View * VariableBoxEmptyController::view() {
  return &m_view;
}

void VariableBoxEmptyController::setType(Type type) {
  I18n::Message message;
  Layout layout;
  switch (type) {
    case Type::Expressions:
    {
      message = I18n::Message::EmptyExpressionBox;
      char storeExpression[] = {'3', Ion::Charset::Sto, 'A', 0};
      layout = LayoutHelper::String(storeExpression, sizeof(storeExpression)-1);
      break;
    }
    case Type::Functions:
    {
      message = I18n::Message::EmptyFunctionBox;
      char storeFunction[] = {'3', '+', 'x', Ion::Charset::Sto, 'f', '(', 'x', ')', 0};
      layout = LayoutHelper::String(storeFunction, sizeof(storeFunction)-1);
      break;
    }
    default:
      assert(false);
  }
  m_view.setMessage(message);
  m_view.setLayout(layout);
}
