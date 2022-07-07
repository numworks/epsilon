#include "single_element_view.h"
#include <poincare/print_int.h>

namespace Periodic {

void SingleElementView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);

  AtomicNumber z = m_dataSource->selectedElement();
  assert(1 <= z && z <= ElementsDataBase::k_numberOfElements);
  Coloring::ColorPair colors = m_dataSource->coloring()->colorPairForElement(z);
  KDRect cell(k_outerMargin + k_borderSize, bounds().height() - k_outerMargin - k_borderSize - k_cellSize, k_cellSize, k_cellSize);
  ctx->fillRect(cell, colors.bg());

  KDRect borders[4] = {
    KDRect(cell.x() - k_borderSize, cell.y() - k_borderSize, cell.width() + 2 * k_borderSize, k_borderSize), // Top
    KDRect(cell.x() - k_borderSize, cell.y() + cell.height(), cell.width() + 2 * k_borderSize, k_borderSize), // Bottom
    KDRect(cell.x() - k_borderSize, cell.y(), k_borderSize, cell.height()), // Left
    KDRect(cell.x() + cell.width(), cell.y(), k_borderSize, cell.height()), // Right
  };
  for (KDRect r : borders) {
    ctx->fillRect(r, colors.fg());
  }

  const char * symbol = ElementsDataBase::Symbol(z);
  KDCoordinate symbolXOffset = k_symbolZAMargin;
  KDSize symbolSize = KDFont::Font(KDFont::Size::Large)->stringSize(symbol);

  constexpr size_t k_bufferSize = 4;
  char zBuffer[k_bufferSize];
  int zLength = Poincare::PrintInt::Left(z, zBuffer, k_bufferSize - 1);
  zBuffer[zLength] = '\0';
  KDSize zSize = KDFont::Font(KDFont::Size::Small)->stringSize(zBuffer);

  uint16_t a = ElementsDataBase::NumberOfMass(z);
  char aBuffer[k_bufferSize];
  KDSize aSize = KDSizeZero;
  if (a == ElementData::k_AUnknown) {
    symbolXOffset += zSize.width();
    aBuffer[0] = '\0';
  } else {
    assert(a < 1000);
    int aLength = Poincare::PrintInt::Left(a, aBuffer, k_bufferSize - 1);
    aBuffer[aLength] = '\0';
    aSize = KDFont::Font(KDFont::Size::Small)->stringSize(aBuffer);
    assert(aSize.width() >= zSize.width()); // since A >= Z
    symbolXOffset += aSize.width();
  }

  KDPoint symbolOrigin(cell.x() + (k_cellSize - symbolSize.width() + symbolXOffset) / 2, cell.y() + (k_cellSize - symbolSize.height()) / 2);
  ctx->drawString(symbol, symbolOrigin, KDFont::Size::Large, colors.fg(), colors.bg());
  ctx->drawString(zBuffer, KDPoint(symbolOrigin.x() - zSize.width() - k_symbolZAMargin, symbolOrigin.y() + symbolSize.height() - k_ZVerticalOffset), KDFont::Size::Small, colors.fg(), colors.bg());
  ctx->drawString(aBuffer, KDPoint(symbolOrigin.x() - aSize.width() - k_symbolZAMargin, symbolOrigin.y() - aSize.height() + k_AVerticalOffset), KDFont::Size::Small, colors.fg(), colors.bg());

  const char * name = I18n::translate(ElementsDataBase::Name(z));
  KDSize nameSize = KDFont::Font(KDFont::Size::Small)->stringSize(name);
  KDPoint nameOrigin(
    (cell.x() + cell.width() + k_borderSize + bounds().width() - nameSize.width()) / 2,
    symbolOrigin.y() + k_nameVerticalOffset
  );
  ctx->drawString(name, nameOrigin, KDFont::Size::Small, colors.fg(), k_backgroundColor);
}

}
