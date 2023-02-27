#ifndef STORE_APP_H
#define STORE_APP_H

#include <apps/shared/expression_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include <escher/tab_view_data_source.h>

namespace Shared {

class StoreApp : public ExpressionFieldDelegateApp {
 public:
  class Snapshot : public Shared::SharedApp::Snapshot,
                   public Escher::TabViewDataSource {
    // TODO: Memoize formulas of FillColumnWithFormula
  };

  static StoreApp *storeApp() {
    return static_cast<StoreApp *>(Escher::Container::activeApp());
  }

 protected:
  using ExpressionFieldDelegateApp::ExpressionFieldDelegateApp;

 private:
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override {
    return !m_intrusiveStorageChangeFlag ||
           strcmp(recordName.extension, Ion::Storage::lisExtension) != 0;
  }
};

}  // namespace Shared
#endif
