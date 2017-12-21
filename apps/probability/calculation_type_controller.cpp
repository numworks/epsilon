#include "calculation_type_controller.h"
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

CalculationTypeController::CalculationTypeController(Responder * parentResponder, Law * law, Calculation * calculation, CalculationController * calculationController) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 0, 0, 0, 0, 0, this, nullptr, false, false),
  m_law(law),
  m_calculation(calculation),
  m_calculationController(calculationController)
{
  assert(m_calculation != nullptr);
}

View * CalculationTypeController::view() {
  return &m_selectableTableView;
}

void CalculationTypeController::viewWillAppear() {
  selectCellAtLocation(0, (int)m_calculation->type());
}


void CalculationTypeController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

void CalculationTypeController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_calculationController->setCalculationAccordingToIndex(selectedRow());
    m_calculationController->reload();
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Right) {
    if (event == Ion::Events::Right) {
      m_calculationController->selectCellAtLocation(1,0);
    }
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

int CalculationTypeController::numberOfRows() {
  if (m_law->isContinuous()) {
    return k_numberOfImages-1;
  }
  return k_numberOfImages;
}

KDCoordinate CalculationTypeController::cellWidth() {
  return ImageCell::k_width;
}

KDCoordinate CalculationTypeController::cellHeight() {
  return ImageCell::k_height;
}

HighlightCell * CalculationTypeController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_numberOfImages);
  return &m_imageCells[index];
}

int CalculationTypeController::reusableCellCount() {
  return k_numberOfImages;
}

void CalculationTypeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ImageCell * myCell = (ImageCell *)cell;
  const Image *  images[k_numberOfImages] = {ImageStore::Calcul1Icon, ImageStore::Calcul2Icon, ImageStore::Calcul3Icon, ImageStore::Calcul4Icon};
  const Image * focusedImages[k_numberOfImages] = {ImageStore::FocusedCalcul1Icon, ImageStore::FocusedCalcul2Icon, ImageStore::FocusedCalcul3Icon, ImageStore::FocusedCalcul4Icon};
  myCell->setImage(images[index], focusedImages[index]);
}

}
