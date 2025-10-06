#pragma once

#include <string.h>

#include <array>
#include <cmath>

namespace Statistics::Categorical {

using Label = const char[11];

struct TableDimension {
  int col;
  int row;
};

struct TableData {
  TableData() {
    for (float* data = &m_data[0][0];
         data < &m_data[0][0] + k_maxNumberOfGroups * k_maxNumberOfCategory;
         data++) {
      *data = NAN;
    }
  }

  TableDimension currentDimension() {
    int maxRow = 0;
    int maxCol = 0;
    for (int col = 0; col < k_maxNumberOfGroups; col++) {
      for (int row = 0; row < k_maxNumberOfCategory; row++) {
        if (std::isfinite(m_data[col][row])) {
          maxCol = std::max(maxCol, col);
          maxRow = std::max(maxRow, row);
        }
      }
    }
    return TableDimension{maxCol + 1, maxRow + 1};
  }

  static constexpr int k_maxNumberOfGroups = 6;
  static constexpr int k_maxNumberOfCategory = 10;

  std::array<Label, k_maxNumberOfGroups> m_groupLabels = {};
  std::array<Label, k_maxNumberOfCategory> m_categoryLabels = {};
  float m_data[k_maxNumberOfGroups][k_maxNumberOfCategory];
};

}  // namespace Statistics::Categorical
