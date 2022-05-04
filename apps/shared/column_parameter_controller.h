#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Shared {

class EditableCellTableViewController;

class ColumnParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  static constexpr int k_titleBufferSize = Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  ColumnParameterController(Escher::Responder * parentResponder) :
    SelectableListViewController(parentResponder),
    m_columnIndex(-1)
  {}
  static constexpr int k_maxSizeOfColumnName = 16; // this is an ad hoc value. Most of the time, colum_name are very short like "X1", "n" or "f(x)"
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  const char * title() override { return m_titleBuffer; };
  virtual void initializeColumnParameters(); // Always initialize parent class before initiliazing child.
protected:
  virtual EditableCellTableViewController * editableCellTableViewController() = 0;
  Escher::StackViewController * stackView();
  int m_columnIndex;
  char m_columnNameBuffer[k_maxSizeOfColumnName];
  char m_titleBuffer[k_titleBufferSize];

};

}

#endif
