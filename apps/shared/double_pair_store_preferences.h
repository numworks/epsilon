#ifndef SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H
#define SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H

#include "double_pair_store.h"

namespace Shared {

class DoublePairStorePreferences {
public:
  DoublePairStorePreferences() : m_valid{false, false, false}, m_hidden{false, false, false} {}
  static_assert(DoublePairStore::k_numberOfSeries == 3, "Initialization of flags in DoublePairStorePreferences is wrong.");

  bool seriesIsActive(int series) const { return m_valid[series] && !m_hidden[series]; }
  bool seriesIsValid(int series) const { return m_valid[series]; }
  void setSeriesValid(int series, bool valid) { m_valid[series] = valid; }
  void setSeriesHidden(int series, bool hidden) { m_hidden[series] = hidden; }

private:
  bool m_valid[DoublePairStore::k_numberOfSeries];
  bool m_hidden[DoublePairStore::k_numberOfSeries];
};

}

#endif
