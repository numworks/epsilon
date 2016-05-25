#include <escher/tab_view.h>
#include <escher/tab_view_controller.h>

TabView::TabView(TabViewController * tabViewController) :
  View(),
  m_tabViewController(tabViewController)
{
  /*
  m_numberOfTabs = tabViewController->numberOfTabs();
  for (uint8_t i=0; i<m_numberOfTabs; i++) {
    //m_cells[i] = TabViewCell(name);
    //FIXME: use setName
  }
  */
}

int TabView::numberOfSubviews() {
  return 0; //FIXME
}

View * TabView::subview(int index) {
  return nullptr;
}

void TabView::layoutSubviews() {
  // TODO
}

void TabView::storeSubviewAtIndex(View * view, int index) {
  // TODO
}
