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

ImageTableView::ImageTableView(Responder * parentResponder, Law * law) :
  View(),
  Responder(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, 0, 0, 0, 0, nullptr, false, false)),
  m_isSelected(false),
  m_law(law)
{
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
    m_law->setCalculationType((Law::CalculationType)m_selectableTableView.selectedRow());
    select(false);
    m_selectableTableView.reloadData();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  return false;
}

void ImageTableView::select(bool select) {
  if (!select) {
    m_selectableTableView.deselectTable();
    m_isSelected = select;
  } else {
    m_isSelected = select;
    m_selectableTableView.selectCellAtLocation(0, m_law->calculationType());
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
  if (!m_isSelected) {
    switch (m_law->calculationType()) {
      case 0:
        myCell->setImage(ImageStore::Calcul1Icon, ImageStore::FocusedCalcul1Icon);
        break;
      case 1:
        myCell->setImage(ImageStore::Calcul2Icon, ImageStore::FocusedCalcul2Icon);
        break;
      case 2:
        myCell->setImage(ImageStore::Calcul3Icon, ImageStore::FocusedCalcul3Icon);
        break;
      default:
        break;
    }
  } else {
    switch (index) {
      case 0:
        myCell->setImage(ImageStore::Calcul1Icon, ImageStore::FocusedCalcul1Icon);
        break;
      case 1:
        myCell->setImage(ImageStore::Calcul2Icon, ImageStore::FocusedCalcul2Icon);
        break;
      case 2:
        myCell->setImage(ImageStore::Calcul3Icon, ImageStore::FocusedCalcul3Icon);
        break;
      default:
        break;
    }
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
