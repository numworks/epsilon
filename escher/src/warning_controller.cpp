#include <escher/container.h>
#include <escher/warning_controller.h>

#include <algorithm>

namespace Escher {

WarningController::ContentView::ContentView()
    : SolidColorView(KDColorBlack),
      m_textView1((I18n::Message)0,
                  {{.glyphColor = KDColorWhite,
                    .backgroundColor = KDColorBlack,
                    .font = KDFont::Size::Small},
                   .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_textView2((I18n::Message)0,
                  {{.glyphColor = KDColorWhite,
                    .backgroundColor = KDColorBlack,
                    .font = KDFont::Size::Small},
                   .horizontalAlignment = KDGlyph::k_alignCenter,
                   .verticalAlignment = (1.0f - k_shiftedAlignment)}) {}

void WarningController::ContentView::setLabels(I18n::Message label1,
                                               I18n::Message label2) {
  m_textView1.setMessage(label1);
  m_textView2.setMessage(label2);
}

int WarningController::ContentView::numberOfSubviews() const {
  return (strlen(m_textView2.text()) == 0) ? 1 : 2;
}

View* WarningController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View* views[2] = {&m_textView1, &m_textView2};
  return views[index];
}

void WarningController::ContentView::layoutSubviews(bool force) {
  if (numberOfSubviews() == 1) {
    setChildFrame(&m_textView1, bounds(), force);
    m_textView1.setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    return;
  }
  assert(numberOfSubviews() == 2);
  KDRect fullBounds = bounds();
  KDCoordinate halfHeight = fullBounds.height() / 2;
  setChildFrame(&m_textView1,
                KDRect(fullBounds.topLeft(), fullBounds.width(), halfHeight),
                force);
  m_textView1.setAlignment(KDGlyph::k_alignCenter, k_shiftedAlignment);
  setChildFrame(&m_textView2,
                KDRect(fullBounds.left(), fullBounds.top() + halfHeight,
                       fullBounds.width(), halfHeight),
                force);
}

KDSize WarningController::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize textSize1 = m_textView1.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 1) {
    return KDSize(textSize1.width() + k_horizontalMargin,
                  textSize1.height() + 2 * k_topAndBottomMargin);
  }
  assert(numberOfSubviews() == 2);
  KDSize textSize2 = m_textView2.minimalSizeForOptimalDisplay();
  return KDSize(
      std::max(textSize1.width(), textSize2.width()) + k_horizontalMargin,
      textSize1.height() + textSize2.height() + 2 * k_topAndBottomMargin +
          k_middleMargin);
}

WarningController::WarningController(Responder* parentResponder,
                                     I18n::Message warningMessage1,
                                     I18n::Message warningMessage2)
    : ViewController(parentResponder),
      m_warningMessage1(warningMessage1),
      m_exitOnOKBackEXEOnly(false) {}

void WarningController::setLabel(I18n::Message label1, I18n::Message label2,
                                 bool specialExitKeys) {
  m_contentView.setLabels(label1, label2);
  m_exitOnOKBackEXEOnly = specialExitKeys;
}

const char* WarningController::title() {
  return I18n::translate(m_warningMessage1);
}

View* WarningController::view() { return &m_contentView; }

bool WarningController::handleEvent(Ion::Events::Event event) {
  if (!event.isKeyPress() || event == Ion::Events::OnOff) {
    return false;
  }
  if (m_exitOnOKBackEXEOnly &&
      !(event == Ion::Events::OK || event == Ion::Events::EXE ||
        event == Ion::Events::Back)) {
    return true;
  }
  Container::activeApp()->modalViewController()->dismissModal();
  return true;
}

}  // namespace Escher
