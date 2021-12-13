#ifndef __LIST_BOOK_CONTROLLER_H__
#define __LIST_BOOK_CONTROLLER_H__

#include <escher.h>
#include <apps/external/archive.h>

#include "read_book_controller.h"

namespace Reader
{

class ListBookController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource, public AlternateEmptyViewDefaultDelegate
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
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;
private:
  SelectableTableView m_tableView;
  static const int k_maxFilesNumber = 20;
  External::Archive::File m_files[k_maxFilesNumber];
  int m_txtFilesNumber;
  int m_urtFilesNumber;
  static const int k_cellsNumber = 6;
  MessageTableCellWithChevron m_cells[k_cellsNumber];
  ReadBookController m_readBookController;
};

}

#endif