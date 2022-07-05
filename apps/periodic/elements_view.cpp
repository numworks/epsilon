#include "elements_view.h"

namespace Periodic {

void ElementsView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, k_backgroundColor);
  for (int i = 0; i < k_numberOfCells; i++) {
    AtomicNumber z = elementInCell(i);
    if (z == k_noElement) {
      continue;
    }
    KDRect cell = rectForCell(i);
    if (cell.intersects(rect)) {
      drawElementCell(z, cell, ctx, rect);
    }
  }
}

AtomicNumber ElementsView::elementInCell(size_t cellIndex) const {
  assert(cellIndex < k_numberOfCells);
  constexpr AtomicNumber k_elementsLayout[k_numberOfCells] = {
     1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
     3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,   7,   8,   9,  10,
    11,  12,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,  14,  15,  16,  17,  18,
    19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
    55,  56,   0,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
    87,  88,   0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118,
     0,   0,   0,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
     0,   0,   0,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103,
  };
  return k_elementsLayout[cellIndex];
}

KDRect ElementsView::rectForCell(size_t cellIndex) const {
  size_t row = cellIndex / k_numberOfColumns;
  assert(row < k_numberOfRows);
  size_t col = cellIndex % k_numberOfColumns;
  KDCoordinate x = k_tableLeftMargin + col * (k_cellSize + k_cellMargin);
  KDCoordinate y = k_tableTopMargin + row * (k_cellSize + k_cellMargin);
  constexpr int k_lanthanideRow = 7;
  if (row >= k_lanthanideRow) {
    y += k_lanthanideTopMargin - k_cellMargin;
  }
  return KDRect(x, y, k_cellSize, k_cellSize);
}

void ElementsView::drawElementCell(AtomicNumber z, KDRect cell, KDContext * ctx, KDRect rect) const {
  ElementsViewDataSource::ColorPair colors = m_dataSource->colorPairForElement(z);

  ctx->fillRect(cell.intersectedWith(rect), colors.bg());

  if (z == m_dataSource->selectedElement()) {
    KDRect margins[4] = {
      KDRect(cell.x() - k_cellMargin, cell.y() - k_cellMargin, cell.width() + 2 * k_cellMargin, k_cellMargin), // Top
      KDRect(cell.x() - k_cellMargin, cell.y() + cell.height(), cell.width() + 2 * k_cellMargin, k_cellMargin), // Bottom
      KDRect(cell.x() - k_cellMargin, cell.y(), k_cellMargin, cell.height()), // Left
      KDRect(cell.x() + cell.width(), cell.y(), k_cellMargin, cell.height()), // Right
    };
    for (KDRect r : margins) {
      ctx->fillRect(r.intersectedWith(rect), colors.fg());
    }
  }

  const char * symbol = ElementsDataBase::Symbol(z);
  KDSize symbolSize = KDFont::Font(KDFont::Size::Small)->stringSize(symbol);
  KDPoint textOrigin(cell.x() + (cell.width() - symbolSize.width()) / 2, cell.y() + (cell.height() - symbolSize.height()) / 2 + 1);
  ctx->drawString(symbol, textOrigin, KDFont::Size::Small, colors.fg(), colors.bg());
}

}
