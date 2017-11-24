#ifndef APPS_VARIABLE_BOX_CONTROLLER_H
#define APPS_VARIABLE_BOX_CONTROLLER_H

#define MATRIX_VARIABLES 1

#include <escher.h>
#include <poincare.h>
#include "variable_box_leaf_cell.h"
#include "i18n.h"

class VariableBoxController : public StackViewController {
public:
  VariableBoxController(Poincare::GlobalContext * context);
  void didBecomeFirstResponder() override;
  void setTextFieldCaller(TextField * textField);
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentViewController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
  public:
    ContentViewController(Responder * parentResponder, Poincare::GlobalContext * context);
    View * view() override;
    const char * title() override;
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event event) override;
    int numberOfRows() override;
    HighlightCell * reusableCell(int index, int type) override;
    int reusableCellCount(int type) override;
    void willDisplayCellForIndex(HighlightCell * cell, int index) override;
    KDCoordinate rowHeight(int j) override;
    KDCoordinate cumulatedHeightFromIndex(int j) override;
    int indexFromCumulatedHeight(KDCoordinate offsetY) override;
    int typeAtLocation(int i, int j) override;
    void setTextFieldCaller(TextField * textField);
    void reloadData();
    void resetPage();
    void viewDidDisappear() override;
  private:
    enum class Page {
      RootMenu,
      Scalar,
#if LIST_VARIABLES
      List,
#endif
      Matrix
    };
    constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
#if LIST_VARIABLES
    constexpr static int k_numberOfMenuRows = 3;
#else
    constexpr static int k_numberOfMenuRows = 2;
#endif
    constexpr static KDCoordinate k_leafMargin = 10;
    Page pageAtIndex(int index);
    void putLabelAtIndexInBuffer(int index, char * buffer);
    I18n::Message nodeLabelAtIndex(int index);
    const Poincare::Expression * expressionForIndex(int index);

    Poincare::GlobalContext * m_context;
    TextField * m_textFieldCaller;
    int m_firstSelectedRow;
    int m_previousSelectedRow;
    Page m_currentPage;
    VariableBoxLeafCell m_leafCells[k_maxNumberOfDisplayedRows];
    MessageTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
    SelectableTableView m_selectableTableView;
  };
  ContentViewController m_contentViewController;
};

#endif
