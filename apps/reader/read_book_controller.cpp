#include "read_book_controller.h"

namespace reader 
{

ReadBookController::ReadBookController(Responder * parentResponder) :
  ViewController(parentResponder)  
{
}

View * ReadBookController::view() 
{
  return &m_readerView;
}

void ReadBookController::setBook(const External::Archive::File& file)
{
    m_file = &file;
    loadPosition();
    m_readerView.setText(reinterpret_cast<const char*>(file.data), file.dataLength);
}

bool ReadBookController::handleEvent(Ion::Events::Event event) 
{  
    if(event == Ion::Events::Down)
    {
        m_readerView.nextPage();
        return true;
    }
    if(event == Ion::Events::Up)
    {
        m_readerView.previousPage();
        return true;
    }
    if(event == Ion::Events::Back || event == Ion::Events::Home)
    {
      savePosition();
    }
    return false;
}

void ReadBookController::savePosition() const
{
  int pageOffset = m_readerView.getPageOffset(); 
  Ion::Storage::Record::ErrorStatus status = Ion::Storage::sharedStorage()->createRecordWithFullName(m_file->name, &pageOffset, sizeof(pageOffset));
  if(Ion::Storage::Record::ErrorStatus::NameTaken == status)
  {
      Ion::Storage::Record::Data data;
      data.buffer = &pageOffset;
      data.size = sizeof(pageOffset);
      status = Ion::Storage::sharedStorage()->recordNamed(m_file->name).setValue(data);
  }
}

void ReadBookController::loadPosition()
{
  Ion::Storage::Record r = Ion::Storage::sharedStorage()->recordNamed(m_file->name);
  if(Ion::Storage::sharedStorage()->hasRecord(r))
  {
    int pageOffset = *(static_cast<const int*>(r.value().buffer));
    m_readerView.setPageOffset(pageOffset);
  }
  else
  {
    m_readerView.setPageOffset(0);
  }
}

}