#ifndef __LIST_BOOK_CONTROLLER_H__
#define __LIST_BOOK_CONTROLLER_H__

#include <escher.h>
#include <apps/external/archive.h>

#include "read_book_controller.h"

namespace reader
{

class ListBookController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource
{
public:
    ListBookController(Responder * parentResponder);
    View* view() override;

    int numberOfRows() const override;
    KDCoordinate cellHeight() override;
    HighlightCell * reusableCell(int index) override;
    int reusableCellCount() const override;
    void willDisplayCellForIndex(HighlightCell * cell, int index) override;
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event event) override;
    bool hasBook(const char* filename) const;
    void cleanRemovedBookRecord();
private:
    SelectableTableView m_tableView;

    static const int NB_FILES = 20;
    External::Archive::File m_files[NB_FILES];
    int m_nbFiles = 0;

    static const int NB_CELLS = 6;
    MessageTableCell m_cells[NB_CELLS];

    ReadBookController m_readBookController;
};

}

#endif