#include "controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace Home {

Controller::Controller(Responder * parentResponder, ::AppsContainer * container) :
  ViewController(parentResponder),
  m_container(container),
  m_tableView(TableView(this)),
  m_activeIndex(0)
{
}

bool Controller::handleEvent(Ion::Events::Event event) {
  int nextActiveIndex = 0;
  switch (event) {
    case Ion::Events::Event::ENTER:
      m_container->switchTo(m_container->appAtIndex(m_activeIndex+1));
      return true;
    case Ion::Events::Event::DOWN_ARROW:
      nextActiveIndex = m_activeIndex+k_numberOfColumns;
      break;
    case Ion::Events::Event::UP_ARROW:
      nextActiveIndex = m_activeIndex-k_numberOfColumns;
      break;
    case Ion::Events::Event::LEFT_ARROW:
      nextActiveIndex = m_activeIndex-1;
      break;
    case Ion::Events::Event::RIGHT_ARROW:
      nextActiveIndex = m_activeIndex+1;
      break;
    default:
      return false;
  }
  if (nextActiveIndex < 0) {
    nextActiveIndex = 0;
  }
  if (nextActiveIndex >= numberOfIcons()) {
    nextActiveIndex = numberOfIcons()-1;
  }
  setActiveIndex(nextActiveIndex);
  return true;
}

void Controller::didBecomeFirstResponder() {
  setActiveIndex(m_activeIndex);
}

View * Controller::view() {
  return &m_tableView;
}

int Controller::numberOfRows() {
  return ((numberOfIcons()-1)/k_numberOfColumns)+1;
}

int Controller::numberOfColumns() {
  return k_numberOfColumns;
}

KDCoordinate Controller::cellHeight() {
  return 50;
}

KDCoordinate Controller::cellWidth() {
  return 50;
}

TableViewCell * Controller::reusableCell(int index) {
  return &m_cells[index];
}

int Controller::reusableCellCount() {
  return k_maxNumberOfCells;
}

void Controller::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  AppCell * appCell = (AppCell *)cell;
  int appIndex = (j*k_numberOfColumns+i)+1;
  if (appIndex >= m_container->numberOfApps()) {
    appCell->setVisible(false);
  } else {
    appCell->setVisible(true);
    ::App * app = m_container->appAtIndex((j*k_numberOfColumns+i)+1);
    appCell->setApp(app);
  }
}

int Controller::numberOfIcons() {
  assert(m_container->numberOfApps() > 0);
  return m_container->numberOfApps() - 1;
}

void Controller::setActiveIndex(int index) {
  if (m_activeIndex < 0 && m_activeIndex >= numberOfIcons()) {
    return;
  }

  int j = m_activeIndex/k_numberOfColumns;
  int i = m_activeIndex-j*k_numberOfColumns; // Avoid modulo
  AppCell * previousCell = (AppCell *)m_tableView.cellAtLocation(i, j);
  previousCell->setHighlighted(false);

  m_activeIndex = index;

  j = m_activeIndex/k_numberOfColumns;
  i = m_activeIndex-j*k_numberOfColumns; // Avoid modulo
  AppCell * nextCell = (AppCell *)m_tableView.cellAtLocation(i, j);
  nextCell->setHighlighted(true);

}

}
