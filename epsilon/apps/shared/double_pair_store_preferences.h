#pragma once

#include <array>

#include "double_pair_store.h"

namespace Shared {

class DoublePairStorePreferences {
 public:
  DoublePairStorePreferences() {
    // TODO: use std::array::fill(false)
    for (size_t i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
      m_valid[i] = false;
      m_hidden[i] = false;
      m_colorIndex[i] = static_cast<uint8_t>(i);
    }
  }

  bool seriesIsActive(int series) const {
    return m_valid[series] && !m_hidden[series];
  }
  bool seriesIsValid(int series) const { return m_valid[series]; }
  void setSeriesValid(int series, bool valid) { m_valid[series] = valid; }
  void setSeriesHidden(int series, bool hidden) { m_hidden[series] = hidden; }

  uint8_t colorIndexForSeries(int series) const { return m_colorIndex[series]; }
  void setColorIndexForSeries(int series, uint8_t colorIndex) {
    m_colorIndex[series] = colorIndex;
  }

 private:
  std::array<bool, DoublePairStore::k_numberOfSeries> m_valid;
  std::array<bool, DoublePairStore::k_numberOfSeries> m_hidden;
  std::array<uint8_t, DoublePairStore::k_numberOfSeries> m_colorIndex;
};

}  // namespace Shared
