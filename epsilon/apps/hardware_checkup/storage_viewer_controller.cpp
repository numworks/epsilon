#include "storage_viewer_controller.h"

#include <ion/storage/file_system.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

namespace HardwareCheckup {

bool StorageViewerController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus) {
    int newID = 0;
    // Longest base name is dum99
    constexpr int baseNameSize = 6;
    char baseName[baseNameSize];
    Ion::Storage::FileSystem* fs = Ion::Storage::FileSystem::sharedFileSystem;
    // Find the first available name with the format "dumX.py" with X a number.
    do {
      newID += 1;
      if (newID > 99) {
        return false;
      }
      Poincare::Print::CustomPrintf(baseName, baseNameSize, "dum%i", newID);
    } while (fs->hasRecord(Ion::Storage::Record(baseName, k_dummyExtension)));
    // Fill half of the remaining storage with dummy data.
    size_t dataSize = fs->availableSize() / 2;
    if (dataSize > 0 && fs->createRecordWithExtension(
                            baseName, k_dummyExtension, this, dataSize) ==
                            Ion::Storage::Record::ErrorStatus::None) {
      viewWillAppear();
      return true;
    }
  }
  return false;
}

StorageViewerController::ContentView::ContentView()
    : SolidColorView(KDColorWhite),
      m_textView({.style = {.font = KDFont::Size::Small},
                  .verticalAlignment = KDGlyph::k_alignTop}) {}

void StorageViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_textView.setBackgroundColor(color);
}

// Build storage information text and display it.
void StorageViewerController::viewWillAppear() {
  size_t availableSize =
      Ion::Storage::FileSystem::sharedFileSystem->availableSize();
  size_t totalSize = Ion::Storage::FileSystem::k_totalSize;
  size_t usedSize = totalSize - availableSize;
  float ratio = static_cast<float>(usedSize) / totalSize;
  // Set a color between green and red depending on the storage usage.
  m_view.setColor(KDColor::Blend(KDColorRed, KDColorGreen,
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
    if (count > 0) {
      Poincare::Print::CustomPrintf(lineBuffer, ContentView::k_lineBufferSize,
                                    "  %s: %i", k_availableExtensions[i],
                                    count);
      m_view.textView()->appendText(lineBuffer);
      displayedExtensions++;
      // Line return every 3 extensions
      if (displayedExtensions % 4 == 0) {
        m_view.textView()->appendText("\n");
      }
    }
  }

  // Display available commands
  m_view.textView()->appendText("\n\n");
  Poincare::Print::CustomPrintf(lineBuffer, ContentView::k_lineBufferSize,
                                "Press + to add dummy %s records.\n",
                                k_dummyExtension);
  m_view.textView()->appendText(lineBuffer);
}

void StorageViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView, KDRect(0, 0, bounds().width(), bounds().height()),
                force);
}

}  // namespace HardwareCheckup
