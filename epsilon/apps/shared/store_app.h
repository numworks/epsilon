#ifndef STORE_APP_H
#define STORE_APP_H

#include <apps/shared/double_pair_store.h>
#include <apps/shared/math_app.h>
#include <apps/shared/store_controller.h>
#include <escher/tab_view_data_source.h>

namespace Shared {

class StoreApp : public MathApp {
 public:
  class Snapshot : public Shared::MathApp::Snapshot,
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

  static StoreApp* storeApp() {
    return static_cast<StoreApp*>(Escher::App::app());
  }

  Snapshot* storeAppSnapshot() const {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }

 protected:
  using MathApp::MathApp;
  virtual StoreController* storeController() = 0;

 private:
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override {
    return !m_intrusiveStorageChangeFlag ||
           strcmp(recordName.extension, Ion::Storage::listExtension) != 0;
  }
};

}  // namespace Shared
#endif
