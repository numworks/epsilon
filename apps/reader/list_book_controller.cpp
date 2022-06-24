#include "list_book_controller.h"
#include "utility.h"
#include <escher/metric.h>
#include "apps/i18n.h"

namespace Reader
{

View* ListBookController::view() {
  return &m_tableView;
}

ListBookController::ListBookController(Responder * parentResponder):
  ViewController(parentResponder),
  m_tableView(this, this, this),
  m_readBookController(this)
{
  m_txtFilesNumber = filesWithExtension(".txt", m_files, k_maxFilesNumber);
  m_urtFilesNumber = filesWithExtension(".urt", m_files + m_txtFilesNumber, k_maxFilesNumber - m_txtFilesNumber);
  cleanRemovedBookRecord();
}

int ListBookController::numberOfRows() const {
  return m_txtFilesNumber + m_urtFilesNumber;
}

KDCoordinate ListBookController::cellHeight() {
  return Metric::StoreRowHeight;
}

HighlightCell * ListBookController::reusableCell(int index) {
  return &m_cells[index];
}

int ListBookController::reusableCellCount() const {
  return k_cellsNumber;
}

void ListBookController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell<> * myTextCell = static_cast<MessageTableCell<> *>(cell);
  MessageTextView* textView = static_cast<MessageTextView*>(myTextCell->labelView());
  textView->setText(m_files[index].name);
  myTextCell->setMessageFont(KDFont::LargeFont); //TODO set cell font at building ?
}

void ListBookController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_tableView);
}

bool ListBookController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right)
  {
    m_readBookController.setBook(m_files[selectedRow()], selectedRow() >= m_txtFilesNumber);
    static_cast<StackViewController*>(parentResponder())->push(&m_readBookController);
    Container::activeApp()->setFirstResponder(&m_readBookController);
    return true;
  }

  return false;
}


bool ListBookController::hasBook(const char* filename) const {
  for(int i=0;i<m_txtFilesNumber + m_urtFilesNumber;i++)
  {
    if(strcmp(m_files[i].name, filename) == 0) {
      return true;
    }
  }
  return false;
}

void ListBookController::cleanRemovedBookRecord() {
  int nb = Ion::Storage::sharedStorage()->numberOfRecordsWithExtension("txt");
  for(int i=0; i<nb; i++) {
    Ion::Storage::Record r = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex("txt", i);
    if(!hasBook(r.fullName())) {
      r.destroy();
    }
  }

  nb = Ion::Storage::sharedStorage()->numberOfRecordsWithExtension("urt");
  for(int i=0; i<nb; i++) {
    Ion::Storage::Record r = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex("urt", i);
    if(!hasBook(r.fullName())) {
      r.destroy();
    }
  }
}

bool ListBookController::isEmpty() const {
  return m_txtFilesNumber + m_urtFilesNumber == 0;
}

I18n::Message ListBookController::emptyMessage() {
  return I18n::Message::NoFileToDisplay;
}

Responder * ListBookController::defaultController() {
  return parentResponder();
}

}