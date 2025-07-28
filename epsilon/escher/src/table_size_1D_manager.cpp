#include <escher/table_size_1D_manager.h>
#include <escher/table_view_data_source.h>

namespace Escher {

KDCoordinate MemoizedTableSize1DManager::computeSizeAtIndex(int i) {
  assert(i >= 0);
  if (!sizeAtIndexIsMemoized(i)) {
    // Update memoization index.
    setMemoizationIndex(i);
  }
  if (sizeAtIndexIsMemoized(i)) {
    // Value might be memoized
    KDCoordinate memoizedSize = memoizedSizes()[getMemoizedIndex(i)];
    if (memoizedSize == k_undefinedSize) {
      // Compute and memoize value
      lockSizeMemoization(true);
      memoizedSize = nonMemoizedSizeAtIndex(i);
      lockSizeMemoization(false);
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
    lockSizeMemoization(true);
    m_memoizedCumulatedSizeOffset =
        nonMemoizedCumulatedSizeBeforeIndex(m_memoizedIndexOffset);
    assert(m_memoizedCumulatedSizeOffset >= 0);
    lockSizeMemoization(false);
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
      assert(m_memoizedTotalSize >= 0);
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
  if (offset == m_lastCumulatedSize) {
    return m_lastIndexAfterCumulatedSize;
  }
  m_lastCumulatedSize = offset;
  /* Take advantage of m_memoizedCumulatedSizeOffset as offset is closer to
   * m_memoizedCumulatedSizeOffset than 0. */
  if (m_memoizedCumulatedSizeOffset == k_undefinedSize) {
    // Memoized cumulatedSize is required and must be recomputed
    lockSizeMemoization(true);
    m_memoizedCumulatedSizeOffset =
        nonMemoizedCumulatedSizeBeforeIndex(m_memoizedIndexOffset);
    assert(m_memoizedCumulatedSizeOffset >= 0);
    lockSizeMemoization(false);
  }
  // Search index around m_memoizedIndexOffset
  KDCoordinate cumulatedSize = m_memoizedCumulatedSizeOffset;
  if (offset == cumulatedSize) {
    m_lastIndexAfterCumulatedSize = m_memoizedIndexOffset;
    return m_memoizedIndexOffset;
  }
  if (offset > m_memoizedCumulatedSizeOffset) {
    int nLines = numberOfLines();
    for (int i = m_memoizedIndexOffset; i < nLines; i++) {
      // From here on, memoization might be updated.
      cumulatedSize += sizeAtIndex(i);
      if (offset < cumulatedSize) {
        m_lastIndexAfterCumulatedSize = i;
        return i;
      }
    }
    m_lastIndexAfterCumulatedSize = nLines;
    return nLines;
  }
  for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
    // From here on, memoization might be updated.
    cumulatedSize -= sizeAtIndex(i);
    if (offset >= cumulatedSize) {
      m_lastIndexAfterCumulatedSize = i;
      return i;
    }
  }
  assert(cumulatedSize == 0);
  m_lastIndexAfterCumulatedSize = 0;
  return 0;
}

void MemoizedTableSize1DManager::lockSizeMemoization(bool lockUp) const {
  /* This lock system is used to ensure the memoization state
   * (m_memoizedIndexOffset) is preserved when performing an action that could
   * potentially alter it. */
  m_memoizationLockedLevel += (lockUp ? 1 : -1);
  assert(m_memoizationLockedLevel >= 0);
}

void MemoizedTableSize1DManager::resetSizeMemoization(bool force) {
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
  m_lastCumulatedSize = 0;
  m_lastIndexAfterCumulatedSize = 0;
  if (force) {
    // Do not preserve total size
    m_memoizedTotalSize = k_undefinedSize;
  }
  // Reset cell sizes
  for (int i = 0; i < memoizedLinesCount(); i++) {
    memoizedSizes()[i] = k_undefinedSize;
  }
}

#if 0
void MemoizedTableSize1DManager::updateMemoizationForIndex(
    int index, KDCoordinate previousSize, KDCoordinate newSize) {
  if (newSize == k_undefinedSize) {
    newSize = nonMemoizedSizeAtIndex(index);
  }
  m_memoizedTotalSize = m_memoizedTotalSize - previousSize + newSize;
  assert(m_memoizedTotalSize >= 0);
  if (index < m_memoizedIndexOffset) {
    m_memoizedCumulatedSizeOffset =
        m_memoizedCumulatedSizeOffset - previousSize + newSize;
    assert(m_memoizedCumulatedSizeOffset >= 0);
  }
  m_lastCumulatedSize = 0;
  m_lastIndexAfterCumulatedSize = 0;
  if (!sizeAtIndexIsMemoized(index)) {
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
#endif

int MemoizedTableSize1DManager::getMemoizedIndex(int index) const {
  /* Values are memoized in a circular way : m_memoizedSize[i] only
   * stores values for index such that index%k_memoizedCellsCount == i
   * Eg : 0 1 2 3 4 5 6 shifts to 7 1 2 3 4 5 6 which shifts to 7 8 2 3 4 5 6
   * Shifting the memoization is then more optimized. */
  assert(sizeAtIndexIsMemoized(index));
  int circularOffset = m_memoizedIndexOffset % memoizedLinesCount();
  return (circularOffset + index - m_memoizedIndexOffset) %
         memoizedLinesCount();
}

void MemoizedTableSize1DManager::setMemoizationIndex(int index) {
  assert(index >= 0);
  if (m_memoizationLockedLevel > 0 || index == m_memoizedIndexOffset) {
    return;
  }

  if (index < m_memoizedIndexOffset / 2 &&
      m_memoizedIndexOffset - index > memoizedLinesCount()) {
    // New memoization index is too far to be updated by shifting.
    resetSizeMemoization(false);
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
    lockSizeMemoization(true);
    KDCoordinate size = nonMemoizedSizeAtIndex(m_memoizedIndexOffset);
    lockSizeMemoization(false);
    // Compute and set memoized index size
    memoizedSizes()[getMemoizedIndex(m_memoizedIndexOffset)] = size;
    // Update cumulated size
    if (m_memoizedCumulatedSizeOffset != k_undefinedSize) {
      m_memoizedCumulatedSizeOffset -= size;
      assert(m_memoizedCumulatedSizeOffset >= 0);
    }
  } else {
    if (m_memoizedIndexOffset > numberOfLines() - memoizedLinesCount()) {
      // No need to shift further.
      return;
    }
    if (m_memoizedCumulatedSizeOffset != k_undefinedSize) {
      lockSizeMemoization(true);
      m_memoizedCumulatedSizeOffset +=
          nonMemoizedSizeAtIndex(m_memoizedIndexOffset);
      assert(m_memoizedCumulatedSizeOffset >= 0);
      lockSizeMemoization(false);
    }
    // Unknown value is the previous one
    memoizedSizes()[getMemoizedIndex(m_memoizedIndexOffset)] = k_undefinedSize;
    m_memoizedIndexOffset++;
  }
}

}  // namespace Escher
