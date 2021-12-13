#include "read_book_controller.h"

namespace Reader 
{

ReadBookController::ReadBookController(Responder * parentResponder) :
  ViewController(parentResponder)  
{
}

View * ReadBookController::view() {
  return &m_readerView;
}

void ReadBookController::setBook(const External::Archive::File& file, bool isRichTextFile) {
  m_file = &file;
  loadPosition();
  m_readerView.setText(reinterpret_cast<const char*>(file.data), file.dataLength, isRichTextFile);
}

bool ReadBookController::handleEvent(Ion::Events::Event event) {  
  if(event == Ion::Events::Down) {
    m_readerView.nextPage();
    return true;
  }
  if(event == Ion::Events::Up) {
    m_readerView.previousPage();
    return true;
  }
  return false;
}

void ReadBookController::viewDidDisappear() {
  savePosition();
}

void ReadBookController::savePosition() const {
  BookSave save = m_readerView.getBookSave();

  Ion::Storage::Record::ErrorStatus status = Ion::Storage::sharedStorage()->createRecordWithFullName(m_file->name, &save, sizeof(save));
  if(Ion::Storage::Record::ErrorStatus::NameTaken == status) {
    Ion::Storage::Record::Data data;
    data.buffer = &save;
    data.size = sizeof(save);
    status = Ion::Storage::sharedStorage()->recordNamed(m_file->name).setValue(data);
  }
}

void ReadBookController::loadPosition() {
  Ion::Storage::Record r = Ion::Storage::sharedStorage()->recordNamed(m_file->name);
  if(Ion::Storage::sharedStorage()->hasRecord(r)) {
    BookSave save = *(static_cast<const BookSave*>(r.value().buffer));
    m_readerView.setBookSave(save);
  }
  else {
    m_readerView.setBookSave({
      0,
      Palette::PrimaryText
    });
  }
}

}