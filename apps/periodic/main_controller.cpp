#include "main_controller.h"
#include "app.h"
#include "table_layout.h"

namespace Periodic {

// MainController::ContentView

Escher::View * MainController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_elementsView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void MainController::ContentView::layoutSubviews(bool force) {
  KDSize bannerSize = m_bannerView.minimalSizeForOptimalDisplay();
  m_bannerView.setFrame(KDRect(0, bounds().height() - bannerSize.height(), bounds().width(), bannerSize.height()), force);
  m_elementsView.setFrame(KDRect(0, 0, bounds().width(), bounds().height() - bannerSize.height()), force);
}

// MainController

void MainController::selectedElementHasChanged(AtomicNumber oldZ) {
  m_view.bannerView()->reload();
  m_view.elementsView()->cursorMoved(oldZ);
}

bool MainController::handleEvent(Ion::Events::Event e) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  AtomicNumber z = dataSource->selectedElement();
  if (!ElementsDataBase::IsElement(z)) {
    if (e == Ion::Events::Up) {
      dataSource->setSelectedElement(m_previousElement);
      return true;
    }
    if (e == Ion::Events::OK || e == Ion::Events::EXE) {
      stackOpenPage(&m_coloringController);
      return true;
    }
    return false;
  }
  AtomicNumber newZ = z;
  if (e == Ion::Events::Up) {
    newZ = TableLayout::NextElement(z, TableLayout::Direction::DecreasingRow);
  } else if (e == Ion::Events::Down) {
    newZ = TableLayout::NextElement(z, TableLayout::Direction::IncreasingRow);
  } else if (e == Ion::Events::Left) {
    newZ = TableLayout::NextElement(z, TableLayout::Direction::DecreasingZ);
  } else if (e == Ion::Events::Right) {
    newZ = TableLayout::NextElement(z, TableLayout::Direction::IncreasingZ);
  }
  m_previousElement = z;
  dataSource->setSelectedElement(newZ);
  return newZ != z;
}

}
