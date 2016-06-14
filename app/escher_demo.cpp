extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <ion.h>
}
#include <escher.h>

class MyFunCell : public ChildlessView {
public:
  MyFunCell();
  void drawRect(KDRect rect) const override;
};

MyFunCell::MyFunCell() :
  ChildlessView()
{
}

void MyFunCell::drawRect(KDRect rect) const {
  KDDrawString("Cell", KDPointZero, 0);
}

class ListController : public ViewController, public TableViewDataSource {
public:
  ListController();
  View * view() override;
  const char * title() const override;
  bool handleEvent(ion_event_t event) override;

  int numberOfCells() override;
  void willDisplayCellForIndex(View * cell, int index) override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_maxNumberOfCells = 10;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the tableview!
  MyFunCell m_cells[k_maxNumberOfCells];
  TableView m_tableView;
};

ListController::ListController() :
  ViewController(),
  m_tableView(TableView(this))
{
}

View * ListController::view() {
  return &m_tableView;
}

const char * ListController::title() const {
  return "List";
}

bool ListController::handleEvent(ion_event_t event) {
  if (event != ENTER) {
    return false;
  }
  m_tableView.scrollToRow(0);
  return true;
}

int ListController::numberOfCells() {
  return 3;
};

View * ListController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_maxNumberOfCells);
  return &m_cells[index];
}

int ListController::reusableCellCount() {
  return k_maxNumberOfCells;
}

void ListController::willDisplayCellForIndex(View * cell, int index) {
}

KDCoordinate ListController::cellHeight() {
  return 40;
}

class DemoViewController : public ViewController {
public:
  DemoViewController(KDColor c);
  View * view() override;
  const char * title() const override;
private:
  SolidColorView m_view;
};

DemoViewController::DemoViewController(KDColor c) :
  ViewController(),
  m_view(SolidColorView(c))
{
}

View * DemoViewController::view() {
  return &m_view;
}

const char * DemoViewController::title() const {
  return "HELLO";
}

class MyTestApp : public App {
public:
  MyTestApp();
protected:
  ViewController * rootViewController() override;
private:
  DemoViewController m_demoViewController;
  ListController m_listViewController;
  TabViewController m_tabViewController;
};

MyTestApp::MyTestApp() :
  App(),
  m_demoViewController(DemoViewController(0x1235)),
  m_listViewController(ListController()),
  m_tabViewController(&m_demoViewController, &m_listViewController)
{
}

ViewController * MyTestApp::rootViewController() {
  return &m_tabViewController;
}

void ion_app() {

  //KDDrawString("Hello", {0,0}, 0);

  //KDFillRect({0,0,100,100}, 0x55);
  //KDFillRect({100,100,100,100}, 0x99);

  /*
  KDCoordinate i = 0;
  char message[4] = {'F', 'O', 'O', 0};
  while (1) {
    KDPoint p = {i, i};
    KDDrawString(message, p, false);
    ion_event_t event = ion_get_event();
    if (event < 0x100) {
      message[1] = event;
    }
    i+= 1;
  }
  */

  MyTestApp myApp = MyTestApp();

  myApp.run();

  /*
  int i = 0;

  while(true) {
#if ESCHER_VIEW_LOGGING
    std::cout << window << std::endl;
#endif
    tabVC.setActiveTab(i);
    ion_event_t event = ion_get_event();
    tabVC.handleKeyEvent(event);
    i = (i+1)%2;
  }
  */



  /*
  static GraphApp app = GraphApp();
  app.run();
  */
  /*
  KDRect wholeScreen = {0, 0, 100, 100};
  KDColor a = 0x55;
  View * window = new SolidColorView(wholeScreen, a);
  KDRect textRect = {0, 0, 50, 50};
  MyTextView * textView = new MyTextView(textRect);
  window->addSubview(textView);

  //window->draw();

  while (1) {
    ion_sleep();
    textRect.y = textRect.y+1;
    textView->setFrame(textRect);
  }

  delete textView;
  delete window;
  */
}
