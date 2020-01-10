#include "main_controller.h"
#include <apps/i18n.h>
#include <apps/apps_container.h>
#include <assert.h>
#include "archive.h"
#include "app.h"
#include <kandinsky/palette.h>
#include <ion/include/ion.h>

using namespace Poincare;

namespace External {

using namespace Archive;

MainController::MainController(Responder * parentResponder, ::App * app) :
  ViewController(parentResponder),
  m_selectableTableView(this)
{
  m_app = app;
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && ((selectedRow() < k_numberOfCells - 2) && numberOfFiles() > 0)) {
      uint32_t res = executeFile(m_cells[selectedRow()].text(), ((App *)m_app)->heap(), ((App *)m_app)->heapSize());
      ((App*)m_app)->redraw();
      switch(res) {
        case 0:
          break;
        case 1:
          Container::activeApp()->displayWarning(I18n::Message::ExternalAppApiMismatch);
          break;
        case 2:
          Container::activeApp()->displayWarning(I18n::Message::StorageMemoryFull1);
          break;
        default:
          Container::activeApp()->displayWarning(I18n::Message::ExternalAppExecError);
          break;
      }
      return true;
  }
  return false;
}

int MainController::numberOfRows() const {
  return k_numberOfCells;
};

KDCoordinate MainController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY/rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  assert(index < k_numberOfCells);
  return &m_cells[index];
}

int MainController::reusableCellCount(int type) {
  return k_numberOfCells;
}

int MainController::typeAtLocation(int i, int j) {
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  PointerTextTableCell * myTextCell = (PointerTextTableCell *)cell;
  struct File f;
  #ifdef DEVICE
  if(Ion::fccId() == "2ALWP-N0100"){
    if(index == 0){
      myTextCell->setText("External is not compatible");
      myTextCell->setTextColor(Palette::Red);
      myTextCell->setHighlighted(myTextCell->isHighlighted());
    } else {
      myTextCell->setText("with n0100");
      myTextCell->setTextColor(Palette::Red);
      myTextCell->setHighlighted(myTextCell->isHighlighted());
    }
  }else{
  if(index == k_numberOfCells-1){
    myTextCell->setText("zardam.github.io/nw-external-apps/");
    myTextCell->setTextColor(Palette::AccentText);
    myTextCell->setHighlighted(myTextCell->isHighlighted());
    return;
  }
  if(index == k_numberOfCells-2){
    myTextCell->setText("Get more apps at");
    myTextCell->setTextColor(Palette::AccentText);
    myTextCell->setHighlighted(myTextCell->isHighlighted());
    return;
  }
  if(index == 0 && numberOfFiles() == 0){
    myTextCell->setText("No apps installed");
    myTextCell->setTextColor(Palette::Red);
    myTextCell->setHighlighted(myTextCell->isHighlighted());
  }
  if(numberOfFiles() > 0){
    if(fileAtIndex(index, f)) {
      myTextCell->setText(f.name);
      myTextCell->setTextColor(f.isExecutable ? Palette::PrimaryText : Palette::Palette::SecondaryText);
      myTextCell->setHighlighted(myTextCell->isHighlighted());
    }
  }
  }
  #else
  if(index == 0){
    myTextCell->setText("External is not compatible");
    myTextCell->setTextColor(Palette::Red);
    myTextCell->setHighlighted(myTextCell->isHighlighted());
  } else {
    myTextCell->setText("with the simulator");
    myTextCell->setTextColor(Palette::Red);
    myTextCell->setHighlighted(myTextCell->isHighlighted());
  }
  #endif
}

void MainController::viewWillAppear() {
  int count;
  #ifdef DEVICE
  if(Ion::fccId() == "2ALWP-N0100"){
    count = 2;
  }else {
    if(numberOfFiles() > 0){
      count = numberOfFiles()+2;
    } else {
      count = 3;
    }
  }

  #else
    count = 2;
  #endif
  k_numberOfCells = count <= k_maxNumberOfCells ? count : k_maxNumberOfCells;
  m_selectableTableView.reloadData();
}

}
