#include "persisting_bytes_viewer_controller.h"

extern "C" {
#include <assert.h>
}

namespace HardwareCheckup {

Escher::View* PersistingBytesViewerController::view() { return &m_view; }

bool PersistingBytesViewerController::handleEvent(Ion::Events::Event event) {
  return false;
}

void PersistingBytesViewerController::viewWillAppear() {
  m_view.setColor(KDColorGreen);
  m_view.textView()->setText("Persisting Bytes");
}

PersistingBytesViewerController::ContentView::ContentView()
    : SolidColorView(KDColorWhite),
      m_textView({.horizontalAlignment = KDGlyph::k_alignCenter}) {}

void PersistingBytesViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_textView.setBackgroundColor(color);
}

void PersistingBytesViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

Escher::View* PersistingBytesViewerController::ContentView::subviewAtIndex(
    int index) {
  assert(index == 0);
  return &m_textView;
}

}  // namespace HardwareCheckup
