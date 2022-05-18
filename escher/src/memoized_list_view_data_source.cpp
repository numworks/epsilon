#include <escher/memoized_list_view_data_source.h>
#include <poincare/exception_checkpoint.h>
#include <assert.h>

namespace Escher {

MemoizedListViewDataSource::MemoizedListViewDataSource() :
  m_memoizationLockedLevel(0)
{
  resetMemoization(true);
}

KDCoordinate MemoizedListViewDataSource::rowHeight(int j) {
  if (j < m_memoizedIndexOffset || j >= m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Update memoization index if new cell is called.
    setMemoizationIndex(j);
  }
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Value might be memoized
    KDCoordinate memoizedRowHeight = m_memoizedCellHeight[getMemoizedIndex(j)];
    if (memoizedRowHeight == k_resetMemoizedValue) {
      // Compute and memoize value
      updateMemoizationLock(true);
      memoizedRowHeight = nonMemoizedRowHeight(j);
      updateMemoizationLock(false);
      m_memoizedCellHeight[getMemoizedIndex(j)] = memoizedRowHeight;
    }
    return memoizedRowHeight;
  }
  /* Requested row is neither memoized, nor adjacent (unless list is under lock)
   * If such a configuration where to happen, memoization shift pattern should
   * be updated. */
  assert(m_memoizationLockedLevel > 0);
  return nonMemoizedRowHeight(j);
}

KDCoordinate MemoizedListViewDataSource::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows() && m_memoizedTotalHeight != k_resetMemoizedValue) {
    // Total Height is memoized to save time and preserve memoization.
    return m_memoizedTotalHeight;
  }
  if (j < m_memoizedIndexOffset / 2) {
    return nonMemoizedCumulatedHeightFromIndex(j);
  }
  /* Take advantage of m_memoizedCumulatedHeightOffset as j is closer to
   * m_memoizedIndexOffset than 0. */
  if (m_memoizedCumulatedHeightOffset == k_resetMemoizedValue) {
    // Memoized cumulatedHeight is required and must be recomputed
    updateMemoizationLock(true);
    m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
    updateMemoizationLock(false);
  }
  // Search cumulatedHeight around m_memoizedCumulatedHeightOffset
  KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
  if (j >= m_memoizedIndexOffset) {
    int nRows = numberOfRows();
    for (int i = m_memoizedIndexOffset; i < nRows; i++) {
      if (i == j) {
        return cumulatedHeight;
      }
      // From here on, memoization might be updated.
      cumulatedHeight += rowHeight(i);
    }
    assert(j == nRows);
    // Update memoized total height
    m_memoizedTotalHeight = cumulatedHeight;
    return cumulatedHeight;
  }
  for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
    // From here on, memoization might be updated.
    cumulatedHeight -= rowHeight(i);
    if (i == j) {
      return cumulatedHeight;
    }
  }
  assert(cumulatedHeight == KDCoordinate(0) && j == 0);
  return cumulatedHeight;
}

int MemoizedListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY < m_memoizedCumulatedHeightOffset / 2) {
    return nonMemoizedIndexFromCumulatedHeight(offsetY);
  }
  /* Take advantage of m_memoizedCumulatedHeightOffset as offsetY is closer to
   * m_memoizedCumulatedHeightOffset than 0. */
  if (m_memoizedCumulatedHeightOffset == k_resetMemoizedValue) {
    // Memoized cumulatedHeight is required and must be recomputed
    updateMemoizationLock(true);
    m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
    updateMemoizationLock(false);
  }
  // Search index around m_memoizedIndexOffset
  KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
  if (offsetY > m_memoizedCumulatedHeightOffset) {
    int nRows = numberOfRows();
    for (int i = m_memoizedIndexOffset; i < nRows; i++) {
      // From here on, memoization might be updated.
      cumulatedHeight += rowHeight(i);
      if (offsetY <= cumulatedHeight) {
        return i;
      }
    }
    return nRows;
  }
  for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
    // From here on, memoization might be updated.
    cumulatedHeight -= rowHeight(i);
    if (offsetY > cumulatedHeight) {
      return i;
    }
  }
  assert(cumulatedHeight == 0);
  return 0;
}

