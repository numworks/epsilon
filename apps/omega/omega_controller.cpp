#include "omega_controller.h"
#include <assert.h>
#include "apps/i18n.h"

namespace Omega {

OmegaController::OmegaController(Responder * parentResponder) :
  ViewController(parentResponder),
  SelectableTableViewDataSource(),
  m_omegaView(&m_selectableTableView),
  m_selectableTableView(this),
  m_contributorsCell(),
  m_versionCell(),
  m_contributorsController(this)
{
  m_contributorsCell.setMessageFont(KDFont::LargeFont);
  m_versionCell.setMessageFont(KDFont::LargeFont);
  m_versionCell.setAccessoryFont(KDFont::SmallFont);
  m_versionCell.setAccessoryText(Ion::omegaVersion());
}

View * OmegaController::view() {
  return &m_omegaView;
}

void OmegaController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool OmegaController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    //StackViewController * stack = (StackViewController *)parentResponder();
    stackController()->push(&m_contributorsController, Palette::BannerFirstText, Palette::BannerFirstBackground, Palette::BannerFirstBorder);
    return true;
  }
  return false;
}

int OmegaController::numberOfRows() const {
  return 2;
}

KDCoordinate OmegaController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

HighlightCell * OmegaController::reusableCell(int index, int type) {
  assert(index >= 0 && index <= 1);
  switch (index)
  {
    case 1:
      return &m_versionCell;
    default:
      return &m_contributorsCell;
  }
}

int OmegaController::reusableCellCount(int type) {
  return 2;
}

int OmegaController::typeAtLocation(int i, int j) {
  return 0;
}

void OmegaController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[2] = {I18n::Message::OmegaContributors, I18n::Message::OmegaVersion};
  myCell->setMessage(titles[index]);
}

StackViewController * OmegaController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
