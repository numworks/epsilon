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

struct TableData {
  using Label = char[11 + 1];  // 11 (from spec) + \x00

  TableData() {
    for (Label& label : m_groupLabels) {
      label[0] = '\x00';
    }
    for (Label& label : m_categoryLabels) {
      label[0] = '\x00';
    }
    for (GroupStatus& status : m_groupStatus) {
      status = GroupStatus::Empty;
    }
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

  bool isGroupActive(int col) {
    return m_groupStatus[col] == GroupStatus::Active;
  }
  bool isGroupEmpty(int col) {
    if (m_groupStatus[col] == GroupStatus::Hidden) {
      // A hidden group could be empty or not
      return !computeGroupHasValue(col);
    }
    return m_groupStatus[col] == GroupStatus::Empty;
  }

  void setGroupActive(bool active, int col) {
    GroupStatus& status = m_groupStatus[col];
    if (status != GroupStatus::Empty) {
      status = active ? GroupStatus::Active : GroupStatus::Hidden;
    }
  }
  float getValue(int col, int row) { return m_data[col][row]; }

  void setValue(float data, int col, int row) {
    assert(std::isfinite(data));
    m_data[col][row] = data;
    if (m_groupStatus[col] == GroupStatus::Empty) {
      m_groupStatus[col] = GroupStatus::Active;
    }
  }

  void setGroupName(const char* name, int col) {
    assert(0 <= col && col < k_maxNumberOfGroups);
    strlcpy(m_groupLabels[col], name, sizeof(m_groupLabels[col]));
  }

  void getGroupName(int col, char* buffer, int bufferSize) {
    assert(0 <= col && col < k_maxNumberOfGroups);
    if (m_groupLabels[col][0] != '\x00') {
      strlcpy(buffer, m_groupLabels[col], bufferSize);
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
    strlcpy(m_categoryLabels[row], name, sizeof(m_categoryLabels[row]));
  }

  void getCategoryName(int row, char* buffer, int bufferSize) {
    assert(0 <= row && row < k_maxNumberOfCategory);
    if (m_categoryLabels[row][0] != '\x00') {
      strlcpy(buffer, m_categoryLabels[row], bufferSize);
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
    m_data[col][row] = NAN;
    // A hidden column stays hidden even if emptied
    if (m_groupStatus[col] != GroupStatus::Hidden) {
      m_groupStatus[col] =
          computeGroupHasValue(col) ? GroupStatus::Active : GroupStatus::Empty;
    }
  }
  void clearColumn(int col) {
    for (int row = 0; row < k_maxNumberOfCategory; row++) {
      m_data[col][row] = NAN;
    }
    if (m_groupStatus[col] != GroupStatus::Hidden) {
      m_groupStatus[col] = GroupStatus::Empty;
    }
  }
  void clearRow(int row) {
    for (int col = 0; col < k_maxNumberOfGroups; col++) {
      m_data[col][row] = NAN;
      if (m_groupStatus[col] == GroupStatus::Active &&
          !computeGroupHasValue(col)) {
        m_groupStatus[col] = GroupStatus::Empty;
      }
    }
  }

  static constexpr int k_maxNumberOfGroups = 6;
  static constexpr int k_maxNumberOfCategory = 10;

 private:
  bool computeGroupHasValue(int column) {
    for (int row = 0; row < k_maxNumberOfCategory; row++) {
      if (std::isfinite(m_data[column][row])) {
        return true;
      }
    }
    return false;
  }

  enum class GroupStatus : uint8_t {
    Empty,   // Hidden because no value
    Active,  // Shown
    Hidden,  // User chose to hide this group
  };

  std::array<Label, k_maxNumberOfGroups> m_groupLabels = {};
  std::array<GroupStatus, k_maxNumberOfGroups> m_groupStatus = {};
  std::array<Label, k_maxNumberOfCategory> m_categoryLabels = {};
  float m_data[k_maxNumberOfGroups][k_maxNumberOfCategory];
};

}  // namespace Statistics::Categorical
