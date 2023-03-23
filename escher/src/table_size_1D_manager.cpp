#include <escher/table_size_1D_manager.h>
#include <escher/table_view_data_source.h>

namespace Escher {

KDCoordinate MemoizedTableSize1DManager::computeSizeAtIndex(int i) {
  if (i < m_memoizedIndexOffset ||
      i >= m_memoizedIndexOffset + memoizedLinesCount()) {
    // Update memoization index.
    setMemoizationIndex(i);
  }
  if (i >= m_memoizedIndexOffset &&
      i < m_memoizedIndexOffset + memoizedLinesCount()) {
    // Value might be memoized
    KDCoordinate memoizedSize = memoizedSizes()[getMemoizedIndex(i)];
    if (memoizedSize == k_undefinedSize) {
      // Compute and memoize value
      lockMemoization(true);
      memoizedSize = nonMemoizedSizeAtIndex(i);
      lockMemoization(false);
      memoizedSizes()[getMemoizedIndex(i)] = memoizedSize;
    }
    return memoizedSize;
  }
  /* Requested size is neither memoized, nor adjacent (unless it's locked)
   * If such a configuration where to happen, memoization shift pattern should
   * be updated. */
  assert(m_memoizationLockedLevel > 0);
  return k_undefinedSize;
}

KDCoordinate MemoizedTableSize1DManager::computeCumulatedSizeBeforeIndex(
    int i, KDCoordinate defaultSize) {
  int totalNumberOfLines = numberOfLines();
  if (i == totalNumberOfLines && m_memoizedTotalSize != k_undefinedSize) {
    // Total size is memoized to save time and preserve memoization.
    return m_memoizedTotalSize;
  }
  if (i < m_memoizedIndexOffset / 2) {
    return k_undefinedSize;
  }
  /* Take advantage of m_memoizedCumulatedSizeOffset as j is closer to
   * m_memoizedIndexOffset than 0. */
  if (m_memoizedCumulatedSizeOffset == k_undefinedSize) {
    // Memoized cumultaedSize is required and must be recomputed
    lockMemoization(true);
    m_memoizedCumulatedSizeOffset =
        nonMemoizedCumulatedSizeBeforeIndex(m_memoizedIndexOffset);
    lockMemoization(false);
  }
  // Search cumulatedSize around m_memoizedCumulatedSizeOffset
  KDCoordinate cumulatedSize = m_memoizedCumulatedSizeOffset;
  if (i >= m_memoizedIndexOffset) {
    for (int k = m_memoizedIndexOffset; k < totalNumberOfLines; k++) {
      if (i == k) {
        break;
      }
      // From here on, memoization might be updated.
      cumulatedSize += sizeAtIndex(k);
    }
    if (i == totalNumberOfLines) {
      // Update memoized total size
      m_memoizedTotalSize = cumulatedSize;
    }
  } else {
    for (int k = m_memoizedIndexOffset - 1; k >= 0; k--) {
      // From here on, memoization might be updated.
      cumulatedSize -= sizeAtIndex(k);
      if (i == k) {
        break;
      }
    }
  }
  return cumulatedSize;
}

int MemoizedTableSize1DManager::computeIndexAfterCumulatedSize(
    KDCoordinate offset, KDCoordinate defaultSize) {
  if (offset < m_memoizedCumulatedSizeOffset / 2) {
    return k_undefinedSize;
  }
  /* Take advantage of m_memoizedCumulatedSizeOffset as offset is closer to
   * m_memoizedCumulatedSizeOffset than 0. */
  if (m_memoizedCumulatedSizeOffset == k_undefinedSize) {
    // Memoized cumulatedSize is required and must be recomputed
    lockMemoization(true);
    m_memoizedCumulatedSizeOffset =
        nonMemoizedCumulatedSizeBeforeIndex(m_memoizedIndexOffset);
    lockMemoization(false);
  }
  // Search index around m_memoizedIndexOffset
  KDCoordinate cumulatedSize = m_memoizedCumulatedSizeOffset;
  if (offset == cumulatedSize) {
    return m_memoizedIndexOffset;
  }
  if (offset > m_memoizedCumulatedSizeOffset) {
    int nLines = numberOfLines();
    for (int i = m_memoizedIndexOffset; i < nLines; i++) {
      // From here on, memoization might be updated.
      cumulatedSize += sizeAtIndex(i);
      if (offset < cumulatedSize) {
        return i;
      }
    }
    return nLines;
  }
  for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
    // From here on, memoization might be updated.
    cumulatedSize -= sizeAtIndex(i);
    if (offset >= cumulatedSize) {
      return i;
    }
  }
  assert(cumulatedSize == 0);
  return 0;
}

