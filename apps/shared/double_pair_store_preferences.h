#ifndef SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H
#define SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H

#include "double_pair_store.h"

namespace Shared {

class DoublePairStorePreferences {
public:
  DoublePairStorePreferences() : m_validSeries{false, false, false} {}
  static_assert(DoublePairStore::k_numberOfSeries == 3, "Initialization of DoublePairStorePreferences::m_validSeries is wrong.");

  bool seriesIsValid(int series) const { return m_validSeries[series]; }
  void setSeriesValid(int series, bool valid) { m_validSeries[series] = valid; }

private:
  bool m_validSeries[DoublePairStore::k_numberOfSeries];
};

}

#endif
