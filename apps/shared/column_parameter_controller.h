#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include "column_helper.h"

namespace Shared {

class ColumnParameters {
public:
  ColumnParameters() : m_columnIndex(-1) {}
  constexpr static int k_titleBufferSize = Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  virtual void initializeColumnParameters(); // Always initialize parent class before initiliazing child.
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