void MemoizedListViewDataSource::resetMemoization(bool force) {
  if (!force && m_memoizationLockedLevel > 0) {
    return;
  }
  /* With interruption and exceptions, a memoization lock status may end up
   * corrupted. It is here resetted because called with force. If we were not
   * supposed to do this, it will be caught when next unlock call will trigger
   * an assertion as m_memoizationLockedLevel will be negative. */
  m_memoizationLockedLevel = 0;
  // Reset memoized index and corresponding cumulated height.
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = 0;
  if (force) {
    // Do not preserve total height
    m_memoizedTotalHeight = k_resetMemoizedValue;
  }
  // Reset cell heights
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetMemoizedValue;
  }
}

int MemoizedListViewDataSource::getMemoizedIndex(int index) {
  /* Values are memoized in a circular way : m_memoizedCellHeight[i] only
   * stores values for index such that index%k_memoizedCellsCount == i
   * Eg : 0 1 2 3 4 5 6 shifts to 7 1 2 3 4 5 6 which shifts to 7 8 2 3 4 5 6
   * Shifting the memoization is then more optimized. */
  assert(index >= m_memoizedIndexOffset && index < m_memoizedIndexOffset + k_memoizedCellsCount);
  int circularOffset = m_memoizedIndexOffset % k_memoizedCellsCount;
  return (circularOffset + index - m_memoizedIndexOffset) % k_memoizedCellsCount;
}

void MemoizedListViewDataSource::setMemoizationIndex(int index) {
  if (m_memoizationLockedLevel > 0 || index == m_memoizedIndexOffset) {
    return;
  }

  if (index < m_memoizedIndexOffset / 2 && m_memoizedIndexOffset - index > k_memoizedCellsCount) {
    // New memoization index is too far to be updated by shifting.
    resetMemoization(false);
    m_memoizedIndexOffset = index;
    m_memoizedCumulatedHeightOffset = index == 0 ? 0 : k_resetMemoizedValue;
    return;
  }

  bool lowerIndex = m_memoizedIndexOffset > index;
  // If cell is above new memoization index profit from k_memoizedCellsCount
  int indexDelta = lowerIndex ? m_memoizedIndexOffset - index : index - m_memoizedIndexOffset - k_memoizedCellsCount + 1;
  for (int i = 0; i < indexDelta; ++i) {
    shiftMemoization(lowerIndex);
  }
}

void MemoizedListViewDataSource::shiftMemoization(bool lowerIndex) {
  // Only a limited number of cells' height are memoized.
  if (m_memoizationLockedLevel > 0) {
    // Memoization is locked, do not shift
    return;
  }
  // The new unknown value must be resetted.
  if (lowerIndex) {
    assert(m_memoizedIndexOffset > 0);
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    updateMemoizationLock(true);
    KDCoordinate height = nonMemoizedRowHeight(m_memoizedIndexOffset);
    updateMemoizationLock(false);
    // Compute and set memoized index height
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = height;
    // Update cumulated height
    if (m_memoizedCumulatedHeightOffset != k_resetMemoizedValue) {
      m_memoizedCumulatedHeightOffset-= height;
      assert(m_memoizedCumulatedHeightOffset != k_resetMemoizedValue);
    }
  } else {
    if (m_memoizedIndexOffset > numberOfRows() - k_memoizedCellsCount) {
      // No need to shift further.
      return;
    }
    if (m_memoizedCumulatedHeightOffset != k_resetMemoizedValue) {
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
      updateMemoizationLock(false);
      assert(m_memoizedCumulatedHeightOffset != k_resetMemoizedValue);
    }
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetMemoizedValue;
    m_memoizedIndexOffset++;
  }
}

void MemoizedListViewDataSource::updateMemoizationLock(bool lockUp) {
  /* This lock system is used to ensure the memoization state
   * (m_memoizedIndexOffset) is preserved when performing an action that could
   * potentially alter it. */
  m_memoizationLockedLevel += (lockUp ? 1 : -1);
  assert(m_memoizationLockedLevel >= 0);
}

}
