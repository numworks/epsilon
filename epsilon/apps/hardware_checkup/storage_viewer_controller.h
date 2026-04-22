#pragma once

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <ion/storage/file_system.h>

namespace HardwareCheckup {

class StorageViewerController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    constexpr static size_t k_lineBufferSize = 45 + 1 + 1;
    constexpr static size_t k_maxNumberOfLine = 16;
    constexpr static size_t k_bufferSize =
        (k_lineBufferSize - 1) * k_maxNumberOfLine + 1;
    ContentView();
    Escher::AbstractBufferTextView* textView() { return &m_textView; }
    void setColor(Escher::ThemeComponent color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_textView;
    }
    Escher::BufferTextView<k_bufferSize> m_textView;
  };
  // Special extension only used here.
  constexpr static const char* k_dummyBaseName = "0";
  constexpr static const char* k_dummyExtension = "dummy";
  constexpr static const char* k_availableExtensions[] = {
      Ion::Storage::parametricComponentExtension,
      Ion::Storage::systemExtension,
      Ion::Storage::statisticsMemoizedFormulaExtension,
      Ion::Storage::regressionMemoizedFormulaExtension,

      Ion::Storage::equationExtension,
      Ion::Storage::expressionExtension,
      Ion::Storage::listExtension,
      Ion::Storage::matrixExtension,

      Ion::Storage::scriptExtension,
      Ion::Storage::sequenceExtension,
      Ion::Storage::regressionFunctionExtension,
      Ion::Storage::regressionListExtension,
      Ion::Storage::functionExtension,

      k_dummyExtension,
  };
  constexpr static int k_numberOfAvailableExtensions =
      sizeof(k_availableExtensions) / sizeof(k_availableExtensions[0]);

  bool createDummyRecord() const;
  bool deleteDummyRecord() const;

  ContentView m_view;
};

}  // namespace HardwareCheckup
