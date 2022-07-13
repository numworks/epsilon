#include "single_element_view.h"
#include "app.h"
#include <poincare/print_int.h>

namespace Periodic {

void SingleElementView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);

  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  AtomicNumber z = dataSource->selectedElement();
  assert(ElementsDataBase::IsElement(z));
  DataField::ColorPair colors = dataSource->field()->getColors(z);

  KDRect bgRect((bounds().width() - k_totalSize) / 2 + k_borderSize, (bounds().height() - k_totalSize) / 2 + k_borderSize, k_cellSize, k_cellSize);
  ctx->fillRect(bgRect, colors.bg());

  KDRect borders[4] = {
    KDRect(bgRect.x() - k_borderSize, bgRect.y() - k_borderSize, k_totalSize, k_borderSize), // Top
    KDRect(bgRect.x() - k_borderSize, bgRect.y() + k_cellSize, k_totalSize, k_borderSize), // Bottom
    KDRect(bgRect.x() - k_borderSize, bgRect.y(), k_borderSize, k_cellSize), // Left
    KDRect(bgRect.x() + bgRect.width(), bgRect.y(), k_borderSize, k_cellSize), // Right
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

  KDPoint symbolOrigin(bgRect.x() + (k_cellSize - symbolSize.width() + symbolXOffset) / 2, bgRect.y() + (k_cellSize - symbolSize.height()) / 2);
  ctx->drawString(symbol, symbolOrigin, KDFont::Size::Large, colors.fg(), colors.bg());
  ctx->drawString(zBuffer, KDPoint(symbolOrigin.x() - zSize.width() - k_symbolZAMargin, symbolOrigin.y() + symbolSize.height() - k_ZVerticalOffset), KDFont::Size::Small, colors.fg(), colors.bg());
  ctx->drawString(aBuffer, KDPoint(symbolOrigin.x() - aSize.width() - k_symbolZAMargin, symbolOrigin.y() - aSize.height() + k_AVerticalOffset), KDFont::Size::Small, colors.fg(), colors.bg());
}

}
