#include "focused_view_controller.h"

#include <ion/backlight.h>
#include <ion/display.h>
#include <kandinsky/basic_color.h>
#include <omg/unreachable.h>

namespace HardwareTest {

void FocusedViewController::viewWillAppear() {
  m_success = runTest();
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
  Ion::Display::setFocus(k_focusRatio, KDPoint(0, 0));
  KDColor backgroundColor = (m_success ? BasicColor::Green : BasicColor::Red);
  m_view.setTexts(testName(), m_success ? "OK" : "KO");
  m_view.setBackgroundColor(backgroundColor);
}

bool FocusedViewController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (event == Ion::Events::OK && m_success) {
    /* This can't be done in ViewWillDisappear because viewWillAppear of next
     * view is called before. */
    Ion::Display::resetFocus();
    // Handled in WizardViewController
    return false;
  }
  return true;
}

FocusedViewController::ContentView::ContentView()
    : SolidColorView(BasicColor::White),
      m_prefixView({.style = {.font = KDFont::Size::Small},
                    .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_testNameView({.horizontalAlignment = KDGlyph::k_alignCenter}),
      m_testResultView({.horizontalAlignment = KDGlyph::k_alignCenter}),
      m_suffixView1({
          .style = {.font = KDFont::Size::Small},
      }),
      m_suffixView2({
          .style = {.font = KDFont::Size::Small},
      }) {}

Escher::View* FocusedViewController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 5);
  switch (index) {
    case 0:
      return &m_prefixView;
    case 1:
      return &m_testNameView;
    case 2:
      return &m_testResultView;
    case 3:
      return &m_suffixView1;
    case 4:
      return &m_suffixView2;
    default:
      OMG_UNREACHABLE;
  }
}

void FocusedViewController::ContentView::setTexts(const char* testName,
                                                  const char* testResult) {
  m_testNameView.setText(testName);
  m_testResultView.setText(testResult);
}

void FocusedViewController::ContentView::setPrefix(const char* prefix) {
  m_prefixView.setText(prefix);
}

void FocusedViewController::ContentView::setSuffix1(const char* suffix1) {
  m_suffixView1.setText(suffix1);
}

void FocusedViewController::ContentView::setSuffix2(const char* suffix2) {
  m_suffixView2.setText(suffix2);
}

void FocusedViewController::ContentView::setBackgroundColor(KDColor color) {
  m_prefixView.setBackgroundColor(color);
  m_testNameView.setBackgroundColor(color);
  m_testResultView.setBackgroundColor(color);
  m_suffixView1.setBackgroundColor(color);
  m_suffixView2.setBackgroundColor(color);
  SolidColorView::setColor(color);
}

KDCoordinate FocusedViewController::ContentView::layoutGlyphView(
    Escher::GlyphsView* view, KDCoordinate y, bool force) {
  KDCoordinate textHeight = KDFont::GlyphSize(view->font()).height();
  setChildFrame(
      view,
      KDRect(KDPoint(k_margin, y), KDSize(k_width - 2 * k_margin, textHeight)),
      force);
  return textHeight + k_margin;
}

void FocusedViewController::ContentView::layoutSubviews(bool force) {
  KDCoordinate y = k_margin;
  // Display prefix
  y += layoutGlyphView(&m_prefixView, y, force);
  y += k_margin;
  y += layoutGlyphView(&m_testNameView, y, force);
  y += layoutGlyphView(&m_testResultView, y, force);
  y += k_margin;
  y += layoutGlyphView(&m_suffixView1, y, force);
  y += layoutGlyphView(&m_suffixView2, y, force);
  assert(y <= k_height);
}

}  // namespace HardwareTest
