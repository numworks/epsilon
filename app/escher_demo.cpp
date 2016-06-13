extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <ion.h>
}
#include <escher.h>

class GraphView : public ChildlessView {
public:
  void drawRect(KDRect rect) const override;
};

void GraphView::drawRect(KDRect rect) const {
  /*
  std::cout << "DrawingRect of GraphView in " <<
   rect.x << "," << rect.y << "," <<
   rect.width << "," << rect.height << "," <<
   std::endl;
   */
  KDColor bg = 0xFFFF;
  KDFillRect(rect, bg);
  for (KDCoordinate x=rect.x; x<rect.x+rect.width; x++) {
    KDSetPixel((KDPoint){x, (KDCoordinate)(x*x/rect.width)}, 0x00);
  }
}

class GraphViewController : public ViewController {
public:
  GraphViewController();
  View * view() override;
  const char * title() const override;
private:
  GraphView m_graphView;
  // It's best to declare m_graphView *before* m_scrollView
  // Otherwise we cannot use m_graphView in the m_scrollView constructor
  // Well, we can have the pointer but it points to an unitialized object
  ScrollView m_scrollView;
};

GraphViewController::GraphViewController() :
  ViewController(),
  m_graphView(GraphView()),
  m_scrollView(ScrollView(&m_graphView))
{
}

View * GraphViewController::view() {
  return &m_scrollView;
}

const char * GraphViewController::title() const {
  return "Graph";
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
  GraphViewController m_graphViewController;
  TabViewController m_tabViewController;
};

MyTestApp::MyTestApp() :
  App(),
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
