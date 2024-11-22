#ifndef SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H
#define SHARED_DOUBLE_PAIR_STORE_PREFERENCES_H

#include <array>

#include "double_pair_store.h"

namespace Shared {

class DoublePairStorePreferences {
 public:
  DoublePairStorePreferences() {
    m_valid.fill(false);
    m_hidden.fill(false);
  }

  bool seriesIsActive(int series) const {
    return m_valid[series] && !m_hidden[series];
  }
  bool seriesIsValid(int series) const { return m_valid[series]; }
  void setSeriesValid(int series, bool valid) { m_valid[series] = valid; }
  void setSeriesHidden(int series, bool hidden) { m_hidden[series] = hidden; }

 private:
  std::array<bool, DoublePairStore::k_numberOfSeries> m_valid;
  std::array<bool, DoublePairStore::k_numberOfSeries> m_hidden;
};

}  // namespace Shared

#endif