void MemoizedTableSize1DManager::lockMemoization(bool lockUp) const {
  /* This lock system is used to ensure the memoization state
   * (m_memoizedIndexOffset) is preserved when performing an action that could
   * potentially alter it. */
  m_memoizationLockedLevel += (lockUp ? 1 : -1);
  assert(m_memoizationLockedLevel >= 0);
}

void MemoizedTableSize1DManager::resetMemoization(bool force) {
  if (!force && m_memoizationLockedLevel > 0) {
    return;
  }
  /* With interruption and exceptions, a memoization lock status may end up
   * corrupted. It is here resetted because called with force. If we were not
   * supposed to do this, it will be caught when next unlock call will trigger
   * an assertion as m_memoizationLockedLevel will be negative. */
  m_memoizationLockedLevel = 0;
  // Reset memoized index and corresponding cumulated size.
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedSizeOffset = 0;
  if (force) {
    // Do not preserve total size
    m_memoizedTotalSize = k_undefinedSize;
  }
  // Reset cell sizes
  for (int i = 0; i < memoizedLinesCount(); i++) {
    memoizedSizes()[i] = k_undefinedSize;
  }
}

void MemoizedTableSize1DManager::updateMemoizationForIndex(
    int index, KDCoordinate previousSize, KDCoordinate newSize) {
  if (newSize == k_undefinedSize) {
    newSize = nonMemoizedSizeAtIndex(index);
  }
  m_memoizedTotalSize = m_memoizedTotalSize - previousSize + newSize;
  if (index >= m_memoizedIndexOffset + memoizedLinesCount()) {
    return;
  }
  if (index < m_memoizedIndexOffset) {
    m_memoizedCumulatedSizeOffset =
        m_memoizedCumulatedSizeOffset - previousSize + newSize;
    return;
  }
  memoizedSizes()[getMemoizedIndex(index)] = newSize;
}

void MemoizedTableSize1DManager::deleteIndexFromMemoization(
    int index, KDCoordinate previousSize) {
  m_memoizedTotalSize -= previousSize;
  if (index >= m_memoizedIndexOffset + memoizedLinesCount()) {
    return;
  }
  if (index <= m_memoizedIndexOffset) {
    m_memoizedCumulatedSizeOffset -= previousSize;
    return;
  }
  for (int i = index; i < m_memoizedIndexOffset + memoizedLinesCount() - 1;
       i++) {
    memoizedSizes()[getMemoizedIndex(i)] =
        memoizedSizes()[getMemoizedIndex(i + 1)];
  }
  memoizedSizes()[getMemoizedIndex(m_memoizedIndexOffset +
                                   memoizedLinesCount() - 1)] = k_undefinedSize;
}

int MemoizedTableSize1DManager::getMemoizedIndex(int index) const {
  /* Values are memoized in a circular way : m_memoizedSize[i] only
   * stores values for index such that index%k_memoizedCellsCount == i
   * Eg : 0 1 2 3 4 5 6 shifts to 7 1 2 3 4 5 6 which shifts to 7 8 2 3 4 5 6
   * Shifting the memoization is then more optimized. */
  assert(index >= m_memoizedIndexOffset &&
         index < m_memoizedIndexOffset + memoizedLinesCount());
  int circularOffset = m_memoizedIndexOffset % memoizedLinesCount();
  return (circularOffset + index - m_memoizedIndexOffset) %
         memoizedLinesCount();
}

