#pragma once

#include <escher/theme/data_colors.h>

#include <array>

#include "double_pair_store.h"

namespace Shared {

class DoublePairStorePreferences {
 public:
  DoublePairStorePreferences() {
    // TODO: use std::array::fill(false)
    static_assert(UINT8_MAX >= DoublePairStore::k_numberOfSeries,
                  "Too many series");
    static_assert(
        Escher::k_numberOfDataColors >= DoublePairStore::k_numberOfSeries,
        "Not enough colors in the data color palette to give each series a "
        "default color.");
    for (size_t i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
      m_valid[i] = false;
      m_hidden[i] = false;
      m_colorIndex[i] = static_cast<uint8_t>(i);
    }
  }
  constexpr static bool ValidSeriesIndex(int series) {
    return series >= 0 && series < DoublePairStore::k_numberOfSeries;
  }
  bool seriesIsActive(int series) const {
    assert(ValidSeriesIndex(series));
    return m_valid[series] && !m_hidden[series];
  }
  bool seriesIsValid(int series) const {
    assert(ValidSeriesIndex(series));
    return m_valid[series];
  }
  void setSeriesValid(int series, bool valid) {
    assert(ValidSeriesIndex(series));
    m_valid[series] = valid;
  }
  void setSeriesHidden(int series, bool hidden) {
    assert(ValidSeriesIndex(series));
    m_hidden[series] = hidden;
  }

  uint8_t colorIndexForSeries(int series) const {
    assert(ValidSeriesIndex(series));
    return m_colorIndex[series];
  }
  void setColorIndexForSeries(int series, uint8_t colorIndex) {
    assert(ValidSeriesIndex(series));
    m_colorIndex[series] = colorIndex;
  }

 private:
  std::array<bool, DoublePairStore::k_numberOfSeries> m_valid;
  std::array<bool, DoublePairStore::k_numberOfSeries> m_hidden;
  std::array<uint8_t, DoublePairStore::k_numberOfSeries> m_colorIndex;
};

}  // namespace Shared
