#include "storage_viewer_controller.h"

extern "C" {
#include <assert.h>
}

namespace HardwareCheckup {

Escher::View* StorageViewerController::view() { return &m_view; }

bool StorageViewerController::handleEvent(Ion::Events::Event event) {
  return false;
}

void StorageViewerController::viewWillAppear() {
  m_view.setColor(KDColorBlue);
  m_view.textView()->setText("Storage");
}

StorageViewerController::ContentView::ContentView()
    : SolidColorView(KDColorWhite),
      m_textView({.horizontalAlignment = KDGlyph::k_alignCenter}) {}

void StorageViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_textView.setBackgroundColor(color);
}

void StorageViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

Escher::View* StorageViewerController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textView;
}

}  // namespace HardwareCheckup
