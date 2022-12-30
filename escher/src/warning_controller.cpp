#include <escher/warning_controller.h>
#include <escher/container.h>
#include <algorithm>

namespace Escher {

WarningController::ContentView::ContentView() :
  SolidColorView(KDColorBlack),
  m_textView1(KDFont::Size::Small, (I18n::Message)0, k_middleAlignment, k_middleAlignment, KDColorWhite, KDColorBlack),
  m_textView2(KDFont::Size::Small, (I18n::Message)0, k_middleAlignment, (1.0f-k_shiftedAlignment), KDColorWhite, KDColorBlack)
{
}

void WarningController::ContentView::setLabels(I18n::Message label1, I18n::Message label2) {
  m_textView1.setMessage(label1);
  m_textView2.setMessage(label2);
}

int WarningController::ContentView::numberOfSubviews() const {
  return (strlen(m_textView2.text()) == 0) ? 1 : 2;
}

View * WarningController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View * views[2] = {&m_textView1, &m_textView2};
  return views[index];
}

void WarningController::ContentView::layoutSubviews(bool force) {
  if (numberOfSubviews() == 1) {
    m_textView1.setFrame(bounds(), force);
    m_textView1.setAlignment(k_middleAlignment, k_middleAlignment);
    return;
  }
  assert(numberOfSubviews() == 2);
  KDRect fullBounds = bounds();
  KDCoordinate halfHeight = fullBounds.height()/2;
  m_textView1.setFrame(KDRect(fullBounds.topLeft(), fullBounds.width(), halfHeight), force);
  m_textView1.setAlignment(k_middleAlignment, k_shiftedAlignment);
  m_textView2.setFrame(KDRect(fullBounds.left(), fullBounds.top()+halfHeight, fullBounds.width(), halfHeight), force);
}

KDSize WarningController::ContentView::minimalSizeForOptimalDisplay() const  {
  KDSize textSize1 = m_textView1.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return KDSize(textSize1.width() + k_horizontalMargin, textSize1.height() + 2*k_topAndBottomMargin);
  }
  assert(numberOfSubviews() == 2);
  KDSize textSize2 = m_textView2.minimalSizeForOptimalDisplay();
  return KDSize(std::max(textSize1.width(), textSize2.width()) + k_horizontalMargin,
      textSize1.height() + textSize2.height() + 2*k_topAndBottomMargin + k_middleMargin);
}

WarningController::WarningController(Responder * parentResponder, I18n::Message warningMessage1, I18n::Message warningMessage2) :
  ViewController(parentResponder),
  m_warningMessage1(warningMessage1),
  m_exitOnOKBackEXEOnly(false)
{
}

void WarningController::setLabel(I18n::Message label1, I18n::Message label2, bool specialExitKeys) {
  m_contentView.setLabels(label1, label2);
  m_exitOnOKBackEXEOnly = specialExitKeys;
}

const char * WarningController::title() {
  return I18n::translate(m_warningMessage1);
}

View * WarningController::view() {
  return &m_contentView;
}

bool WarningController::handleEvent(Ion::Events::Event event) {
  if (!Ion::Events::EventCanDiscardPopUp(event) || event == Ion::Events::OnOff) {
    return false;
  }
  if (m_exitOnOKBackEXEOnly && !(event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Back)) {
    return true;
  }
  Container::activeApp()->modalViewController()->dismissModal();
  return true;
}

}
