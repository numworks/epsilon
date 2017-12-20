#include "image_table_view.h"
#include <assert.h>
#include "app.h"
#include "images/calcul1_icon.h"
#include "images/calcul2_icon.h"
#include "images/calcul3_icon.h"
#include "images/calcul4_icon.h"
#include "images/focused_calcul1_icon.h"
#include "images/focused_calcul2_icon.h"
#include "images/focused_calcul3_icon.h"
#include "images/focused_calcul4_icon.h"

namespace Probability {

ImageCell::ImageCell() :
  HighlightCell(),
  m_icon(nullptr),
  m_focusedIcon(nullptr)
{
}

int ImageCell::numberOfSubviews() const {
  return 1;
}

View * ImageCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_iconView;
}

void ImageCell::layoutSubviews() {
  m_iconView.setFrame(bounds());
}

void ImageCell::reloadCell() {
  HighlightCell::reloadCell();
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void ImageCell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
}

ImageTableView::ImageTableView(Responder * parentResponder, Law * law, Calculation * calculation, CalculationController * calculationController) :
  View(),
  Responder(parentResponder),
  m_selectableTableView(this, this, 0, 0, 0, 0, 0, 0, this, nullptr, false, false),
  m_isSelected(false),
  m_law(law),
  m_calculation(calculation),
  m_calculationController(calculationController)
{
  assert(m_calculation != nullptr);
}

void ImageTableView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->strokeRect(KDRect(k_margin, k_margin, ImageCell::k_width+2*k_outline, ImageCell::k_height+2*k_outline), Palette::GreyMiddle);
}

KDSize ImageTableView::minimalSizeForOptimalDisplay() const {
  return KDSize(2*k_totalMargin+ImageCell::k_width, k_totalMargin+k_numberOfImages*ImageCell::k_height);
}

void ImageTableView::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  app()->setFirstResponder(&m_selectableTableView);
}

void ImageTableView::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, 0);
}

void ImageTableView::willExitResponderChain(Responder * nextFirstResponder) {
  m_selectableTableView.deselectTable();
}

bool ImageTableView::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Down) && !m_isSelected) {
    select(true);
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && m_isSelected) {
    m_calculationController->setCalculationAccordingToIndex(selectedRow());
    select(false);
    return true;
  }
  if (event == Ion::Events::Back) {
    select(false);
    return true;
  }
  return false;
}

void ImageTableView::select(bool select) {
  if (!select) {
    m_selectableTableView.deselectTable();
    m_isSelected = select;
    m_selectableTableView.reloadData();
    /* The dropdown menu is drawn on the law curve view, so when we deselect
     * the dropdown menu, we need to redraw the law curve view */
    m_calculationController->reload();
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_isSelected = select;
    m_selectableTableView.reloadData();
    m_selectableTableView.selectCellAtLocation(0, (int)m_calculation->type());
  }
}

int ImageTableView::numberOfRows() {
  if (m_isSelected) {
    if (m_law->isContinuous()) {
      return k_numberOfImages-1;
    }
    return k_numberOfImages;
  }
  return 1;
}

KDCoordinate ImageTableView::cellHeight() {
  return ImageCell::k_height;
}

HighlightCell * ImageTableView::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_numberOfImages);
  return &m_imageCells[index];
}

int ImageTableView::reusableCellCount() {
  return k_numberOfImages;
}

void ImageTableView::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ImageCell * myCell = (ImageCell *)cell;
  const Image *  images[k_numberOfImages] = {ImageStore::Calcul1Icon, ImageStore::Calcul2Icon, ImageStore::Calcul3Icon, ImageStore::Calcul4Icon};
  const Image * focusedImages[k_numberOfImages] = {ImageStore::FocusedCalcul1Icon, ImageStore::FocusedCalcul2Icon, ImageStore::FocusedCalcul3Icon, ImageStore::FocusedCalcul4Icon};
  if (!m_isSelected) {
    myCell->setImage(images[(int)m_calculation->type()], focusedImages[(int)m_calculation->type()]);
  } else {
    myCell->setImage(images[index], focusedImages[index]);
  }
  myCell->reloadCell();
}

int ImageTableView::numberOfSubviews() const {
  return 1;
}

View * ImageTableView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_selectableTableView;
}

void ImageTableView::layoutSubviews() {
  m_selectableTableView.setFrame(KDRect(k_totalMargin, k_totalMargin, bounds().width()-2*k_totalMargin, bounds().height()-k_totalMargin));
}

}
