#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "column_helper.h"

namespace Shared {

class ColumnParameters {
 public:
  ColumnParameters() : m_column(-1) {}
  // Max translated name of Message::ColumnOptions + max column name
  constexpr static int k_titleBufferSize =
      ClearColumnHelper::k_maxSizeOfColumnName + 23;
  // Always initialize parent class before initiliazing child.
  virtual void initializeColumnParameters();

 protected:
  virtual ClearColumnHelper* clearColumnHelper() = 0;
  int m_column;
  char m_columnNameBuffer[ClearColumnHelper::k_maxSizeOfColumnName];
  char m_titleBuffer[k_titleBufferSize];
};

class ColumnParameterController
    : public Escher::ExplicitSelectableListViewController,
      public ColumnParameters {
 public:
  ColumnParameterController(Escher::Responder* parentResponder)
      : Escher::ExplicitSelectableListViewController(parentResponder),
        ColumnParameters() {}
  void viewWillAppear() override;
  const char* title() const override { return m_titleBuffer; }

 protected:
  Escher::StackViewController* stackView();
};

}  // namespace Shared

#endif
