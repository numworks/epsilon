#ifndef STATISTICS_MULTIPLE_DATA_VIEW_H
#define STATISTICS_MULTIPLE_DATA_VIEW_H

#include "data_view.h"
#include "../store.h"

namespace Statistics {

class MultipleDataView : public DataView {
public:
  MultipleDataView(Store * store) : DataView(), m_store(store) {}
  KDCoordinate subviewHeight();

  // Display
  void reload() override;

  // View
  int numberOfSubviews() const override;
protected:
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutDataSubviews(bool force) override;
  Escher::View * subviewAtIndex(int index) override;

  Store * m_store;
};

}

#endif
