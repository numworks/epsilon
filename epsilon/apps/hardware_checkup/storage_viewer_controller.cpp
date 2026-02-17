#include "storage_viewer_controller.h"

#include <poincare/preferences.h>

extern "C" {
#include <assert.h>
}
#include <ion/storage/file_system.h>
#include <poincare/print.h>

namespace HardwareCheckup {

bool StorageViewerController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus) {
    const char* extension = Ion::Storage::scriptExtension;
    int newID = 0;
    char baseName[7];
    do {
      newID += 1;
      if (newID > 999) {
        assert(false);
        return false;
      }
      Poincare::Print::CustomPrintf(baseName, 7, "dum%i", newID);
    } while (Ion::Storage::FileSystem::sharedFileSystem->hasRecord(
        Ion::Storage::Record(baseName, extension)));
    size_t dataSize =
        Ion::Storage::FileSystem::sharedFileSystem->availableSize() / 2;
    if (dataSize > 0 &&
        Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
            baseName, extension, this, dataSize) ==
            Ion::Storage::Record::ErrorStatus::None) {
      viewWillAppear();
      return true;
    }
  }
  return false;
}

void StorageViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_textView.setBackgroundColor(color);
}

void StorageViewerController::viewWillAppear() {
  // Build storage information text
  char buffer[ContentView::k_bufferSize];
  const char* extensions[] = {
      Ion::Storage::systemExtension,
      Ion::Storage::scriptExtension,
      Ion::Storage::equationExtension,
      Ion::Storage::expressionExtension,
      Ion::Storage::functionExtension,
      Ion::Storage::listExtension,
      Ion::Storage::sequenceExtension,
      Ion::Storage::matrixExtension,
      Ion::Storage::regressionExtension,
      Ion::Storage::parametricComponentExtension,
      Ion::Storage::regressionMemoizedFormulaExtension,
      Ion::Storage::statisticsMemoizedFormulaExtension,
  };
  constexpr int numberOfExtensions = sizeof(extensions) / sizeof(extensions[0]);

  size_t availableSize =
      Ion::Storage::FileSystem::sharedFileSystem->availableSize();
  size_t totalSize = Ion::Storage::FileSystem::k_totalSize;

  float ratio = static_cast<float>(totalSize - availableSize) / totalSize;
  m_view.setColor(KDColor::Blend(KDColorRed, KDColorGreen,
                                 static_cast<uint8_t>(255 * ratio)));

  // Start with available size
  int len = Poincare::Print::CustomPrintf(
      buffer, ContentView::k_bufferSize, "Storage: %i / %i (%*.*ef%%)\n",
      totalSize - availableSize, totalSize, 100.0f * ratio,
      Poincare::Preferences::PrintFloatMode::Decimal, 3);

  // Add record counts for each extension
  int extensionsPerLine = 0;
  for (int i = 0; i < numberOfExtensions &&
                  strlen(buffer) < ContentView::k_bufferSize - 20;
       ++i) {
    int count = Ion::Storage::FileSystem::sharedFileSystem
                    ->numberOfRecordsWithExtension(extensions[i]);
    if (count > 0) {
      len += Poincare::Print::CustomPrintf(buffer + len,
                                           ContentView::k_bufferSize - len,
                                           "  %s: %i", extensions[i], count);
      extensionsPerLine++;
      if (extensionsPerLine >= 3) {
        len += Poincare::Print::CustomPrintf(
            buffer + len, ContentView::k_bufferSize - len, "\n");
        extensionsPerLine = 0;
      }
    }
  }
  len += Poincare::Print::CustomPrintf(buffer + len,
                                       ContentView::k_bufferSize - len,
                                       "\n\nPress + to add dummy py records");
  m_view.textView()->setText(buffer);
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
