#include "list_book_controller.h"
#include "utility.h"
namespace reader
{

View* ListBookController::view()
{
    return &m_tableView;
}

ListBookController::ListBookController(Responder * parentResponder):
    ViewController(parentResponder),
    m_tableView(this, this)
{
    size_t nbTotalFiles = External::Archive::numberOfFiles();

    for(size_t i=0; i < nbTotalFiles; ++i)
    {
        External::Archive::File file;
        External::Archive::fileAtIndex(i, file);
        if(stringEndsWith(file.name, ".txt"))
        {
            m_files[m_nbFiles] = file;
            m_nbFiles++;
        }
    }
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

}