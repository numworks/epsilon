#pragma once

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/print.h>
#include <string.h>

#include <array>

namespace Statistics::Categorical {

struct TableDimension {
  int col;
  int row;
};

class Store {
 private:
  enum class GroupStatus : uint8_t {
    Empty,   // Inactive because no value
    Active,  // Active, the group has at least one value
    Hidden,  // Inactive because User chose to (the group can be empty or not)
  };

 public:
  static constexpr int k_maxNumberOfGroups = 6;
  static constexpr int k_maxNumberOfCategory = 10;
  using Label = char[11 + 1];  // 11 (from spec) + \x00

  /* Subpart of the [Store] that will be stored in the snapshot */
  struct TableData {
    friend class Store;
    TableData();

   private:
    std::array<Label, k_maxNumberOfGroups> m_groupLabels = {};
    std::array<GroupStatus, k_maxNumberOfGroups> m_groupStatus = {};
    std::array<bool, k_maxNumberOfGroups> m_showRelativeFreqColumn = {};
    std::array<Label, k_maxNumberOfCategory> m_categoryLabels = {};
    float m_data[k_maxNumberOfGroups][k_maxNumberOfCategory];
  };

  Store(TableData* tableData) : m_table(tableData) {
    recomputeDimensions();
    recomputeAllSums();
  }

  /* Returns the max dimension of the current data in each axis */
  TableDimension currentDimension() const {
    return TableDimension{m_numberOfColumns, m_numberOfRows};
  }

  // Group status
  void setGroupActive(bool active, int col);
  bool isGroupActive(int col) const {
    return m_table->m_groupStatus[col] == GroupStatus::Active;
  }
  bool isGroupEmpty(int col) const;

  // Data
  float getValue(int col, int row) const {
    assert(0 <= col && col < k_maxNumberOfGroups);
    assert(0 <= row && row < k_maxNumberOfCategory);
    return m_table->m_data[col][row];
  }
  void setValue(float data, int col, int row);
  bool authorizedValue(float data) { return data >= 0; }

  // Labels
  void getGroupName(int col, char* buffer, int bufferSize) const;
  void setGroupName(const char* name, int col) {
    assert(0 <= col && col < k_maxNumberOfGroups);
    strlcpy(m_table->m_groupLabels[col], name, sizeof(Label));
  }
  void getCategoryName(int row, char* buffer, int bufferSize) const;
  void setCategoryName(const char* name, int row) {
    assert(0 <= row && row < k_maxNumberOfCategory);
    strlcpy(m_table->m_categoryLabels[row], name, sizeof(Label));
  }

  // Erase & Clear
  void eraseValue(int col, int row);
  void clearColumn(int col);
  void clearRow(int row);

  // RF releated
  bool isRelativeFrequencyColumnActive(int col) const {
    assert(0 <= col && col < k_maxNumberOfGroups);
    return m_table->m_showRelativeFreqColumn[col];
  }
  void setRelativeFrequencyColumn(int col, bool active) {
    assert(0 <= col && col < k_maxNumberOfGroups);
    m_table->m_showRelativeFreqColumn[col] = active;
  }
  float getRelativeFrequency(int col, int row) const;

 private:
  void recomputeDimensions();
  void recomputeSum(int column);
  void recomputeAllSums();
  bool nonMemoizedIsGroupEmpty(int column) const;

  std::array<float, k_maxNumberOfGroups> m_sumOfCategories;
  int m_numberOfRows;
  int m_numberOfColumns;
  TableData* m_table;
};

}  // namespace Statistics::Categorical
