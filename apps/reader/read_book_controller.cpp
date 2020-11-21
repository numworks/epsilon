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
    m_readerView.setText(reinterpret_cast<const char*>(file.data));
}

}