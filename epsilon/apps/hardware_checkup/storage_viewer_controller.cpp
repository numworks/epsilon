#include "storage_viewer_controller.h"

#include <ion/storage/file_system.h>
#include <kandinsky/basic_color.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

namespace HardwareCheckup {

bool StorageViewerController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Plus && createDummyRecord()) ||
      (event == Ion::Events::Minus && deleteDummyRecord())) {
    viewWillAppear();
    return true;
  }
  return false;
}

bool StorageViewerController::createDummyRecord() const {
  Ion::Storage::FileSystem* fs = Ion::Storage::FileSystem::sharedFileSystem;
  // Create dummy record
  if (fs->availableSize() == 0 ||
      fs->createRecordWithExtension(k_dummyBaseName, k_dummyExtension, nullptr,
                                    0) !=
          Ion::Storage::Record::ErrorStatus::None) {
    return false;
  }
  // Give this record all of the available space
  fs->putAvailableSpaceAtEndOfRecord(
      Ion::Storage::Record(k_dummyBaseName, k_dummyExtension));
  return true;
}

bool StorageViewerController::deleteDummyRecord() const {
  return Ion::Storage::Record(k_dummyBaseName, k_dummyExtension).tryToDestroy();
}

StorageViewerController::ContentView::ContentView()
    : SolidColorView(BasicColor::White),
      m_textView({.style = {.font = KDFont::Size::Small},
                  .verticalAlignment = KDGlyph::k_alignTop}) {}

void StorageViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  // TODO: temporary
  m_textView.setBackgroundColor(Escher::ThemeComponent::BackgroundWhite);
}

// Build storage information text and display it.
void StorageViewerController::viewWillAppear() {
  size_t availableSize =
      Ion::Storage::FileSystem::sharedFileSystem->availableSize();
  size_t totalSize = Ion::Storage::FileSystem::k_totalSize;
  size_t usedSize = totalSize - availableSize;
  float ratio = static_cast<float>(usedSize) / totalSize;
  // Set a color between green and red depending on the storage usage.
  m_view.setColor(KDColor::Blend(BasicColor::Red, BasicColor::Green,
                                 static_cast<uint8_t>(255 * ratio)));

  char lineBuffer[ContentView::k_lineBufferSize];
  m_view.textView()->setText("");
  // Display size usage
  Poincare::Print::CustomPrintf(
      lineBuffer, ContentView::k_lineBufferSize,
      "Storage usage: %i / %i  (%*.*ef%%)\n", usedSize, totalSize,
      100.0f * ratio, Poincare::Preferences::PrintFloatMode::Decimal, 3);
  m_view.textView()->appendText(lineBuffer);

  // Add record counts for each extension
  m_view.textView()->appendText("\nRecord count by extension:\n");
  int displayedExtensions = 0;
  for (int i = 0; i < k_numberOfAvailableExtensions; ++i) {
    int count = Ion::Storage::FileSystem::sharedFileSystem
                    ->numberOfRecordsWithExtension(k_availableExtensions[i]);
    Poincare::Print::CustomPrintf(lineBuffer, ContentView::k_lineBufferSize,
                                  "  %s: %i", k_availableExtensions[i], count);
    m_view.textView()->appendText(lineBuffer);
    displayedExtensions++;
    // Line return every 4 extensions
    if (displayedExtensions % 4 == 0) {
      m_view.textView()->appendText("\n");
    }
  }

  // Display available commands
  m_view.textView()->appendText("\n\n");
  m_view.textView()->appendText("Press + to fill space with a dummy record.\n");
  m_view.textView()->appendText("\nPress - to remove it.");
}

void StorageViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView, KDRect(0, 0, bounds().width(), bounds().height()),
                force);
}

}  // namespace HardwareCheckup
