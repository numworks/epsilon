#ifndef SHARED_STORE_MENU_CONTROLLER_H
#define SHARED_STORE_MENU_CONTROLLER_H

#include <escher/pervasive_box.h>
#include <escher/stack_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/editable_expression_cell.h>
#include "text_field_delegate.h"
#include "layout_field_delegate.h"
#include <apps/i18n.h>
#include "pop_up_controller.h"

class StoreMenuController : public Escher::ModalViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate, public Escher::PervasiveBox {
public:
  StoreMenuController();
  void setText(const char * text);

  // PervasiveBox
  void open() override;

  // Responder
  void didBecomeFirstResponder() override;

  // ListViewDataSource
  int numberOfRows() const override { return 1; }
  int reusableCellCount(int type) override { return 1; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int typeAtIndex(int index) const override { return 0; }
  Escher::HighlightCell * reusableCell(int index, int type) override { return &m_cell; }

  // LayoutFieldDelegate
  bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(Escher::LayoutField * layoutField) override;

  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::AbstractTextField * textField) override;

private:
  class InnerListController : public ViewController {
  public:
    InnerListController(StoreMenuController * dataSource, Escher::SelectableTableViewDelegate * delegate = nullptr);
    const char * title() override { return I18n::translate(I18n::Message::DefineVariable); }
    Escher::View * view() override { return &m_selectableTableView; }
    void didBecomeFirstResponder() override;
    Escher::SelectableTableView * selectableTableView() { return &m_selectableTableView; }
  private:
    Escher::SelectableTableView m_selectableTableView;
  };

  bool parseAndStore(const char * text);
  void openAbortWarning();
  Escher::StackViewController m_stackViewController;
  InnerListController m_listController;
  Escher::EditableExpressionCell m_cell;
  Escher::MessagePopUpControllerWithCustomCancel m_abortController;
  bool m_preventReload;
};

#endif
