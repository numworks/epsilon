#pragma once

#include <apps/shared/double_pair_store.h>
#include <apps/shared/math_app.h>
#include <apps/shared/store_controller.h>
#include <escher/tab_view_data_source.h>
#include <poincare/tree.h>

namespace Shared {

class StoreApp : public MathApp {
 public:
  class Snapshot : public Shared::MathApp::Snapshot,
                   public Escher::TabViewDataSource {};

  static StoreApp* storeApp() {
    return static_cast<StoreApp*>(Escher::App::app());
  }

  bool memoizeFormula(Poincare::Layout formula, int index);
  Poincare::Layout memoizedFormula(int index) const;

  constexpr static int k_numberOfMemoizedFormulas =
      DoublePairStore::k_numberOfColumnsPerSeries *
      DoublePairStore::k_numberOfSeries;

 protected:
  using MathApp::MathApp;
  virtual const char* memoizedFormulaExtension() const = 0;

 private:
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override {
    return !m_intrusiveStorageChangeFlag ||
           strcmp(recordName.extension, Ion::Storage::listExtension) != 0;
  }
};

}  // namespace Shared
