extern "C" {
#include <assert.h>
#include <escher.h>
#include <stdlib.h>
#include <ion.h>
}

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

void ion_app() {

  //KDDrawString("Hello", {0,0}, 0);

  //KDFillRect({0,0,100,100}, 0x55);
  //KDFillRect({100,100,100,100}, 0x99);

  Window window;
  DemoViewController v1 = DemoViewController(0x55);
  DemoViewController v2 = DemoViewController(0x99);
  ViewController * tabs[]= { &v1, &v2};
  TabViewController tabVC = TabViewController(tabs, 2);

  window.setFrame({{0,0}, {200, 200}});
  window.setSubview(tabVC.view(), 0);
  tabVC.view()->setFrame(window.bounds());


  int i = 0;

  while(true) {
    tabVC.setActiveTab(i);
    ion_event_t event = ion_get_event();
    tabVC.handleKeyEvent(event);
    i = (i+1)%2;
  }



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
