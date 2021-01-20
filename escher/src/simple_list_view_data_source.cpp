#include <escher/simple_list_view_data_source.h>
#include <assert.h>
#include <iostream>

namespace Escher {

SimpleListViewDataSource::SimpleListViewDataSource() : m_memoizationLockedLevel(0) {
  resetMemoization();
}

void SimpleListViewDataSource::prepareCellForHeightCalculation(HighlightCell * cell, int index) {
  // A non-null implementation of cellWidth is required to compute cell height.
  assert(cellWidth() != 0);
  // Set cell's frame width
  cell->setSize(KDSize(cellWidth(), cell->bounds().height()));
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
}

KDCoordinate SimpleListViewDataSource::rowHeight(int j) {
  // Update memoization when non-memoized cells are called.
  if (j == m_memoizedIndexOffset - 1) {
    shiftMemoization(true);
  } else if (j == m_memoizedIndexOffset + k_memoizedCellsCount) {
    shiftMemoization(false);
  } else if (j == 0 && m_memoizedIndexOffset != 0) {
    resetMemoization(false);
  }
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Value might be memoized
    KDCoordinate memoizedRowHeight = m_memoizedCellHeight[getMemoizedIndex(j)];
    if (memoizedRowHeight == k_resetedMemoizedValue) {
      // Memoize value
      updateMemoizationLock(true);
      memoizedRowHeight = nonMemoizedRowHeight(j);
      updateMemoizationLock(false);
      m_memoizedCellHeight[getMemoizedIndex(j)] = memoizedRowHeight;
    } else {
      // We don't want this assert to update memoization
      assert(updateMemoizationLock(true) && memoizedRowHeight == nonMemoizedRowHeight(j) && updateMemoizationLock(false));
    }
    return memoizedRowHeight;
  }
  assert(m_memoizationLockedLevel > 0); // TODO handle this in memoization
  updateMemoizationLock(true);
  KDCoordinate rowHeight = nonMemoizedRowHeight(j);
  updateMemoizationLock(false);
  return rowHeight;
}

KDCoordinate SimpleListViewDataSource::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows() && m_memoizedTotalHeight != k_resetedMemoizedValue) {
    // Return a memoizedTotal Height to save computation time
    assert(updateMemoizationLock(true) && m_memoizedTotalHeight == nonMemoizedCumulatedHeightFromIndex(j) && updateMemoizationLock(false));
    return m_memoizedTotalHeight;
  }
  /* Take advantage of m_memoizedCumulatedHeightOffset if j is closer to
   * m_memoizedIndexOffset than 0. */
  if (j >= m_memoizedIndexOffset / 2) {
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      // Memoized cumulatedHeight must be recomputed
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
      updateMemoizationLock(false);
    } else {
      assert(updateMemoizationLock(true) && m_memoizedCumulatedHeightOffset == nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset) && updateMemoizationLock(false));
    }

    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (j >= m_memoizedIndexOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        if (i == j) {
          assert(updateMemoizationLock(true) && nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight && updateMemoizationLock(false));
          return cumulatedHeight;
        }
        cumulatedHeight += rowHeight(i);
      }
      assert(updateMemoizationLock(true) && nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight && updateMemoizationLock(false));
      assert(j == numberOfRows());
      // Update memoized total height
      m_memoizedTotalHeight = cumulatedHeight;
      return cumulatedHeight;
    } else {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (i == j) {
          assert(updateMemoizationLock(true) && nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight && updateMemoizationLock(false));
          return cumulatedHeight;
        }
      }
      assert(updateMemoizationLock(true) && nonMemoizedCumulatedHeightFromIndex(j) == KDCoordinate(0) && updateMemoizationLock(false));
      assert(cumulatedHeight == 0 && j == 0);
      return KDCoordinate(0);
    }
  }
  return nonMemoizedCumulatedHeightFromIndex(j);
}

int SimpleListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  /* Take advantage of m_memoizedCumulatedHeightOffset if offsetY is closer to
   * m_memoizedCumulatedHeightOffset than 0. */
  if (offsetY >= m_memoizedCumulatedHeightOffset / 2) {
    // Memoized cumulatedHeight must be recomputed
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      updateMemoizationLock(true);
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
      updateMemoizationLock(false);
    } else {
      assert(updateMemoizationLock(true) && m_memoizedCumulatedHeightOffset == nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset) && updateMemoizationLock(false));
    }
    // If advantageous, search index around m_memoizedIndexOffset
    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (offsetY > m_memoizedCumulatedHeightOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        cumulatedHeight += rowHeight(i);
        if (offsetY <= cumulatedHeight) {
          assert(updateMemoizationLock(true) && nonMemoizedIndexFromCumulatedHeight(offsetY) == i && updateMemoizationLock(false));
          return i;
        }
      }
      assert(updateMemoizationLock(true) && nonMemoizedIndexFromCumulatedHeight(offsetY) == numberOfRows() && updateMemoizationLock(false));
      return numberOfRows();
    } else if (offsetY >= m_memoizedCumulatedHeightOffset / 2) {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (offsetY > cumulatedHeight) {
          assert(updateMemoizationLock(true) && nonMemoizedIndexFromCumulatedHeight(offsetY) == i && updateMemoizationLock(false));
          return i;
        }
      }
      assert(updateMemoizationLock(true) && nonMemoizedIndexFromCumulatedHeight(offsetY) == 0 && updateMemoizationLock(false));
      assert(cumulatedHeight == 0);
      return 0;
    }
  }
  return nonMemoizedIndexFromCumulatedHeight(offsetY);
}


KDCoordinate SimpleListViewDataSource::nonMemoizedRowHeight(int j) {
  // Must be overridden if not all cells are stored as reusable.
  assert(j < numberOfRows());
  assert(numberOfColumns() == 1);
  assert(reusableCellCount(typeAtIndex(j)) == numberOfRows());
  // TODO Hugo : If possible, instantiate a temporary cell here (! type)
  Escher::HighlightCell * cell = reusableCell(j, typeAtIndex(j));
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

int SimpleListViewDataSource::getMemoizedIndex(int index) {
  // Values are memoized in a circular way :  m_memoizedCellHeight[i] only
  // stores values for index such that index%k_memoizedCellsCount == i
  // It is then much simpler to shift the memoization.
  assert(index >= m_memoizedIndexOffset && index < m_memoizedIndexOffset + k_memoizedCellsCount);
  int circularShift = m_memoizedIndexOffset % k_memoizedCellsCount;
  return (index - m_memoizedIndexOffset + circularShift) % k_memoizedCellsCount;
}

// int SimpleListViewDataSource::setMemoizedOffset(int offset) {
  // if (offset > m_memoizedIndexOffset) {
  //   if () {
  // !!! Loops
  //   }
  // }
  // if (offset - m_memoizedIndexOffset >= 7 || m_memoizedIndexOffset - offset >= 7) {
  //   KDCoordinate cumulatedHeightOffset = cumulatedHeightFromIndex
  //   resetMemoization()
  // }
// }

void SimpleListViewDataSource::shiftMemoization(bool newCellIsUnder) {
  if (m_memoizationLockedLevel > 0) {
    // Do not shift
    return;
  }
  // The new unknown value must be reseted.
  if (newCellIsUnder) {
    assert(m_memoizedIndexOffset > 0);
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    KDCoordinate height = nonMemoizedRowHeight(m_memoizedIndexOffset);
    // Compute and set memoized index height
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = height;
    // Update cumulated height
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      m_memoizedCumulatedHeightOffset-= height;
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
  } else {
    assert(m_memoizedIndexOffset <= numberOfRows() - k_memoizedCellsCount);
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
    m_memoizedIndexOffset++;
  }
}

void SimpleListViewDataSource::resetMemoization(bool force) {
  if (m_memoizationLockedLevel > 0) {
    assert(!force);
    return;
  }
  // Reset memoized index and corresponding cumulated height.
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = 0;
  if (force) {
    // No need to always reset total height
    m_memoizedTotalHeight = k_resetedMemoizedValue;
  }
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetedMemoizedValue;
  }
}

void SimpleListViewDataSource::resetMemoizationForIndex(int j) {
  // Memoization shouldn't be under lock
  assert(m_memoizationLockedLevel == 0);
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    m_memoizedCellHeight[getMemoizedIndex(j)] = k_resetedMemoizedValue;
  } else if (j < m_memoizedIndexOffset) {
    m_memoizedCumulatedHeightOffset = k_resetedMemoizedValue;
  }
  m_memoizedTotalHeight = k_resetedMemoizedValue;
}

}
