#ifndef SHARED_STORE_TO_SERIES_H
#define SHARED_STORE_TO_SERIES_H

#include <escher/i18n.h>
#include <poincare/statistics/statistics.h>

#include "double_pair_store.h"

namespace Shared {

class StoreToSeries : public Poincare::Series {
 public:
  StoreToSeries(const DoublePairStore* store, int series)
      : m_store(store), m_series(series) {}
  double getX(int i) const override { return m_store->get(m_series, 0, i); }
  double getY(int i) const override { return m_store->get(m_series, 1, i); }
  int numberOfPairs() const override {
    return m_store->numberOfPairsOfSeries(m_series);
  }

 private:
  const DoublePairStore* m_store;
  int m_series;
};

}  // namespace Shared

#endif
