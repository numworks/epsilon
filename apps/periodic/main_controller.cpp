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

MainController::MainController(Escher::StackViewController * parentResponder) :
  ViewController(parentResponder),
  m_detailsController(parentResponder),
  m_displayTypeController(parentResponder),
  m_view(this)
{}

void MainController::selectedElementHasChanged() {
  m_view.bannerView()->reload();
  m_view.elementsView()->cursorMoved();
  m_detailsController.resetMemoization();
}

void MainController::activeDataFieldHasChanged() {
  m_view.bannerView()->reload();
}

bool MainController::handleEvent(Ion::Events::Event e) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  AtomicNumber z = dataSource->selectedElement();

  if (!ElementsDataBase::IsElement(z)) {
    if (e == Ion::Events::Up) {
      dataSource->setSelectedElement(dataSource->previousElement());
      return true;
    }
    if (e == Ion::Events::OK || e == Ion::Events::EXE) {
      stackOpenPage(&m_displayTypeController);
      return true;
    }
    return false;
  }

  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    stackOpenPage(&m_detailsController);
    return true;
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
  dataSource->setSelectedElement(newZ);
  return newZ != z;
}

void MainController::textFieldDidStartEditing(Escher::TextField * textField) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  dataSource->setTextFilter(m_view.bannerView()->textField()->text());
  /* Changing the selected element will reload the banner. */
  dataSource->setSelectedElement(ElementsDataBase::k_noElement);
}

bool MainController::textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool MainController::textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) {
  if (!m_view.bannerView()->textField()->isEditing() && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    /* OK should not start the edition */
    return handleEvent(event);
  }
  return false;
}

bool MainController::textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  dataSource->setSelectedElement(dataSource->firstMatchingElement());
  dataSource->setTextFilter(nullptr);
  return true;
}

bool MainController::textFieldDidAbortEditing(Escher::TextField * textField) {
  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  dataSource->setTextFilter(nullptr);
  dataSource->setSelectedElement(dataSource->previousElement());
  return false;
}

bool MainController::textFieldDidHandleEvent(Escher::TextField * textField, bool returnValue, bool textSizeDidChange) {
  if (textSizeDidChange) {
    m_view.elementsView()->reload();
  }
  return returnValue;
}

}
