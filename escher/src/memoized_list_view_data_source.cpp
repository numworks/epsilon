#include <escher/memoized_list_view_data_source.h>
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
    if (memoizedRowHeight == k_resetedMemoizedValue) {
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
  if (j == numberOfRows() && m_memoizedTotalHeight != k_resetedMemoizedValue) {
    // Total Height is memoized to save time and preserve memoization.
    return m_memoizedTotalHeight;
  }
  /* Take advantage of m_memoizedCumulatedHeightOffset if j is closer to
   * m_memoizedIndexOffset than 0. */
  if (j >= m_memoizedIndexOffset / 2) {
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      // Memoized cumulatedHeight is required and must be recomputed
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
      updateMemoizationLock(false);
    }
    // Search cumulatedHeight around m_memoizedCumulatedHeightOffset
    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (j >= m_memoizedIndexOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        if (i == j) {
          return cumulatedHeight;
        }
        cumulatedHeight += rowHeight(i);
      }
      assert(j == numberOfRows());
      // Update memoized total height
      m_memoizedTotalHeight = cumulatedHeight;
      return cumulatedHeight;
    } else {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (i == j) {
          return cumulatedHeight;
        }
      }
      assert(cumulatedHeight == 0 && j == 0);
      return KDCoordinate(0);
    }
  }
  return nonMemoizedCumulatedHeightFromIndex(j);
}

int MemoizedListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  /* Take advantage of m_memoizedCumulatedHeightOffset if offsetY is closer to
   * m_memoizedCumulatedHeightOffset than 0. */
  if (offsetY >= m_memoizedCumulatedHeightOffset / 2) {
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      // Memoized cumulatedHeight is required and must be recomputed
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
      updateMemoizationLock(false);
    }
    // Search index around m_memoizedIndexOffset
    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (offsetY > m_memoizedCumulatedHeightOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        cumulatedHeight += rowHeight(i);
        if (offsetY <= cumulatedHeight) {
          return i;
        }
      }
      return numberOfRows();
    } else {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (offsetY > cumulatedHeight) {
          return i;
        }
      }
      assert(cumulatedHeight == 0);
      return 0;
    }
  }
  return nonMemoizedIndexFromCumulatedHeight(offsetY);
}

void MemoizedListViewDataSource::prepareCellForHeightCalculation(HighlightCell * cell, int index) {
  // A non-null implementation of cellWidth is required to compute cell height.
  assert(cellWidth() != 0);
  // Set cell's frame width
  cell->setSize(KDSize(cellWidth(), cell->bounds().height()));
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
}

void MemoizedListViewDataSource::resetMemoization(bool force) {
  if (m_memoizationLockedLevel > 0) {
    // Memoization shouldn't be under lock if called with force
    assert(!force);
    return;
  }
  // Reset memoized index and corresponding cumulated height.
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = 0;
  if (force) {
    // Do not preserve total height
    m_memoizedTotalHeight = k_resetedMemoizedValue;
  }
  // Reset cell heights
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetedMemoizedValue;
  }
}

KDCoordinate MemoizedListViewDataSource::nonMemoizedRowHeight(int j) {
  /* This default implementation must be used carefully, only when all cells are
   * stored as reusable cells. */
  assert(j < numberOfRows() && reusableCellCount(typeAtIndex(j)) > 0);
  /* Most nonMemoizedRowHeight implementations boils down to instantiating a
   * temporary cell on which calling prepareCellForHeightCalculation, and return
   * minimalSizeForOptimalDisplay. However, temporary cell must be instantiated
   * in the type expected in willDisplayCellAtIndex(), which is unknown here. */
  Escher::HighlightCell * cell = reusableCell(j, typeAtIndex(j));
  /* prepareCellForHeightCalculation will set up cell's width, which is required
   * to compute minimal height for optimal display. */
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
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
    m_memoizedCumulatedHeightOffset = index == 0 ? 0 : k_resetedMemoizedValue;
    return;
  }

  bool newCellIsUnder = m_memoizedIndexOffset > index;
  // If cell is above new memoization index profit from k_memoizedCellsCount
  int indexDelta = newCellIsUnder ? m_memoizedIndexOffset - index : index - m_memoizedIndexOffset - k_memoizedCellsCount + 1;
  for (int i = 0; i < indexDelta; ++i) {
    shiftMemoization(newCellIsUnder);
  }
}

void MemoizedListViewDataSource::shiftMemoization(bool newCellIsUnder) {
  // Only a limited number of cells' height are memoized.
  if (m_memoizationLockedLevel > 0) {
    // Memoization is locked, do not shift
    return;
  }
  // The new unknown value must be reseted.
  if (newCellIsUnder) {
    if (m_memoizedIndexOffset <= 0) {
      assert(false);
      return;
    }
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    updateMemoizationLock(true);
    KDCoordinate height = nonMemoizedRowHeight(m_memoizedIndexOffset);
    updateMemoizationLock(false);
    // Compute and set memoized index height
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = height;
    // Update cumulated height
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      m_memoizedCumulatedHeightOffset-= height;
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
  } else {
    if (m_memoizedIndexOffset > numberOfRows() - k_memoizedCellsCount) {
      // No need to shift further.
      return;
    }
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
      updateMemoizationLock(false);
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
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
