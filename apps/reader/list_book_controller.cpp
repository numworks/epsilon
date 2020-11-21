#include "list_book_controller.h"
#include "utility.h"
#include "apps/i18n.h"

namespace reader
{

View* ListBookController::view()
{
    return &m_tableView;
}

ListBookController::ListBookController(Responder * parentResponder):
    ViewController(parentResponder),
    m_tableView(this, this, this),
    m_readBookController(this)
{
    m_nbFiles = filesWithExtension(".txt", m_files, NB_FILES);
}

int ListBookController::numberOfRows() const
{
    return m_nbFiles;
}

KDCoordinate ListBookController::cellHeight()
{
    return 50;
}

HighlightCell * ListBookController::reusableCell(int index)
{
    return &m_cells[index];
}
    
int ListBookController::reusableCellCount() const
{
    return NB_CELLS;
}

void ListBookController::willDisplayCellForIndex(HighlightCell * cell, int index)
{
    MessageTableCell* myTextCell = static_cast<MessageTableCell*>(cell);    
    MessageTextView* textView = static_cast<MessageTextView*>(myTextCell->labelView());
    textView->setText(m_files[index].name);
    myTextCell->setMessageFont(KDFont::LargeFont);
}

void ListBookController::didBecomeFirstResponder()
{
    if (selectedRow() < 0) {
        selectCellAtLocation(0, 0);
    }
    Container::activeApp()->setFirstResponder(&m_tableView);
    if(m_nbFiles == 0)
    {
        Container::activeApp()->displayWarning(I18n::Message::NoFileToDisplay);
    }
}

bool ListBookController::handleEvent(Ion::Events::Event event)
{
    if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right)
    {
        
        m_readBookController.setBook(m_files[selectedRow()]);
        static_cast<StackViewController*>(parentResponder())->push(&m_readBookController);
        Container::activeApp()->setFirstResponder(&m_readBookController);
        return true;
    }

    return false;
}

}