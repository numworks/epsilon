#ifndef PYTHON_PORT_H
#define PYTHON_PORT_H

extern "C" {
#include <stddef.h>
#include <py/obj.h>
}
#include <escher/view_controller.h>


namespace MicroPython {

class ScriptProvider {
public:
  virtual const char * contentOfScript(const char * name, bool markAsFetched) = 0;
};

class ExecutionEnvironment {
public:
  ExecutionEnvironment() {}
  static ExecutionEnvironment * currentExecutionEnvironment();
  bool runCode(const char * );
  virtual const char * inputText(const char * prompt) { return nullptr; }

  // Sandbox
  void displaySandbox() { displayViewController(sandbox()); }
  virtual ViewController * sandbox() { return nullptr; }
  virtual void resetSandbox() {}

  // Generic View Controller
  virtual void displayViewController(ViewController * controller) {}
  virtual void hideAnyDisplayedViewController() {}

  virtual void printText(const char * text, size_t length) {}
  virtual void refreshPrintOutput() {}
  void interrupt();
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);
void collectRootsAtAddress(char * address, int len);

class Color {
public:
  enum class Mode {
    MaxIntensity1 = 1,
    MaxIntensity255 = 255,
  };

  static KDColor Parse(mp_obj_t input, Mode Mode = Mode::MaxIntensity255);
private:
  class NamedColor {
  public:
    constexpr NamedColor(const char * name, KDColor color) :
      m_name(name),
      m_color(color)
    {}
    const char * name() const { return m_name; }
    KDColor color() const { return m_color; }
  private:
    const char * m_name;
    KDColor m_color;
  };
};


}

#endif
