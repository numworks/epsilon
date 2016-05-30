extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <ion.h>
}
#include <escher.h>

/*
class App {
public:
  void run();
protected:
  ViewController * rootViewController() = 0;
private:
  View m_window;
};

void App::run() {
  self.window = rootViewController()->view();
  self.responder = rootViewController()->responder();
}

class GraphApp : public App {
public:
  GraphApp();
  void run() override;
protected:
  ViewController * rootViewController() override;
private:
  FunctionController m_functionController;
  GraphController m_graphController;
  TabViewController m_tabController;
};

class FunctionStore {
};

class FunctionController : public ViewController {

};

class GraphController : public ViewController {
};

GraphApp::GraphApp() {
  m_functionController = FunctionController();
  m_graphController = GraphController();
  ViewController * tabs[] = {
    &m_functionController,
    &m_graphController
  };
  m_tabController = TabViewController(tabs);
}
*/

/*
class MyTextView : public View {
  using View::View;
public:
  void drawRect(KDRect frame) override;
};

void MyTextView::drawRect(KDRect frame) {
  KDPoint zero = {0, 0};
  KDDrawString("Hello, world!", zero, 0);
}
*/

class GraphView : public ChildlessView {
public:
  void drawRect(KDRect rect) const override;
};

void GraphView::drawRect(KDRect rect) const {
  KDColor bg = 0x99;
  KDFillRect(rect, bg);
  for (KDCoordinate x=rect.x; x<rect.x+rect.width; x++) {
    KDSetPixel((KDPoint){x, (KDCoordinate)(x*x/rect.width)}, 0x00);
  }
}

class GraphViewController : public ViewController {
public:
  using ViewController::ViewController;
  View * view() override;
  char * title() override;
private:
  GraphView m_view;
};


View * GraphViewController::view() {
  return &m_view;
}

char * GraphViewController::title() {
  return "Graph";
}

class DemoViewController : public ViewController {
public:
  DemoViewController(KDColor c);
  View * view() override;
  char * title() override;
private:
  SolidColorView m_view;
};

DemoViewController::DemoViewController(KDColor c) :
m_view(SolidColorView(c))
{
}

View * DemoViewController::view() {
  return &m_view;
}

char * DemoViewController::title() {
  return "HELLO";
}

class MyTestApp : public App {
public:
  MyTestApp();
protected:
  ViewController * rootViewController() override;
private:
  DemoViewController m_demoViewController;
  GraphViewController m_graphViewController;
  TabViewController m_tabViewController;
};

MyTestApp::MyTestApp() :
  m_demoViewController(DemoViewController(0x55)),
  m_graphViewController(GraphViewController()),
  m_tabViewController(&m_demoViewController, &m_graphViewController)
{
}

ViewController * MyTestApp::rootViewController() {
  return &m_tabViewController;
}

void ion_app() {

  //KDDrawString("Hello", {0,0}, 0);

  //KDFillRect({0,0,100,100}, 0x55);
  //KDFillRect({100,100,100,100}, 0x99);

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
