#pragma once

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/print.h>
#include <string.h>

#include <array>
#include <cmath>

namespace Statistics::Categorical {

struct TableDimension {
  int col;
  int row;
};

class Store {
 private:
  enum class GroupStatus : uint8_t {
    Empty,   // Hidden because no value
    Active,  // Shown
    Hidden,  // User chose to hide this group
  };

 public:
  static constexpr int k_maxNumberOfGroups = 6;
  static constexpr int k_maxNumberOfCategory = 10;
  using Label = char[11 + 1];  // 11 (from spec) + \x00

  /* Subpart of the [Store] that will be stored in the snapshot */
  struct TableData {
    friend class Store;
    TableData() {
      for (int group = 0; group < k_maxNumberOfGroups; ++group) {
        m_groupLabels[group][0] = '\x00';
        m_groupStatus[group] = GroupStatus::Empty;
        m_showRelativeFreqColumn[group] = false;
      }
      for (Label& label : m_categoryLabels) {
        label[0] = '\x00';
      }
      for (float* data = &m_data[0][0];
           data < &m_data[0][0] + k_maxNumberOfGroups * k_maxNumberOfCategory;
           data++) {
        *data = NAN;
      }
    }

   private:
    std::array<Label, k_maxNumberOfGroups> m_groupLabels = {};
    std::array<GroupStatus, k_maxNumberOfGroups> m_groupStatus = {};
    std::array<bool, k_maxNumberOfGroups> m_showRelativeFreqColumn = {};
    std::array<Label, k_maxNumberOfCategory> m_categoryLabels = {};
    float m_data[k_maxNumberOfGroups][k_maxNumberOfCategory];
  };

  Store(TableData* tableData) : m_table(tableData) { recomputeAllSums(); }

  TableDimension currentDimension() const {
    int maxRow = 0;
    int maxCol = 0;
    for (int col = 0; col < k_maxNumberOfGroups; col++) {
      for (int row = 0; row < k_maxNumberOfCategory; row++) {
        if (std::isfinite(m_table->m_data[col][row])) {
          maxCol = std::max(maxCol, col);
          maxRow = std::max(maxRow, row);
        }
      }
    }
    return TableDimension{maxCol + 1, maxRow + 1};
  }

  bool isGroupActive(int col) const {
    return m_table->m_groupStatus[col] == GroupStatus::Active;
  }
  bool isGroupEmpty(int col) const {
    if (m_table->m_groupStatus[col] == GroupStatus::Hidden) {
      // A hidden group could be empty or not
      return !computeGroupHasValue(col);
    }
    return m_table->m_groupStatus[col] == GroupStatus::Empty;
  }

  void setGroupActive(bool active, int col) {
    GroupStatus& status = m_table->m_groupStatus[col];
    if (status != GroupStatus::Empty) {
      status = active ? GroupStatus::Active : GroupStatus::Hidden;
    }
  }
  float getValue(int col, int row) const { return m_table->m_data[col][row]; }

  void setValue(float data, int col, int row) {
    assert(std::isfinite(data));
    m_table->m_data[col][row] = data;
    if (m_table->m_groupStatus[col] == GroupStatus::Empty) {
      m_table->m_groupStatus[col] = GroupStatus::Active;
    }
    recomputeSum(col);
  }

  void setGroupName(const char* name, int col) {
    assert(0 <= col && col < k_maxNumberOfGroups);
    strlcpy(m_table->m_groupLabels[col], name, sizeof(Label));
  }

  void getGroupName(int col, char* buffer, int bufferSize) const {
    assert(0 <= col && col < k_maxNumberOfGroups);
    if (m_table->m_groupLabels[col][0] != '\x00') {
      strlcpy(buffer, m_table->m_groupLabels[col], bufferSize);
      return;
    }
    // Default group name
    static_assert('1' + k_maxNumberOfGroups - 1 <= '9');
    char digit = '1' + col;
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%c",
                                  I18n::translate(I18n::Message::Group), digit);
  }

  void setCategoryName(const char* name, int row) {
    assert(0 <= row && row < k_maxNumberOfCategory);
    strlcpy(m_table->m_categoryLabels[row], name, sizeof(Label));
  }

  void getCategoryName(int row, char* buffer, int bufferSize) const {
    assert(0 <= row && row < k_maxNumberOfCategory);
    if (m_table->m_categoryLabels[row][0] != '\x00') {
      strlcpy(buffer, m_table->m_categoryLabels[row], bufferSize);
      return;
    }
    // Default group name
    static_assert('A' + k_maxNumberOfCategory - 1 <= 'Z');
    char digit = 'A' + row;
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %c",
                                  I18n::translate(I18n::Message::Category),
                                  digit);
  }

  void eraseValue(int col, int row) {
    m_table->m_data[col][row] = NAN;
    // A hidden column stays hidden even if emptied
    if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
      m_table->m_groupStatus[col] =
          computeGroupHasValue(col) ? GroupStatus::Active : GroupStatus::Empty;
    }
    recomputeSum(col);
  }
  void clearColumn(int col) {
    for (int row = 0; row < k_maxNumberOfCategory; row++) {
      m_table->m_data[col][row] = NAN;
    }
    if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
      m_table->m_groupStatus[col] = GroupStatus::Empty;
    }
    m_sumOfCategories[col] = NAN;
  }
  void clearRow(int row) {
    for (int col = 0; col < k_maxNumberOfGroups; col++) {
      m_table->m_data[col][row] = NAN;
      if (m_table->m_groupStatus[col] == GroupStatus::Active &&
          !computeGroupHasValue(col)) {
        m_table->m_groupStatus[col] = GroupStatus::Empty;
      }
      recomputeSum(col);
    }
  }

  bool isRelativeFrequencyColumnActive(int col) const {
    return m_table->m_showRelativeFreqColumn[col];
  }
  void setRelativeFrequencyColumn(int col, bool active) {
    m_table->m_showRelativeFreqColumn[col] = active;
  }
  float getRelativeFrequency(int col, int row) const {
    float value = m_table->m_data[col][row];
    return std::isfinite(value) ? value / m_sumOfCategories[col] : NAN;
  }

 private:
  void recomputeSum(int column) {
    assert(0 <= column && column < k_maxNumberOfGroups);
    float sum = 0;
    bool hasValue = false;
    for (int row = 0; row < k_maxNumberOfCategory; row++) {
      if (std::isfinite(m_table->m_data[column][row])) {
        hasValue = true;
        sum += m_table->m_data[column][row];
      }
    }
    m_sumOfCategories[column] = hasValue ? sum : NAN;
  }

  void recomputeAllSums() {
    for (int col = 0; col < k_maxNumberOfGroups; col++) {
      recomputeSum(col);
    }
  }

  bool computeGroupHasValue(int column) const {
    for (int row = 0; row < k_maxNumberOfCategory; row++) {
      if (std::isfinite(m_table->m_data[column][row])) {
        return true;
      }
    }
    return false;
  }

  std::array<float, k_maxNumberOfGroups> m_sumOfCategories;
  TableData* m_table;
};

}  // namespace Statistics::Categorical
