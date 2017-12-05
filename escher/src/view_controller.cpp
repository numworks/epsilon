#include <escher/view_controller.h>
#include <poincare/complex.h>
#include <string.h>

ViewController::ViewController(Responder * parentResponder) :
  Responder(parentResponder)
{
}

const char * ViewController::title() {
  return nullptr;
}

void ViewController::viewWillAppear() {
}

void ViewController::viewDidDisappear() {
}

void ViewController::convertFloatToText(double value, char * buffer, const char * header, size_t headerLength, const char * trailer, int precision) {
  strcpy(buffer, header);
  int numberOfChar = headerLength;
  numberOfChar += Poincare::PrintFloat::convertFloatToText<double>(value, buffer+numberOfChar, Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(precision), precision);
  strcpy(buffer+numberOfChar, trailer);
}

