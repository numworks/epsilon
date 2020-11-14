#ifndef __LIST_BOOK_CONTROLLER_H__
#define __LIST_BOOK_CONTROLLER_H__

#include <escher.h>

namespace reader
{

class ListBookController : public ViewController, public SimpleListViewDataSource, public ScrollViewDataSource
{
public:
    ListBookController(Responder * parentResponder);
    View* view() override;

    int numberOfRows() const override;
    KDCoordinate cellHeight() override;
    HighlightCell * reusableCell(int index) override;
    int reusableCellCount() const override;
private:
    TableView m_tableView;
};

}

#endif