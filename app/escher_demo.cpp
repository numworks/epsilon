extern "C" {
#include <assert.h>
#include <escher.h>
#include <stdlib.h>
#include <ion.h>
}

class MyTextView : public View {
  using View::View;
public:
  void drawRect(KDRect frame) override;
};

void MyTextView::drawRect(KDRect frame) {
  KDPoint zero = {0, 0};
  KDDrawString("Hello, world!", zero, 0);
}

void ion_app() {
  KDRect wholeScreen = {0, 0, 100, 100};
  KDColor a = 0x55;
  View * window = new SolidColorView(wholeScreen, a);
  KDRect textRect = {0, 0, 50, 50};
  MyTextView * textView = new MyTextView(textRect);
  window->addSubview(textView);

  window->draw();

  while (1) {
    ion_sleep();
    textRect.y = textRect.y+1;
    textView->setFrame(textRect);
  }

  delete textView;
  delete window;
}
