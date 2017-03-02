#ifndef APPS_VARIABLE_BOX_CONTROLLER_H
#define APPS_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "variable_box_leaf_cell.h"

class VariableBoxController : public StackViewController {
public:
  VariableBoxController(Poincare::Context * context);
  void didBecomeFirstResponder() override;
  void setTextFieldCaller(TextField * textField);
  void viewWillAppear() override;
  void viewWillDisappear() override;
private:
  class ContentViewController : public ViewController, public ListViewDataSource {
  public:
    ContentViewController(Responder * parentResponder, Poincare::Context * context);
    View * view() override;
    const char * title() const override;
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
    void deselectTable();
  private:
    enum class Page {
      RootMenu,
      Scalar,
      List,
      Matrix
    };
    constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
    constexpr static int k_numberOfMenuRows = 3; //240/40
    constexpr static KDCoordinate k_leafRowHeight = 40;
    constexpr static KDCoordinate k_nodeRowHeight = 40;
    Page pageAtIndex(int index);
    void putLabelAtIndexInBuffer(int index, char * buffer);
    const char * nodeLabelAtIndex(int index);
    const Poincare::Expression * expressionForIndex(int index);

    Poincare::Context * m_context;
    TextField * m_textFieldCaller;
    int m_firstSelectedRow;
    int m_previousSelectedRow;
    Page m_currentPage;
    VariableBoxLeafCell m_leafCells[k_maxNumberOfDisplayedRows];
    PointerTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
    SelectableTableView m_selectableTableView;
  };
  ContentViewController m_contentViewController;
};

#endif
