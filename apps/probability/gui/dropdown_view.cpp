#include "dropdown_view.h"

#include <assert.h>
#include <escher/container.h>
#include <escher/palette.h>

#include <cmath>

#include "../images/caret.h"

namespace Probability {

PopupView::PopupView(Escher::HighlightCell * cell) : m_cell(cell) {
  m_caret.setImage(ImageStore::Caret);
}

void PopupView::setHighlighted(bool highlighted) {
  m_cell->setHighlighted(highlighted);
  m_caret.setHighlighted(highlighted);
  Escher::HighlightCell::setHighlighted(highlighted);
}

KDSize PopupView::minimalSizeForOptimalDisplay() const {
  KDSize cellSize = m_cell->minimalSizeForOptimalDisplay();
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  return KDSize(
      cellSize.width() + caretSize.width() + 2 * k_marginImageHorizontal + k_marginCaretRight,
      fmaxf(cellSize.height(), caretSize.height()) + 2 * k_marginImageVertical);
}

void PopupView::layoutSubviews(bool force) {
  KDSize cellSize = m_cell->minimalSizeForOptimalDisplay();
  KDSize caretSize = m_caret.minimalSizeForOptimalDisplay();
  m_cell->setFrame(KDRect(KDPoint(k_marginImageHorizontal, k_marginImageVertical), cellSize),
                   force);
  KDCoordinate yCaret = (cellSize.height() - caretSize.height()) / 2 + k_marginImageVertical;
  m_caret.setFrame(
      KDRect(KDPoint(2 * k_marginImageHorizontal + cellSize.width(), yCaret), caretSize),
      force);
}

int PopupView::numberOfSubviews() const {
  return 2 - m_isPoppingUp;
}

Escher::View * PopupView::subviewAtIndex(int i) {
  if (!m_isPoppingUp && i == 1) {
    return &m_caret;
  }
  return m_cell;
}

void PopupView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Escher::Palette::Select : KDColorWhite;
  drawInnerRect(ctx, bounds(), backColor);
  KDColor borderColor = m_isPoppingUp ? backColor : Escher::Palette::GrayBright;
  drawBorderOfRect(ctx, bounds(), borderColor);
}

PopupListViewDataSource::PopupListViewDataSource(Escher::ListViewDataSource * listViewDataSource) :
    m_listViewDataSource(listViewDataSource) {
}

KDCoordinate PopupListViewDataSource::cellWidth() {
  // TODO memoize
  Escher::HighlightCell * cell = reusableCell(0, 0);
  willDisplayCellForIndex(cell, 0);
  return cell->minimalSizeForOptimalDisplay().width();
}

KDCoordinate PopupListViewDataSource::rowHeight(int j) {
  // TODO memoize
  Escher::HighlightCell * cell = reusableCell(0, 0);
  willDisplayCellForIndex(cell, 0);
  return cell->minimalSizeForOptimalDisplay().height();
}

Escher::HighlightCell * PopupListViewDataSource::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfRows());
  return &m_popupViews[index];
}

void PopupListViewDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  PopupView * popupView = static_cast<PopupView *>(cell);
  popupView->setInnerCell(m_listViewDataSource->reusableCell(index, 0));  // TODO hoping no type?
  popupView->setHighlighted(popupView->isHighlighted());
  // TODO remove borders and caret
  popupView->setPopping(true);
  m_listViewDataSource->willDisplayCellForIndex(popupView->innerCell(), index);
}

DropdownPopup::DropdownPopup(Escher::Responder * parentResponder,
                             Escher::ListViewDataSource * listDataSource) :
    ViewController(parentResponder),
    m_listDataSource(listDataSource),
    m_selectableTableView(this, &m_listDataSource, &m_selectionDataSource),
    m_borderingView(&m_selectableTableView) {
  m_selectableTableView.setMargins(0);
}

void DropdownPopup::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool DropdownPopup::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    Escher::Container::activeApp()->dismissModalViewController();
    if (m_callback) {
      m_callback->onDropdownSelected(m_selectionDataSource.selectedRow());
    }
    return true;
  }
  return false;
}

Dropdown::Dropdown(Escher::Responder * parentResponder,
                   Escher::ListViewDataSource * listDataSource,
                   DropdownCallback * callback) :
    Responder(parentResponder), m_popup(this, listDataSource) {
  registerCallback(callback);
}

bool Dropdown::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    KDPoint topLeftAngle = Escher::Container::activeApp()->modalView()->pointFromPointInView(
        this,
        KDPointZero);
    Escher::Container::activeApp()->displayModalViewController(&m_popup,
                                                               0.f,
                                                               0.f,
                                                               topLeftAngle.y(),
                                                               topLeftAngle.x());
    return true;
  }
  return false;
}

void Dropdown::setHighlighted(bool highlighted) {
  PopupView::setHighlighted(highlighted);
  if (highlighted) Escher::Container::activeApp()->setFirstResponder(this);
}

}  // namespace Probability
