#include "store.h"

#include <cmath>

namespace Statistics::Categorical {

Store::TableData::TableData() {
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

void Store::recomputeDimensions() {
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
  m_numberOfColumns = maxCol + 1;
  m_numberOfRows = maxRow + 1;
  assert(0 < m_numberOfColumns && m_numberOfColumns <= k_maxNumberOfGroups);
  assert(0 < m_numberOfRows && m_numberOfRows <= k_maxNumberOfCategory);
}

bool Store::isGroupEmpty(int col) const {
  assert(0 <= col && col < k_maxNumberOfGroups);
  if (m_table->m_groupStatus[col] == GroupStatus::Hidden) {
    // A hidden group could be empty or not
    return nonMemoizedIsGroupEmpty(col);
  }
  return m_table->m_groupStatus[col] == GroupStatus::Empty;
}

void Store::setGroupActive(bool active, int col) {
  assert(0 <= col && col < k_maxNumberOfGroups);
  GroupStatus& status = m_table->m_groupStatus[col];
  if (status != GroupStatus::Empty) {
    status = active ? GroupStatus::Active : GroupStatus::Hidden;
  }
}

void Store::setValue(float data, int col, int row) {
  assert(std::isfinite(data));
  assert(0 <= col && col < k_maxNumberOfGroups);
  assert(0 <= row && row < k_maxNumberOfCategory);
  m_table->m_data[col][row] = data;
  if (m_table->m_groupStatus[col] == GroupStatus::Empty) {
    m_table->m_groupStatus[col] = GroupStatus::Active;
  }
  // Setting a value can only make dimension bigger: recompute dimensions lazily
  m_numberOfColumns = std::max(m_numberOfColumns, col + 1);
  m_numberOfRows = std::max(m_numberOfRows, row + 1);
  recomputeSum(col);
}

void Store::getGroupName(int col, char* buffer, int bufferSize) const {
  assert(0 <= col && col < k_maxNumberOfGroups);
  if (m_table->m_groupLabels[col][0] != '\x00') {
    strlcpy(buffer, m_table->m_groupLabels[col], bufferSize);
    return;
  }
  // Default group name
  static_assert('1' + k_maxNumberOfGroups - 1 <= '9');
  char digit = '1' + col;
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %c",
                                I18n::translate(I18n::Message::Group), digit);
}

void Store::getCategoryName(int row, char* buffer, int bufferSize) const {
  assert(0 <= row && row < k_maxNumberOfCategory);
  if (m_table->m_categoryLabels[row][0] != '\x00') {
    strlcpy(buffer, m_table->m_categoryLabels[row], bufferSize);
    return;
  }
  // Default group name
  static_assert('A' + k_maxNumberOfCategory - 1 <= 'Z');
  char letter = 'A' + row;
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %c",
                                I18n::translate(I18n::Message::Category),
                                letter);
}

void Store::eraseValue(int col, int row) {
  assert(0 <= row && row < k_maxNumberOfCategory);
  assert(0 <= col && col < k_maxNumberOfGroups);
  m_table->m_data[col][row] = NAN;
  // A hidden column stays hidden even if emptied
  if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
    m_table->m_groupStatus[col] =
        nonMemoizedIsGroupEmpty(col) ? GroupStatus::Empty : GroupStatus::Active;
  }
  recomputeDimensions();
  recomputeSum(col);
}

void Store::clearColumn(int col) {
  assert(0 <= col && col < k_maxNumberOfGroups);
  for (int row = 0; row < m_numberOfRows; row++) {
    m_table->m_data[col][row] = NAN;
  }
  if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
    m_table->m_groupStatus[col] = GroupStatus::Empty;
  }
  m_sumOfCategories[col] = NAN;
  m_table->m_groupLabels[col][0] = '\x00';
  recomputeDimensions();
}

void Store::clearRow(int clearedRow) {
  assert(0 <= clearedRow && clearedRow < k_maxNumberOfCategory);
  if (clearedRow == m_numberOfRows) {
    // Trying to clear an empty row, just clear the label
    m_table->m_categoryLabels[clearedRow][0] = '\x00';
    return;
  }
  for (int col = 0; col < m_numberOfColumns; col++) {
    // Shift rows up
    for (int row = clearedRow + 1; row < m_numberOfRows; ++row) {
      m_table->m_data[col][row - 1] = m_table->m_data[col][row];
    }
    m_table->m_data[col][m_numberOfRows - 1] = NAN;
    if (m_table->m_groupStatus[col] == GroupStatus::Active &&
        nonMemoizedIsGroupEmpty(col)) {
      m_table->m_groupStatus[col] = GroupStatus::Empty;
    }
  }
  // Shift category label up
  for (int row = clearedRow + 1; row < m_numberOfRows; ++row) {
    strlcpy(m_table->m_categoryLabels[row - 1], m_table->m_categoryLabels[row],
            sizeof(Label));
  }
  m_table->m_categoryLabels[m_numberOfRows - 1][0] = '\x00';
  recomputeDimensions();
  recomputeAllSums();
}

float Store::getRelativeFrequency(int col, int row) const {
  assert(0 <= col && col < k_maxNumberOfGroups);
  assert(0 <= row && row < k_maxNumberOfCategory);
  float value = m_table->m_data[col][row];
  return std::isfinite(value) ? value / m_sumOfCategories[col] : NAN;
}

void Store::recomputeSum(int column) {
  assert(0 <= column && column < k_maxNumberOfGroups);
  float sum = 0;
  bool hasValue = false;
  for (int row = 0; row < m_numberOfRows; row++) {
    if (std::isfinite(m_table->m_data[column][row])) {
      hasValue = true;
      sum += m_table->m_data[column][row];
    }
  }
  m_sumOfCategories[column] = hasValue ? sum : NAN;
}

void Store::recomputeAllSums() {
  for (int col = 0; col < m_numberOfColumns; col++) {
    recomputeSum(col);
  }
}

bool Store::nonMemoizedIsGroupEmpty(int column) const {
  assert(0 <= column && column < k_maxNumberOfGroups);
  for (int row = 0; row < m_numberOfRows; row++) {
    if (std::isfinite(m_table->m_data[column][row])) {
      return false;
    }
  }
  return true;
}

}  // namespace Statistics::Categorical
