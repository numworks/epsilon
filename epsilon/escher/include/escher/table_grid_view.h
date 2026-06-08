#pragma once

#include <escher/view.h>

namespace Escher {

class TableView;

class TableGridView : public View {
 public:
  TableGridView(const TableView* table);

  void drawRect(KDContext* ctx, KDRect rect) const override {
    // TODO
  }

  KDSize minimalSizeForOptimalDisplay() const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override { return "TableGridView"; }
#endif

  const TableView* table() const { return m_tableView; }

 private:
  const TableView* m_tableView;
};

}  // namespace Escher
