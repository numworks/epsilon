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

TableDimension Store::currentDimension() const {
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

bool Store::isGroupEmpty(int col) const {
  if (m_table->m_groupStatus[col] == GroupStatus::Hidden) {
    // A hidden group could be empty or not
    return !computeGroupHasValue(col);
  }
  return m_table->m_groupStatus[col] == GroupStatus::Empty;
}

void Store::setGroupActive(bool active, int col) {
  GroupStatus& status = m_table->m_groupStatus[col];
  if (status != GroupStatus::Empty) {
    status = active ? GroupStatus::Active : GroupStatus::Hidden;
  }
}

void Store::setValue(float data, int col, int row) {
  assert(std::isfinite(data));
  m_table->m_data[col][row] = data;
  if (m_table->m_groupStatus[col] == GroupStatus::Empty) {
    m_table->m_groupStatus[col] = GroupStatus::Active;
  }
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
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%c",
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
  char digit = 'A' + row;
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s %c",
                                I18n::translate(I18n::Message::Category),
                                digit);
}

void Store::eraseValue(int col, int row) {
  m_table->m_data[col][row] = NAN;
  // A hidden column stays hidden even if emptied
  if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
    m_table->m_groupStatus[col] =
        computeGroupHasValue(col) ? GroupStatus::Active : GroupStatus::Empty;
  }
  recomputeSum(col);
}

void Store::clearColumn(int col) {
  for (int row = 0; row < k_maxNumberOfCategory; row++) {
    m_table->m_data[col][row] = NAN;
  }
  if (m_table->m_groupStatus[col] != GroupStatus::Hidden) {
    m_table->m_groupStatus[col] = GroupStatus::Empty;
  }
  m_sumOfCategories[col] = NAN;
}

void Store::clearRow(int row) {
  for (int col = 0; col < k_maxNumberOfGroups; col++) {
    m_table->m_data[col][row] = NAN;
    if (m_table->m_groupStatus[col] == GroupStatus::Active &&
        !computeGroupHasValue(col)) {
      m_table->m_groupStatus[col] = GroupStatus::Empty;
    }
    recomputeSum(col);
  }
}

float Store::getRelativeFrequency(int col, int row) const {
  float value = m_table->m_data[col][row];
  return std::isfinite(value) ? value / m_sumOfCategories[col] : NAN;
}

void Store::recomputeSum(int column) {
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

void Store::recomputeAllSums() {
  for (int col = 0; col < k_maxNumberOfGroups; col++) {
    recomputeSum(col);
  }
}

bool Store::computeGroupHasValue(int column) const {
  for (int row = 0; row < k_maxNumberOfCategory; row++) {
    if (std::isfinite(m_table->m_data[column][row])) {
      return true;
    }
  }
  return false;
}

}  // namespace Statistics::Categorical
