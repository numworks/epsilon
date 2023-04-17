#ifndef STORE_APP_H
#define STORE_APP_H

#include <apps/shared/double_pair_store.h>
#include <apps/shared/layout_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include <apps/shared/store_controller.h>
#include <escher/tab_view_data_source.h>

namespace Shared {

class StoreApp : public LayoutFieldDelegateApp {
 public:
  class Snapshot : public Shared::SharedApp::Snapshot,
                   public Escher::TabViewDataSource {
   public:
    Snapshot();

    bool memoizeFormula(Poincare::Layout formula, int index);
    Poincare::Layout memoizedFormula(int index) const;

    constexpr static int k_numberOfMemoizedFormulas =
        DoublePairStore::k_numberOfColumnsPerSeries *
        DoublePairStore::k_numberOfSeries;

   private:
    constexpr static size_t k_bufferSize = Escher::TextField::MaxBufferSize();

    char m_memoizedFormulasBuffer[k_numberOfMemoizedFormulas][k_bufferSize];
  };

  static StoreApp *storeApp() {
    return static_cast<StoreApp *>(Escher::Container::activeApp());
  }

  Snapshot *storeAppSnapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

 protected:
  using LayoutFieldDelegateApp::LayoutFieldDelegateApp;
  virtual StoreController *storeController() = 0;

 private:
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override {
    return !m_intrusiveStorageChangeFlag ||
           strcmp(recordName.extension, Ion::Storage::lisExtension) != 0;
  }
};

}  // namespace Shared
#endif
