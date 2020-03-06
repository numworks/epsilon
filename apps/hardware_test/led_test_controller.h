#ifndef HARDWARE_TEST_LED_TEST_CONTROLLER_H
#define HARDWARE_TEST_LED_TEST_CONTROLLER_H

#include <escher.h>
#include "arrow_view.h"

namespace HardwareTest {

class LEDTestController : public ViewController {
public:
  LEDTestController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public SolidColorView {
  public:
    ContentView();
    SolidColorView * LEDColorIndicatorView();
  private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    SolidColorView m_ledColorIndicatorView;
    SolidColorView m_ledColorOutlineView;
    BufferTextView m_ledView;
    ArrowView m_arrowView;
  };
  void setLEDColor(KDColor color);
  constexpr static KDCoordinate k_arrowLength = 100;
  constexpr static KDCoordinate k_arrowMargin = 20;
  constexpr static KDCoordinate k_indicatorSize = 20;
  constexpr static KDCoordinate k_indicatorMargin = 8;
  constexpr static int k_numberOfColors = 5;
  constexpr static KDColor k_LEDColors[k_numberOfColors] = {KDColorWhite, KDColorRed, KDColorBlue, KDColorGreen, KDColorBlack};
  static KDColor LEDColorAtIndex(int i);
  ContentView m_view;
  int m_LEDColorIndex;
};

}

#endif

