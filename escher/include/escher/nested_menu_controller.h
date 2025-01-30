#ifndef ESCHER_NESTED_MENU_CONTROLLER_H
#define ESCHER_NESTED_MENU_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/container.h>
#include <escher/editable_fiel_help_box.h>
#include <escher/editable_field.h>
#include <escher/highlight_cell.h>
#include <escher/list_view_data_source.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/selectable_list_view.h>
#include <escher/stack_view_controller.h>
#include <ion.h>
#include <omg/ring_buffer.h>

namespace Escher {

class NestedMenuController : public StackViewController::Custom<5>,
                             public StandardMemoizedListViewDataSource,
                             public SelectableListViewDataSource,
                             public SelectableListViewDelegate,
                             public EditableFieldHelpBox {
 public:
  NestedMenuController(Responder* parentResponder,
                       I18n::Message title = (I18n::Message)0);
  void setTitle(I18n::Message title) { m_listController.setTitle(title); }

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  HighlightCell* reusableCell(int index, int type) override;

  void open() override;

 protected:
  using NodeCell = MenuCell<MessageTextView, EmptyCellWidget, ChevronView>;

  void handleResponderChainEvent(
      Responder::ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::BecameFirst) {
      App::app()->setFirstResponder(&m_listController);
    } else {
      StackViewController::handleResponderChainEvent(event);
    }
  }

  class StackState {
   public:
    StackState(int selectedRow = -1, KDCoordinate verticalScroll = 0)
        : m_selectedRow(selectedRow), m_verticalScroll(verticalScroll) {}
    int selectedRow() const { return m_selectedRow; }
    KDCoordinate verticalScroll() const { return m_verticalScroll; }

   private:
    int m_selectedRow;
    KDCoordinate m_verticalScroll;
  };

  // A state is needed for all StackView children but the first
  constexpr static int k_maxModelTreeDepth = k_maxNumberOfChildren - 1;
  constexpr static int k_leafCellType = 0;
  constexpr static int k_nodeCellType = 1;
  int stackDepth() { return m_stack.length(); }
  virtual bool selectSubMenu(int selectedRow);
  virtual bool returnToPreviousMenu();
  virtual bool returnToRootMenu();
  virtual bool selectLeaf(int selectedRow) = 0;
  virtual HighlightCell* leafCellAtIndex(int index) = 0;
  virtual NodeCell* nodeCellAtIndex(int index) = 0;
  virtual I18n::Message subTitle() = 0;
  SelectableListView m_selectableListView;
  OMG::RingBuffer<StackState, k_maxModelTreeDepth>* stack() { return &m_stack; }
  virtual int controlChecksum() const { return 0; }
  virtual bool isToolbox() const { return false; }

 private:
  class BreadcrumbController : public ViewController {
   public:
    BreadcrumbController(Responder* parentResponder,
                         SelectableListView* tableView)
        : ViewController(parentResponder),
          m_selectableListView(tableView),
          m_titleCount(0),
          m_titleBuffer("") {}
    const char* title() const override { return m_titleBuffer; }
    void popTitle();
    void pushTitle(I18n::Message title);
    void resetTitle();
    View* view() override { return m_selectableListView; }

   private:
    constexpr static int k_maxTitleLength =
        (Ion::Display::Width - Metric::PopUpMargins.width() -
         2 * Metric::CellSeparatorThickness - Metric::CellMargins.width()) /
        KDFont::GlyphWidth(KDFont::Size::Small);
    constexpr static int k_maxModelTreeDepth =
        StackViewController::Custom<5>::k_maxNumberOfChildren - 1;
    void updateTitle();
    SelectableListView* m_selectableListView;
    int m_titleCount;
    I18n::Message m_titles[k_maxModelTreeDepth];
    char m_titleBuffer[k_maxTitleLength + 1];
  };

  class ListController : public ViewController {
   public:
    ListController(Responder* parentResponder, SelectableListView* tableView,
                   I18n::Message title)
        : ViewController(parentResponder),
          m_selectableListView(tableView),
          m_title(title) {}
    const char* title() const override { return I18n::translate(m_title); }
    void setTitle(I18n::Message title) { m_title = title; }
    View* view() override { return m_selectableListView; }
    void handleResponderChainEvent(ResponderChainEvent event) override;

   private:
    SelectableListView* m_selectableListView;
    I18n::Message m_title;
  };

  StackState currentState() const {
    return StackState(selectedRow(), m_selectableListView.contentOffset().y());
  }
  void loadState(StackState state);
  // NestedMenuController implementations will not interact with the app pages
  void didExitPage(ViewController* controller) const override {}
  void willOpenPage(ViewController* controller) const override {}
  BreadcrumbController m_breadcrumbController;
  ListController m_listController;
  OMG::RingBuffer<StackState, k_maxModelTreeDepth> m_stack;
  int m_savedChecksum;
  constexpr static int k_nestedMenuStackDepth = 1;
};

}  // namespace Escher
#endif