void MemoizedTableSize1DManager::setMemoizationIndex(int index) {
  if (m_memoizationLockedLevel > 0 || index == m_memoizedIndexOffset) {
    return;
  }

  if (index < m_memoizedIndexOffset / 2 &&
      m_memoizedIndexOffset - index > memoizedLinesCount()) {
    // New memoization index is too far to be updated by shifting.
    resetMemoization(false);
    m_memoizedIndexOffset = index;
    m_memoizedCumulatedSizeOffset = index == 0 ? 0 : k_undefinedSize;
    return;
  }

  bool lowerIndex = m_memoizedIndexOffset > index;
  // If cell is above new memoization index profit from k_memoizedCellsCount
  int indexDelta =
      lowerIndex ? m_memoizedIndexOffset - index
                 : index - m_memoizedIndexOffset - memoizedLinesCount() + 1;
  for (int i = 0; i < indexDelta; ++i) {
    shiftMemoization(lowerIndex);
  }
}

void MemoizedTableSize1DManager::shiftMemoization(bool lowerIndex) {
  // Only a limited number of cells' size are memoized.
  if (m_memoizationLockedLevel > 0) {
    // Memoization is locked, do not shift
    return;
  }
  // The new unknown value must be resetted.
  if (lowerIndex) {
    assert(m_memoizedIndexOffset > 0);
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    lockMemoization(true);
    KDCoordinate size = nonMemoizedSizeAtIndex(m_memoizedIndexOffset);
    lockMemoization(false);
    // Compute and set memoized index size
    memoizedSizes()[getMemoizedIndex(m_memoizedIndexOffset)] = size;
    // Update cumulated size
    if (m_memoizedCumulatedSizeOffset != k_undefinedSize) {
      m_memoizedCumulatedSizeOffset -= size;
      assert(m_memoizedCumulatedSizeOffset != k_undefinedSize);
    }
  } else {
    if (m_memoizedIndexOffset > numberOfLines() - memoizedLinesCount()) {
      // No need to shift further.
      return;
    }
    if (m_memoizedCumulatedSizeOffset != k_undefinedSize) {
      lockMemoization(true);
      m_memoizedCumulatedSizeOffset +=
          nonMemoizedSizeAtIndex(m_memoizedIndexOffset);
      lockMemoization(false);
      assert(m_memoizedCumulatedSizeOffset != k_undefinedSize);
    }
    // Unknown value is the previous one
    memoizedSizes()[getMemoizedIndex(m_memoizedIndexOffset)] = k_undefinedSize;
    m_memoizedIndexOffset++;
  }
}

template <int N>
int MemoizedColumnWidthManager<N>::numberOfLines() const {
  return this->m_dataSource->numberOfColumns();
}

template <int N>
KDCoordinate MemoizedColumnWidthManager<N>::sizeAtIndex(int i) const {
  return this->m_dataSource->columnWidth(i);
}

template <int N>
KDCoordinate MemoizedColumnWidthManager<N>::nonMemoizedSizeAtIndex(
    int i) const {
  return this->m_dataSource->nonMemoizedColumnWidth(i, true);
}

template <int N>
KDCoordinate MemoizedColumnWidthManager<N>::nonMemoizedCumulatedSizeBeforeIndex(
    int i) const {
  return this->m_dataSource->nonMemoizedCumulatedWidthBeforeIndex(i);
}

template <int N>
int MemoizedRowHeightManager<N>::numberOfLines() const {
  return this->m_dataSource->numberOfRows();
}

template <int N>
KDCoordinate MemoizedRowHeightManager<N>::sizeAtIndex(int i) const {
  return this->m_dataSource->rowHeight(i);
}

template <int N>
KDCoordinate MemoizedRowHeightManager<N>::nonMemoizedSizeAtIndex(int i) const {
  return this->m_dataSource->nonMemoizedRowHeight(i, true);
}

template <int N>
KDCoordinate MemoizedRowHeightManager<N>::nonMemoizedCumulatedSizeBeforeIndex(
    int i) const {
  return this->m_dataSource->nonMemoizedCumulatedHeightBeforeIndex(i);
}

template class MemoizedColumnWidthManager<7>;

template class MemoizedRowHeightManager<1>;
template class MemoizedRowHeightManager<7>;
template class MemoizedRowHeightManager<10>;

}  // namespace Escher
