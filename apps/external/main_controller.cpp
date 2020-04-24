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
  myTextCell->setHighlighted(myTextCell->isHighlighted());
  struct File f;
  #if defined(DEVICE) || defined(EXTERNAL_BUILTIN)
    #if defined(DEVICE_N0100) && !defined(EXTERNAL_BUILTIN)
      if(index == 0){
        myTextCell->setText(I18n::translate(I18n::Message::ExternalNotCompatible));
        myTextCell->setTextColor(Palette::Red);
      } else {
        myTextCell->setText(I18n::translate(I18n::Message::WithN0100));
        myTextCell->setTextColor(Palette::Red);
      }
    #else
      if(index == k_numberOfCells-1){
        myTextCell->setText(I18n::translate(I18n::Message::URL));
        myTextCell->setTextColor(Palette::AccentText);
        return;
      }
      if(index == k_numberOfCells-2){
        myTextCell->setText(I18n::translate(I18n::Message::GetMoreAppsAt));
        myTextCell->setTextColor(Palette::AccentText);
        return;
      }
      if(index == 0 && numberOfFiles() == 0){
        myTextCell->setText(I18n::translate(I18n::Message::NoAppsInstalled));
        myTextCell->setTextColor(Palette::Red);
      }
      if(numberOfFiles() > 0){
        if(fileAtIndex(index, f)) {
          myTextCell->setText(f.name);
          myTextCell->setTextColor(f.isExecutable ? Palette::PrimaryText : Palette::Palette::SecondaryText);
        }
      }
    #endif
  #else
  if(index == 0){
    myTextCell->setText(I18n::translate(I18n::Message::ExternalNotCompatible));
    myTextCell->setTextColor(Palette::Red);
  } else {
    myTextCell->setText(I18n::translate(I18n::Message::WithSimulator));
    myTextCell->setTextColor(Palette::Red);
  }
  #endif
}

void MainController::viewWillAppear() {
  int count;
  #if defined(DEVICE) || defined(EXTERNAL_BUILTIN)
    #if defined(DEVICE_N0100) || !defined(EXTERNAL_BUILTIN)
    count = 2;
    #else
      if(numberOfFiles() > 0){
        count = numberOfFiles()+2;
      } else {
        count = 3;
      }
    #endif

  #else
    count = 2;
  #endif
  k_numberOfCells = count <= k_maxNumberOfCells ? count : k_maxNumberOfCells;
  m_selectableTableView.reloadData();
}

}
