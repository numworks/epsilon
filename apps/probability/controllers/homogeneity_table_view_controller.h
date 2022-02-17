#ifndef PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/input_homogeneity_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/models/statistic/homogeneity_test.h"

namespace Probability {

// TODO: change name InputHomogeneityTableViewController
// TODO: Factorize with EditableCellTableViewController?
class HomogeneityTableViewController : public TableViewController, DynamicCellsDataSourceDelegate<EvenOddEditableTextCell>, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> {
public:
  HomogeneityTableViewController(Escher::Responder * parent,
                                 HomogeneityTest * statistic,
                                 DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate,
                                 Escher::SelectableTableViewDelegate * tableDelegate);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  Escher::SelectableTableView * selectableTableView() override { return &m_table; }
  Escher::TableViewDataSource * tableViewDataSource() override { return &m_tableData; }

  void initCell(EvenOddBufferTextCell, void * cell, int index) override;
  void initCell(EvenOddEditableTextCell, void * cell, int index) override;
  // TODO factorize with GoodnessTableViewController in TableViewController
  void recomputeDimensions() override { m_tableData.recomputeDimensions(); }
  Escher::SelectableTableView * tableView() override { return &m_table; }

private:
  void deleteSelectedValue();
  Escher::SelectableTableViewDataSource m_selectionDataSource;
  InputHomogeneityDataSource m_tableData;
  SelectableTableViewWithBackground m_table;
  HomogeneityTest * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H */
