#ifndef HARDWARE_TEST_LED_TEST_CONTROLLER_H
#define HARDWARE_TEST_LED_TEST_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

#include "arrow_view.h"

namespace HardwareTest {

class LEDTestController : public Escher::ViewController {
 public:
  LEDTestController(Escher::Responder* parentResponder);
  Escher::View* view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    ContentView();
    Escher::SolidColorView* LEDColorIndicatorView();

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    Escher::View* subviewAtIndex(int index) override;
    Escher::SolidColorView m_ledColorIndicatorView;
    Escher::SolidColorView m_ledColorOutlineView;
    Escher::BufferTextView m_ledView;
    ArrowView m_arrowView;
  };
  void setLEDColor(KDColor color);
  constexpr static KDCoordinate k_arrowLength = 100;
  constexpr static KDCoordinate k_arrowMargin = 20;
  constexpr static KDCoordinate k_indicatorSize = 20;
  constexpr static KDCoordinate k_indicatorMargin = 8;
  constexpr static int k_numberOfColors = 5;
  constexpr static KDColor k_LEDColors[k_numberOfColors] = {
      KDColorWhite, KDColorRed, KDColorBlue, KDColorGreen, KDColorBlack};
  static KDColor LEDColorAtIndex(int i);
  ContentView m_view;
  int m_LEDColorIndex;
};

}  // namespace HardwareTest

#endif
