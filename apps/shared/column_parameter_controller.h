#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include "column_helper.h"

namespace Shared {

class ColumnParameters {
public:
  ColumnParameters() : m_columnIndex(-1) {}
  // Max translated name of Message::ColumnOptions + max column name
  constexpr static int k_titleBufferSize = ClearColumnHelper::k_maxSizeOfColumnName + 23;
  // Always initialize parent class before initiliazing child.
  virtual void initializeColumnParameters();
protected:
  virtual ClearColumnHelper * clearColumnHelper() = 0;
  int m_columnIndex;
  char m_columnNameBuffer[ClearColumnHelper::k_maxSizeOfColumnName];
  char m_titleBuffer[k_titleBufferSize];
};

class ColumnParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>, public ColumnParameters {
public:
  ColumnParameterController(Escher::Responder * parentResponder) :
    SelectableListViewController(parentResponder),
    ColumnParameters()
  {}
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  const char * title() override { return m_titleBuffer; };
protected:
  Escher::StackViewController * stackView();

};

}

#endif
