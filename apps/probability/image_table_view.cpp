#include "image_table_view.h"
#include <assert.h>
#include "app.h"
#include "images/calcul1_icon.h"
#include "images/calcul2_icon.h"
#include "images/calcul3_icon.h"
#include "images/focused_calcul1_icon.h"
#include "images/focused_calcul2_icon.h"
#include "images/focused_calcul3_icon.h"

namespace Probability {

ImageTableView::ImageCell::ImageCell() :
  TableViewCell()
{
}

int ImageTableView::ImageCell::numberOfSubviews() const {
  return 1;
}

View * ImageTableView::ImageCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_iconView;
}

void ImageTableView::ImageCell::layoutSubviews() {
  m_iconView.setFrame(bounds());
}

void ImageTableView::ImageCell::reloadCell() {
  TableViewCell::reloadCell();
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void ImageTableView::ImageCell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
}

ImageTableView::ImageTableView(Responder * parentResponder, Calculation * calculation, CalculationController * calculationController) :
  View(),
  Responder(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, 0, 0, 0, 0, nullptr, false, false)),
  m_isSelected(false),
  m_selectedIcon(0),
  m_calculation(calculation),
  m_calculationController(calculationController)
{
}

void ImageTableView::setCalculation(Calculation * calculation, int index) {
  m_calculation = calculation;
  m_selectedIcon = index;
}

void ImageTableView::didBecomeFirstResponder() {
  m_isSelected = true;
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool ImageTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_calculationController->setCalculationAccordingToIndex(m_selectableTableView.selectedRow());
    select(false);
    setHighlight(true);
    m_selectableTableView.reloadData();
    m_calculationController->reload();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  return false;
}

void ImageTableView::select(bool select) {
  if (!select) {
    m_selectableTableView.deselectTable();
    m_isSelected = select;
    willDisplayCellForIndex(m_selectableTableView.cellAtLocation(0,0), 0);
  } else {
    m_isSelected = select;
    m_selectableTableView.selectCellAtLocation(0, m_selectedIcon);
  }
}

void ImageTableView::setHighlight(bool highlight) {
  if (highlight) {
    m_selectableTableView.selectCellAtLocation(0,0);
  } else {
    m_selectableTableView.deselectTable();
  }
}

int ImageTableView::numberOfRows() {
  if (m_isSelected) {
    return k_numberOfImages;
  }
  return 1;
}

TableViewCell * ImageTableView::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_numberOfImages);
  return &m_imageCells[index];
}

int ImageTableView::reusableCellCount() {
  return k_numberOfImages;
}

void ImageTableView::willDisplayCellForIndex(TableViewCell * cell, int index) {
  ImageCell * myCell = (ImageCell *)cell;
  const Image *  images[3] = {ImageStore::Calcul1Icon, ImageStore::Calcul2Icon, ImageStore::Calcul3Icon};
  const Image * focusedImages[3] = {ImageStore::FocusedCalcul1Icon, ImageStore::FocusedCalcul2Icon, ImageStore::FocusedCalcul3Icon};
  if (!m_isSelected) {
    myCell->setImage(images[m_selectedIcon], focusedImages[m_selectedIcon]);
  } else {
    myCell->setImage(images[index], focusedImages[index]);
  }
  myCell->reloadCell();
}

KDCoordinate ImageTableView::cellHeight() {
  return k_imageHeight;
}

int ImageTableView::numberOfSubviews() const {
  return 1;
}

View * ImageTableView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_selectableTableView;
}

void ImageTableView::layoutSubviews() {
  m_selectableTableView.setFrame(bounds());
}

}
