#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>
#include <float.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header),
  m_titleCells{}
{
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  Shared::StoreTitleCell * mytitleCell = static_cast<Shared::StoreTitleCell *>(cell);
  mytitleCell->setSeparatorLeft(i % Store::k_numberOfColumnsPerSeries == 0);
  int seriesIndex = i/Store::k_numberOfColumnsPerSeries;
  bool valuesColumn = i%Store::k_numberOfColumnsPerSeries == 0;
  assert(seriesIndex >= 0 && seriesIndex < FloatPairStore::k_numberOfSeries);
  if (valuesColumn) {
    I18n::Message valuesMessages[] = {I18n::Message::Values1, I18n::Message::Values2, I18n::Message::Values3};
    mytitleCell->setText(I18n::translate(valuesMessages[seriesIndex]));
  } else {
    I18n::Message sizesMessages[] = {I18n::Message::Sizes1, I18n::Message::Sizes2, I18n::Message::Sizes3};
    mytitleCell->setText(I18n::translate(sizesMessages[seriesIndex]));
  }
  KDColor colors[] = {Palette::Red, Palette::Blue, Palette::Green};
  mytitleCell->setColor(m_store->numberOfPairsOfSeries(seriesIndex) == 0 ? Palette::GreyDark : colors[seriesIndex]); // TODO Share GreyDark and other colors with graph/list_controller
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return m_titleCells[index];
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (std::fabs(floatBody) > FLT_MAX) {
    return false;
  }
  if (columnIndex == 1) {
    if (floatBody < 0) {
      return false;
    }
  }
  return Shared::StoreController::setDataAtLocation(floatBody, columnIndex, rowIndex);
}

View * StoreController::loadView() {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    m_titleCells[i] = new Shared::StoreTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small);
  }
  return Shared::StoreController::loadView();
}

void StoreController::unloadView(View * view) {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  Shared::StoreController::unloadView(view);
}

}
