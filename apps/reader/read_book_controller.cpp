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
    return false;
}

}