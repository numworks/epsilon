#include <escher/container.h>
#include <escher/warning_controller.h>

#include <algorithm>

namespace Escher {

WarningController::ContentView::ContentView()
    : SolidColorView(KDColorBlack),
      m_textView((I18n::Message)0,
                 {.style = {.glyphColor = KDColorWhite,
                            .backgroundColor = KDColorBlack,
                            .font = KDFont::Size::Small},
                  .horizontalAlignment = KDGlyph::k_alignCenter,
                  .verticalAlignment = KDGlyph::k_alignCenter},
                 k_lineSpacing) {}

void WarningController::ContentView::setLabel(I18n::Message label) {
  m_textView.setMessage(label);
}

int WarningController::ContentView::numberOfSubviews() const { return 1; }

View* WarningController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textView;
}

void WarningController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView, bounds(), force);
}

KDSize WarningController::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(textSize.width() + 2 * k_margin,
                textSize.height() + 2 * k_margin);
}

WarningController::WarningController(Responder* parentResponder,
                                     I18n::Message titleMessage)
    : ViewController(parentResponder),
      m_titleMessage(titleMessage),
      m_exitOnOKBackEXEOnly(false) {}

void WarningController::setLabel(I18n::Message label, bool specialExitKeys) {
  m_contentView.setLabel(label);
  m_exitOnOKBackEXEOnly = specialExitKeys;
}

const char* WarningController::title() {
  return I18n::translate(m_titleMessage);
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
  App::app()->modalViewController()->dismissModal();
  return true;
}

}  // namespace Escher
